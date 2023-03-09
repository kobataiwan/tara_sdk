#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>

#include "nc_hi_common.h"
#include <linux/fb.h>
#include "hifb.h"


#include "nc_hi_vivo.h"

#include <_debug_macro.h>

/*******************************************************************************
*	Description		: Get YCbCr value(string information)
*	Argurments		:
*	Return value	:
*	Modify			:
*	warning			:
*******************************************************************************/
//#define USE_PHYSICAL_ADDR
#define MAX_Y_BLOCK_CNT		2//0 //2	// Y Block count is macro block
#define MAX_VI_SKIP_FRAME	10
#define MAX_V0_SKIP_FRAME	10

#define MAX_FRM_CNT_ 256
#define MAX_FRM_WIDTH   4096

// YUV data type
#define YUV_DATA_TYPE_VI	 	0x00
#define YUV_DATA_TYPE_VO	 	0x01

static HI_S32 s_s32MemDev = -1;

static HI_VOID * __COMM_SYS_Mmap(HI_U32 u32PhyAddr, HI_U32 u32Size)
{
    HI_U32 u32Diff;
    HI_U32 u32PagePhy;
    HI_U32 u32PageSize;
    HI_U8 * pPageAddr;

    /* The mmap address should align with page */
    u32PagePhy = u32PhyAddr & 0xfffff000;
    u32Diff    = u32PhyAddr - u32PagePhy;

    /* The mmap size shuld be mutliples of 1024 */
    u32PageSize = ((u32Size + u32Diff - 1) & 0xfffff000) + 0x1000;
    pPageAddr   = mmap ((void *)0, u32PageSize, PROT_READ|PROT_WRITE,
                                    MAP_SHARED, s_s32MemDev, u32PagePhy);
    if (MAP_FAILED == pPageAddr )
    {
        perror("mmap error");
        return NULL;
    }
    return (HI_VOID *) (pPageAddr + u32Diff);
}

static HI_S32 __COMM_SYS_Munmap(HI_VOID* pVirAddr, HI_U32 u32Size)
{
    HI_U32 u32PageAddr;
    HI_U32 u32PageSize;
    HI_U32 u32Diff;

    u32PageAddr = (((HI_U32)pVirAddr) & 0xfffff000);
    u32Diff     = (HI_U32)pVirAddr - u32PageAddr;
    u32PageSize = ((u32Size + u32Diff - 1) & 0xfffff000) + 0x1000;

    return munmap((HI_VOID*)u32PageAddr, u32PageSize);
}



#define MEM_DEV_OPEN___() \
do {\
    	if (s_s32MemDev <= 0)\
        {\
            s_s32MemDev = open("/dev/mem", O_CREAT|O_RDWR|O_SYNC);\
            if (s_s32MemDev < 0)\
            {\
                perror("Open dev/mem error");\
                return;\
            }\
        }\
}while(0)

#define __MEM_DEV_CLOSE() \
do {\
        HI_S32 s32Ret;\
    	if (s_s32MemDev > 0)\
        {\
            s32Ret = close(s_s32MemDev);\
            if(HI_SUCCESS != s32Ret)\
            {\
                perror("Close mem/dev Fail");\
                return s32Ret;\
            }\
            s_s32MemDev = -1;\
        }\
}while(0)



