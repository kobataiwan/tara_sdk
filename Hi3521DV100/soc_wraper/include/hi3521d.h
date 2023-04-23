/* Private Data Structure */
struct hi3521d_priv {
        STR_CTRL_S StreamCtrl[MAX_VENC_CHN];
        void *rq[MAX_CAM_CHN];

        VIDEO_NORM_E gs_enNorm;         // = VIDEO_ENCODING_MODE_NTSC;
        VpssGrpInfo vs_hw[1];
        TaraVideoSource vs[MAX_CAM_CHN];
        ThreadInfo_s raw, viframe;
        int raw_stream;                 // = 0;
        int rawfileformat;
        int getstrflag;                 // = 0
        int getmjpflag;                 // = 0
        int getv2flag;                  // = 0;
        int getStrCnt;                  // = 0;
        int dbgflag;                    // =0;

        pthread_t gs_VencPid, gs_MjpgPid, gs_Video2Pid; //gs_AencPid;
        AVSERVER_VENC_GETSTREAM_PARA_S gs_stParam, gs_mjParam, gs_v2Param;
        GetStreamThreadCtrl getStrThrCtrl[MAX_CAM_CHN];
        TaraVideoEncoder video1[MAX_CAM_CHN], video2[MAX_CAM_CHN];
        TaraMjpegEncoder mjpeg1[MAX_CAM_CHN];
};