int GetLineData_YCbCr( int yline, VIDEO_FRAME_S * pVBuf, unsigned char *pDst, int data_type, int nCnt_W, int nCnt_H, int *pRet_W, int *pRet_H )
{
	int x=0;
	int y=yline;
	int ret = 0;
	int Y = 0;
	int Cb = 0;
	int Cr = 0;

	int ReadCntTot = nCnt_W;

	if( ReadCntTot > pVBuf->u32Width )
		ReadCntTot = pVBuf->u32Width;


	if(pRet_W)
		*pRet_W = ReadCntTot;

	int ReadCntTot_H = nCnt_H;

	if( ReadCntTot_H > pVBuf->u32Height )
		ReadCntTot_H = pVBuf->u32Height;


	if(pRet_H)
		*pRet_H = ReadCntTot_H;

	/* Analysis YCbCr data with Mouse x, y position */
	unsigned int w = 0, h = 0;
	char * pVBufVirt_Y;
	char * pVBufVirt_C;
	char * pMemContent;
	unsigned char TmpBuff[MAX_FRM_WIDTH];
	HI_U32 phy_addr,size = 0;
	HI_CHAR *pUserPageAddr[2];
	PIXEL_FORMAT_E  enPixelFormat = pVBuf->enPixelFormat;
	HI_U32 u32UvHeight = 0;

	if (pVBuf->u32Width > MAX_FRM_WIDTH)
	{
		printf(">>>>> APP : Over max frame width: %d, can't support.\n", MAX_FRM_WIDTH);
		return -1;
	}

	if (PIXEL_FORMAT_YUV_SEMIPLANAR_420 == enPixelFormat)
	{
		DBG_INFO("420\n");
		size = (pVBuf->u32Stride[0])*(pVBuf->u32Height)*3/2;
		u32UvHeight = pVBuf->u32Height/2;
	}
	else if(PIXEL_FORMAT_YUV_SEMIPLANAR_422 == enPixelFormat)
	{
		DBG_INFO("422\n");
		size = (pVBuf->u32Stride[0])*(pVBuf->u32Height)*2;
		u32UvHeight = pVBuf->u32Height;
	}
	else
	{
		size = (pVBuf->u32Stride[0])*(pVBuf->u32Height);
		u32UvHeight = 0;
	}

	phy_addr = pVBuf->u32PhyAddr[0];
	pUserPageAddr[0] = (HI_CHAR *) __COMM_SYS_Mmap(phy_addr, size);
	if (NULL == pUserPageAddr[0])
	{
		printf(">>>>> APP : failed getting page address.\n");
		return -1;
	}

	pVBufVirt_Y = pUserPageAddr[0];
	pVBufVirt_C = pVBufVirt_Y + (pVBuf->u32Stride[0])*(pVBuf->u32Height);

	int ii;
	for(ii=0;ii<ReadCntTot_H;ii++)
	{
		x=0;
		int jj;
		for(jj=0;jj<ReadCntTot;jj++)
		{
			pMemContent = pVBufVirt_Y + y*pVBuf->u32Stride[0] + x;
			Y = pMemContent[0];

			int adjust_x = (x%2 == 0) ? x : x-1;
			int adjust_y = (y > pVBuf->u32Height-2 ) ? pVBuf->u32Height-1 : y;
			adjust_x = ( adjust_x > pVBuf->u32Width-3 ) ? pVBuf->u32Width-1 : adjust_x;
			/*
			 * get Cb value
			 */
			pMemContent = pVBufVirt_C + adjust_y*pVBuf->u32Stride[1] + adjust_x;
			pMemContent += 1;
			Cb = pMemContent[0];

			/*
			 * get Cr value
			 */
			pMemContent = pVBufVirt_C + adjust_y*pVBuf->u32Stride[1] + adjust_x;
			Cr = pMemContent[0];

			/* set YCbCr infomration to buffer */
			*pDst++ = Y;//(Y << 2);

			x++;
		}
		y++;
	}

	/* umap */
	__COMM_SYS_Munmap(pUserPageAddr[0], size);


	return 0;
}

int TestCapFrameLine(int yline, unsigned char *pDst, int nCnt_W, int nCnt_H, int *pRet_W, int *pRet_H )
{
	VIDEO_FRAME_INFO_S astFrame[MAX_FRM_CNT_];
	VIDEO_FRAME_INFO_S astFrameVO[MAX_FRM_CNT_];

	unsigned int ret;

	memset( &astFrame[0], 0x00, sizeof(VIDEO_FRAME_INFO_S) );

	VI_CHN ViChn = 0;
	VO_CHN VoChn = 0;

	HI_S32 s32MilliSec = 2000;

	ViChn = 0;
	VoChn = 0;

	DBG_INFO("Start\n");

	/* set frame depth */
	if (HI_MPI_VI_SetFrameDepth(ViChn, 1))
	{
		DBG_ERR("HI_MPI_VI_SetFrameDepth\n");
	}

	ret = HI_MPI_VI_GetFrame(ViChn, &astFrame[0], s32MilliSec);
	/* get raw data of VI */
	if(ret)
	{
		DBG_ERR("HI_MPI_VI_GetFrame [%08x]\n", ret);
		//printf("HI_MPI_VI_GetFrame err, vi chn %d \n", ViChn);
		//get_video_vi = -1;
		return -1;
	}

	/* release frame after using */
	HI_MPI_VI_ReleaseFrame(ViChn, &astFrame[0]);

	char DstStrBuf[256];

	int ret_nCnt = 0;
	MEM_DEV_OPEN___();
	GetLineData_YCbCr( yline,  &astFrame[0].stVFrame, pDst, YUV_DATA_TYPE_VI, nCnt_W, nCnt_H, pRet_W, pRet_H  );
	__MEM_DEV_CLOSE();

	//DBG_INFO("%s\n",DstStrBuf);

	return 0;
}


int TestCapFrame(int x, int y)
{
	VIDEO_FRAME_INFO_S astFrame[MAX_FRM_CNT_];
	VIDEO_FRAME_INFO_S astFrameVO[MAX_FRM_CNT_];



	memset( &astFrame[0], 0x00, sizeof(VIDEO_FRAME_INFO_S) );

	VI_CHN ViChn = 0;
	VO_CHN VoChn = 0;

	HI_S32 s32MilliSec = 2000;

	ViChn = 0;
	VoChn = 0;

	DBG_INFO("Start\n");

	/* set frame depth */
	if (HI_MPI_VI_SetFrameDepth(ViChn, 1))
	{
		DBG_ERR("HI_MPI_VI_SetFrameDepth\n");
	}

	/* get raw data of VI */
	if (HI_MPI_VI_GetFrame(ViChn, &astFrame[0], s32MilliSec))
	{
		DBG_ERR("HI_MPI_VI_GetFrame\n");
		//printf("HI_MPI_VI_GetFrame err, vi chn %d \n", ViChn);
		//get_video_vi = -1;
		return -1;
	}

	/* release frame after using */
	HI_MPI_VI_ReleaseFrame(ViChn, &astFrame[0]);

	char DstStrBuf[256];

	MEM_DEV_OPEN___();
	__GetAnalysis_YCbCr( x, y, &astFrame[0].stVFrame, DstStrBuf, YUV_DATA_TYPE_VI );
	__MEM_DEV_CLOSE();

	DBG_INFO("%s\n",DstStrBuf);

	return 0;
}

int __GetAnalysis_YCbCr( int x, int y, VIDEO_FRAME_S * pVBuf, HI_CHAR *pyuvstring, int data_type )
{
	int ret = 0;
	int Y = 0;
	int Cb = 0;
	int Cr = 0;
#if 0
	/* is the display 1:1 size ? */
	if( g_vi_nc_analysis.Vo_mux_number != 0 )
	{
		fprintf( stdout, "The Display ch is not 1\n" );
		return -1;
	}
#endif
	if(1)
	{
		/* Analysis YCbCr data with Mouse x, y position */
	    unsigned int w = 0, h = 0;
	    char * pVBufVirt_Y;
	    char * pVBufVirt_C;
	    char * pMemContent;
	    unsigned char TmpBuff[MAX_FRM_WIDTH];
	    HI_U32 phy_addr,size = 0;
		HI_CHAR *pUserPageAddr[2];
	    PIXEL_FORMAT_E  enPixelFormat = pVBuf->enPixelFormat;
	    HI_U32 u32UvHeight = 0;

	    if (pVBuf->u32Width > MAX_FRM_WIDTH)
	    {
	        printf(">>>>> APP : Over max frame width: %d, can't support.\n", MAX_FRM_WIDTH);
	        return -1;
	    }

	    if (PIXEL_FORMAT_YUV_SEMIPLANAR_420 == enPixelFormat)
	    {
	    	DBG_INFO("420\n");
	        size = (pVBuf->u32Stride[0])*(pVBuf->u32Height)*3/2;
	        u32UvHeight = pVBuf->u32Height/2;
	    }
	    else if(PIXEL_FORMAT_YUV_SEMIPLANAR_422 == enPixelFormat)
	    {
	    	DBG_INFO("422\n");
	        size = (pVBuf->u32Stride[0])*(pVBuf->u32Height)*2;
	        u32UvHeight = pVBuf->u32Height;
	    }
	    else
	    {
	        size = (pVBuf->u32Stride[0])*(pVBuf->u32Height);
	        u32UvHeight = 0;
	    }

	    phy_addr = pVBuf->u32PhyAddr[0];
	    pUserPageAddr[0] = (HI_CHAR *) __COMM_SYS_Mmap(phy_addr, size);
	    if (NULL == pUserPageAddr[0])
	    {
	    	printf(">>>>> APP : failed getting page address.\n");
	        return -1;
	    }
	    //printf("stride0: %d,stride1:%d, width: %d\n",pVBuf->u32Stride[0],pVBuf->u32Stride[1] ,pVBuf->u32Width); // Andy
		pVBufVirt_Y = pUserPageAddr[0];
		pVBufVirt_C = pVBufVirt_Y + (pVBuf->u32Stride[0])*(pVBuf->u32Height);

		/*
		 * get Y value
		 */
		int averageY = 0;
		int x_p = 0, y_p = 0;
		int analysis_w = 0, analysis_h = 0;

		analysis_w = ((x - MAX_Y_BLOCK_CNT) < 0) ? 0 : (x - MAX_Y_BLOCK_CNT);	//adjust width
		analysis_h = ((y - MAX_Y_BLOCK_CNT) < 0) ? 0 : (y - MAX_Y_BLOCK_CNT);	//adjust height
		for( y_p = analysis_h; y_p < (analysis_h + (MAX_Y_BLOCK_CNT*2 + 1)); y_p++ )
		{
			for( x_p = analysis_w; x_p < (analysis_w + (MAX_Y_BLOCK_CNT*2 + 1)); x_p++ )
			{
				pMemContent = (pVBufVirt_Y + (y_p*pVBuf->u32Stride[0])) + x_p;
				averageY += pMemContent[0];
			}
		}
		if( MAX_Y_BLOCK_CNT == 0 )
			Y = averageY;
		else
			Y = averageY/( (MAX_Y_BLOCK_CNT*2 + 1)*(MAX_Y_BLOCK_CNT*2 + 1));
		//fprintf( stdout, "x:%d, y:%d, analysis_w:%d, analysis_h:%d\n", x, y, analysis_w, analysis_h);

		pMemContent = pVBufVirt_Y + y*pVBuf->u32Stride[0] + x;
		Y = pMemContent[0];
#if 1
		int adjust_x = (x%2 == 0) ? x : x-1;
		int adjust_y = (y > pVBuf->u32Height-2 ) ? pVBuf->u32Height-1 : y;
		adjust_x = ( adjust_x > pVBuf->u32Width-3 ) ? pVBuf->u32Width-1 : adjust_x;
		//fprintf( stdout, "adjust_x:%d, adjust_y:%d\n", adjust_x, adjust_y);
		/*
		 * get Cb value
		 */
		pMemContent = pVBufVirt_C + adjust_y*pVBuf->u32Stride[1] + adjust_x;
       	pMemContent += 1;
        Cb = pMemContent[0];

		/*
		 * get Cr value
		 */
        pMemContent = pVBufVirt_C + adjust_y*pVBuf->u32Stride[1] + adjust_x;
        Cr = pMemContent[0];
#else
		/*
		 * get Cb value
		 */
		int adjust_x = (x%2 == 0) ? x : x-1;
        pMemContent = pVBufVirt_C + y*pVBuf->u32Stride[1] + adjust_x;
       	pMemContent += 1;
        Cb = pMemContent[0];

		/*
		 * get Cr value
		 */
        pMemContent = pVBufVirt_C + y*pVBuf->u32Stride[1] + adjust_x;
        Cr = pMemContent[0];
#endif

        /* umap */
	    __COMM_SYS_Munmap(pUserPageAddr[0], size);

		/* set YCbCr infomration to buffer */
	    if( data_type == YUV_DATA_TYPE_VI )
	    {
	    	//Y = Y - 16;
	    	int r = Y + (1.4065 * (Cr - 128));
	    	int g = Y - (0.3455 * (Cb - 128)) - (0.7169 * (Cr - 128));
	    	int b = Y + (1.7790 * (Cb - 128));

			if (r < 0) r = 0;
			else if (r > 255) r = 255;
			if (g < 0) g = 0;
			else if (g > 255) g = 255;
			if (b < 0) b = 0;
			else if (b > 255) b = 255;

	    	sprintf( pyuvstring,  "(x:%d,y:%d) 0x%02X,0x%02X,0x%02X | %3d,%3d,%3d",x,y, Y, Cb, Cr, r,g,b );
			//sprintf( pyuvstring,  "VI-Y:%3d[0x%02X] Cb:%3d[0x%02X] Cr:%3d[0x%02X] W:%4d H:%4d", Y, Y, Cb, Cb, Cr, Cr, x, y );
	    }
	    else if( data_type == YUV_DATA_TYPE_VO )
	    {
	    	//sprintf( pyuvstring,  "VO-Y:%3d[0x%02X] Cb:%3d[0x%02X] Cr:%3d[0x%02X] W:%4d H:%4d", Y, Y, Cb, Cb, Cr, Cr, x, y );

	    }
	}

	return 0;
}

int TestCaptureFrame_SK(int x, int y)
{
	FILE *fp;
	char cmd[128] = {0, };
	int ii = 0;
	unsigned int ret;

	VIDEO_FRAME_INFO_S astFrame;

	memset( &astFrame, 0x00, sizeof(VIDEO_FRAME_INFO_S) );

	VI_CHN ViChn = 0;
	HI_S32 s32MilliSec = 2000;

	DBG_INFO("Start\n");

	for(ii=0; ii<4; ii++)
	{
		ViChn = ii*4;
		/* set frame depth */
		if (HI_MPI_VI_SetFrameDepth(ViChn, 1))
		{
			DBG_ERR("HI_MPI_VI_SetFrameDepth\n");
		}

		/* get raw data of VI */
		ret = HI_MPI_VI_GetFrame(ViChn, &astFrame, s32MilliSec);
		if(ret)
		{
			DBG_ERR("HI_MPI_VI_GetFrame[%08x]\n", ret);
			return -1;
		}

		MEM_DEV_OPEN___();
		YUV_Capture_Test_SK( ViChn, x, y, &astFrame.stVFrame, YUV_DATA_TYPE_VI );
		__MEM_DEV_CLOSE();

		/* release frame after using */
		HI_MPI_VI_ReleaseFrame(ViChn, &astFrame);
	}

	return 0;
}


int YUV_Capture_Test_SK( int Ch, int x, int y, VIDEO_FRAME_S * pVBuf, int data_type )
{
	FILE *fp;
	int ret = 0;
	int Y = 0;
	int Cb = 0;
	int Cr = 0;

	char cmd[128] = {0, };

	if(1)
	{
		/* Analysis YCbCr data with Mouse x, y position */
		unsigned int w = 0, h = 0;
		char * pVBufVirt_Y;
		char * pVBufVirt_C;
		char * pMemContent;
		unsigned char TmpBuff[MAX_FRM_WIDTH];
		HI_U32 phy_addr,size = 0;
		HI_CHAR *pUserPageAddr[2];
		PIXEL_FORMAT_E  enPixelFormat = pVBuf->enPixelFormat;
		HI_U32 u32UvHeight = 0;

		if (pVBuf->u32Width > MAX_FRM_WIDTH)
		{
			printf(">>>>> APP : Over max frame width: %d, can't support.\n", MAX_FRM_WIDTH);
			return -1;
		}

		if (PIXEL_FORMAT_YUV_SEMIPLANAR_420 == enPixelFormat)
		{
			DBG_INFO("YUV_420\n");
			size = (pVBuf->u32Stride[0])*(pVBuf->u32Height)*3/2;
			u32UvHeight = pVBuf->u32Height/2;
		}
		else if(PIXEL_FORMAT_YUV_SEMIPLANAR_422 == enPixelFormat)
		{
			DBG_INFO("YUV_422\n");
			size = (pVBuf->u32Stride[0])*(pVBuf->u32Height)*2;
			u32UvHeight = pVBuf->u32Height;
		}
		else
		{
			size = (pVBuf->u32Stride[0])*(pVBuf->u32Height);
			u32UvHeight = 0;
		}

		DBG_INFO("========= Ch%d YUV Capture Infomation =========\n", Ch);
		DBG_INFO("enPixelFormat::%d\n", enPixelFormat);
		DBG_INFO("Width::%d\n", pVBuf->u32Width);
		DBG_INFO("Height::%d\n", pVBuf->u32Height);
		DBG_INFO("u32Field::%d\n", pVBuf->u32Field);
		DBG_INFO("size::%d\n", size);
		DBG_INFO("u32UvHeight::%d\n", u32UvHeight);
		DBG_INFO("===============================================\n");

		phy_addr = pVBuf->u32PhyAddr[0];
		pUserPageAddr[0] = (HI_CHAR *) __COMM_SYS_Mmap(phy_addr, size);
		if (NULL == pUserPageAddr[0])
		{
			printf(">>>>> APP : failed getting page address.\n");
			return -1;
		}
		//printf("stride0: %d,stride1:%d, width: %d\n",pVBuf->u32Stride[0],pVBuf->u32Stride[1] ,pVBuf->u32Width); // Andy
		pVBufVirt_Y = pUserPageAddr[0];
		pVBufVirt_C = pVBufVirt_Y + (pVBuf->u32Stride[0])*(pVBuf->u32Height);

		sprintf( cmd, "/mnt/usb/ch%02d_Raptor3.yuv", Ch );

		fp = fopen( cmd, "wb" );
/*
		if( fp != NULL )
		{
			fwrite( pUserPageAddr[0], 1, size, fp );
		}
*/
 	   /* save Y ----------------------------------------------------------------*/
 	   fprintf(stderr, "saving......Y......");
 	   fflush(stderr);
 	   for(h=0; h<pVBuf->u32Height; h++)
 	   {
 	       pMemContent = pVBufVirt_Y + h*pVBuf->u32Stride[0];
		   fwrite( pMemContent, pVBuf->u32Width, 1, fp );
 	   }
    	fflush(fp);
#if 1
	    /* save U ----------------------------------------------------------------*/
	    if(u32UvHeight>0)
	    {
	        fprintf(stderr, "U......");
	        fflush(stderr);
	        for(h=0; h<u32UvHeight; h++)
	        {
	            pMemContent = pVBufVirt_C + h*pVBuf->u32Stride[1];

	            pMemContent += 1;

	            for(w=0; w<pVBuf->u32Width/2; w++)
	            {
	                TmpBuff[w] = *pMemContent;
	                pMemContent += 2;
	            }

	            fwrite(TmpBuff, pVBuf->u32Width/2, 1, fp);
	        }
	        fflush(fp);
	        /* save V ----------------------------------------------------------------*/

	        fprintf(stderr, "V......");
	        fflush(stderr);
	        for(h=0; h<u32UvHeight; h++)
	        {
	            pMemContent = pVBufVirt_C + h*pVBuf->u32Stride[1];

	            for(w=0; w<pVBuf->u32Width/2; w++)
	            {
	                TmpBuff[w] = *pMemContent;
	                pMemContent += 2;
	            }
	            fwrite(TmpBuff, pVBuf->u32Width/2, 1, fp);
	        }
	        fflush(fp);
	    }

	    fprintf(stderr, "done %d!\n", pVBuf->u32TimeRef);
	    fflush(stderr);
#endif

		fclose(fp);

		/* umap */
		__COMM_SYS_Munmap(pUserPageAddr[0], size);
	}

	return 0;
}

int VectorScope_VI_BuffSize_Get( VI_CHN Chn )
{
	unsigned int ret = 0;
	int Size = 0;

	VIDEO_FRAME_INFO_S astFrame;
	memset( &astFrame, 0x00, sizeof(VIDEO_FRAME_INFO_S) );

	VI_CHN ViChn = 0;
	HI_S32 s32MilliSec = 2000;

	ViChn = Chn * 4;

	/* set frame depth */
	if (HI_MPI_VI_SetFrameDepth(ViChn, 1))
	{
		DBG_ERR("HI_MPI_VI_SetFrameDepth\n");
		return -1;
	}

	/* get raw data of VI */
	ret = HI_MPI_VI_GetFrame(ViChn, &astFrame, s32MilliSec);
	if(ret)
	{
		DBG_ERR("HI_MPI_VI_GetFrame[%08x]\n", ret);
		return -1;
	}

	/* release frame after using */
	ret = HI_MPI_VI_ReleaseFrame(ViChn, &astFrame);
	if(ret)
	{
		DBG_ERR("HI_MPI_VI_ReleaseFrame[%08x]\n", ret);
		return -1;
	}

	MEM_DEV_OPEN___();
	VIDEO_FRAME_S * pVBuf = &astFrame.stVFrame;

	if(1)
	{
		unsigned int w = 0, h = 0;

		unsigned char TmpBuff[MAX_FRM_WIDTH];
		HI_U32 phy_addr = 0;
		HI_CHAR *pUserPageAddr[2];
		PIXEL_FORMAT_E  enPixelFormat = pVBuf->enPixelFormat;
		HI_U32 u32UvHeight = 0;

		if (pVBuf->u32Width > MAX_FRM_WIDTH)
		{
			printf(">>>>> APP : Over max frame width: %d, can't support.\n", MAX_FRM_WIDTH);
			return -1;
		}

		if (PIXEL_FORMAT_YUV_SEMIPLANAR_420 == enPixelFormat)
		{
//			DBG_INFO("420\n");
			Size = (pVBuf->u32Stride[0])*(pVBuf->u32Height)*3/2;
			u32UvHeight = pVBuf->u32Height/2;
		}
		else if(PIXEL_FORMAT_YUV_SEMIPLANAR_422 == enPixelFormat)
		{
			DBG_INFO("422\n");
			Size = (pVBuf->u32Stride[0])*(pVBuf->u32Height)*2;
			u32UvHeight = pVBuf->u32Height;
		}
		else
		{
			Size = (pVBuf->u32Stride[0])*(pVBuf->u32Height);
			u32UvHeight = 0;
		}


		phy_addr = pVBuf->u32PhyAddr[0];
		pUserPageAddr[0] = (HI_CHAR *) __COMM_SYS_Mmap(phy_addr, Size);
		if (NULL == pUserPageAddr[0])
		{
			printf(">>>>> APP : failed getting page address.\n");
			return -1;
		}

		/* umap */
		__COMM_SYS_Munmap(pUserPageAddr[0], Size);
	}
	__MEM_DEV_CLOSE();

	return Size;
}


int VectorScope_VI_Capture( VI_CHN Chn, char *pY, HI_U32 *BuffSize, HI_U32 *UV_H, VIDEO_FRAME_S *pVBuf_Info )
{
	unsigned int ret = 0;

	char * pVBufVirt_Y;
	char * pVBufVirt_C;
	char * pMemContent;

	VIDEO_FRAME_INFO_S astFrame;

	memset( &astFrame, 0x00, sizeof(VIDEO_FRAME_INFO_S) );

	VI_CHN ViChn = 0;
	HI_S32 s32MilliSec = 2000;

	// Default Chn 0
	ViChn = Chn * 4;
	/* set frame depth */
	if (HI_MPI_VI_SetFrameDepth(ViChn, 1))
	{
//		DBG_ERR("HI_MPI_VI_SetFrameDepth\n");
		return -1;
	}

	/* get raw data of VI */
	ret = HI_MPI_VI_GetFrame(ViChn, &astFrame, s32MilliSec);
	if(ret)
	{
		DBG_ERR("HI_MPI_VI_GetFrame[%08x]\n", ret);
		return -1;
	}

	/* release frame after using */
	ret = HI_MPI_VI_ReleaseFrame(ViChn, &astFrame);
	if(ret)
	{
		DBG_ERR("HI_MPI_VI_ReleaseFrame[%08x]\n", ret);
		return -1;
	}

	MEM_DEV_OPEN___();

	VIDEO_FRAME_S * pVBuf = &astFrame.stVFrame;
	{
		int ret = 0;
		int Y = 0;
		int Cb = 0;
		int Cr = 0;

		char cmd[128] = {0, };

		if(1)
		{
			unsigned int w = 0, h = 0;

			unsigned char TmpBuff[MAX_FRM_WIDTH];
			HI_U32 phy_addr,size = 0;
			HI_CHAR *pUserPageAddr[2];
			PIXEL_FORMAT_E  enPixelFormat = pVBuf->enPixelFormat;
			HI_U32 u32UvHeight = 0;

			if (pVBuf->u32Width > MAX_FRM_WIDTH)
			{
				printf(">>>>> APP : Over max frame width: %d, can't support.\n", MAX_FRM_WIDTH);
				return -1;
			}

			if (PIXEL_FORMAT_YUV_SEMIPLANAR_420 == enPixelFormat)
			{
//				DBG_INFO("420\n");
				size = (pVBuf->u32Stride[0])*(pVBuf->u32Height)*3/2;
				u32UvHeight = pVBuf->u32Height/2;
			}
			else if(PIXEL_FORMAT_YUV_SEMIPLANAR_422 == enPixelFormat)
			{
				DBG_INFO("422\n");
				size = (pVBuf->u32Stride[0])*(pVBuf->u32Height)*2;
				u32UvHeight = pVBuf->u32Height;
			}
			else
			{
				size = (pVBuf->u32Stride[0])*(pVBuf->u32Height);
				u32UvHeight = 0;
			}

			// Information Copy
			pVBuf_Info->u32Width  	 = pVBuf->u32Width;
			pVBuf_Info->u32Height 	 = pVBuf->u32Height;
			pVBuf_Info->u32Stride[0] = pVBuf->u32Stride[0];
			pVBuf_Info->u32Stride[1] = pVBuf->u32Stride[1];
			pVBuf_Info->u32Field     = pVBuf->u32Field;
			*UV_H     = u32UvHeight;
			*BuffSize = size;

			phy_addr = pVBuf->u32PhyAddr[0];
			pUserPageAddr[0] = (HI_CHAR *) __COMM_SYS_Mmap(phy_addr, size);
			if (NULL == pUserPageAddr[0])
			{
				printf(">>>>> APP : failed getting page address.\n");
				return -1;
			}

			// Up To Data
			memcpy(pY, pUserPageAddr[0], size);

			/* umap */
			__COMM_SYS_Munmap(pUserPageAddr[0], size);
		}
	}

	__MEM_DEV_CLOSE();

	return 0;
}


int VectorScope_YUV_Data_Get( char *BuffY, char *BuffC, int x, int y, int Stride, int UvHeight, unsigned char *pY, unsigned char *pCb, unsigned char *pCr )
{
	char *pMemContent;

	int adjust_x;
	int adjust_y;

	int Y  = 0;
	int Cb = 0;
	int Cr = 0;

	char *pVBufVirt_Y;
	char *pVBufVirt_C;

	pMemContent = BuffY + y*Stride + x;
	*pY = pMemContent[0];

	adjust_x = ( x%2 == 0 ) ? x : x-1;
	adjust_y = ( y > UvHeight-2 ) ? UvHeight-1 : y;
	adjust_x = ( adjust_x > Stride-3 ) ?Stride-1 : adjust_x;

	/*
	 * get Cb value
	 */
	pMemContent = BuffC + adjust_y*Stride + adjust_x;
	pMemContent += 1;
	*pCb = pMemContent[0];

	/*
	 * get Cr value
	 */
	pMemContent = BuffC + adjust_y*Stride + adjust_x;
	*pCr = pMemContent[0];

	return 0;
}

int VectorScope_YUV420_To_RGB_Get( unsigned char Y, unsigned char Cb, unsigned char Cr, unsigned char *R, unsigned char *G, unsigned char *B )
{

#if 0
	int r = Y + (1.4065 * (Cr - 128));
	int g = Y - (0.3455 * (Cb - 128)) - (0.7169 * (Cr - 128));
	int b = Y + (1.7790 * (Cb - 128));

	if (r < 0) r = 0;
	else if (r > 255) r = 255;
	if (g < 0) g = 0;
	else if (g > 255) g = 255;
	if (b < 0) b = 0;
	else if (b > 255) b = 255;

	printf("1. R(%d) G(%d) B(%d)\n", r, g, b);
#endif
	unsigned char r = 0;
	unsigned char g = 0;
	unsigned char b = 0;

	r = 1.164*(Y - 16) + 1.596*(Cr - 128);
	g = 1.164*(Y - 16) - 0.813*(Cr - 128) - 0.391*(Cb - 128);
	b = 1.164*(Y - 16) + 2.018*(Cb - 128);

	if (r < 0) r = 0;
	else if (r > 255) r = 255;
	if (g < 0) g = 0;
	else if (g > 255) g = 255;
	if (b < 0) b = 0;
	else if (b > 255) b = 255;

	*R = r;
	*G = g;
	*B = b;

	return 0;
}













