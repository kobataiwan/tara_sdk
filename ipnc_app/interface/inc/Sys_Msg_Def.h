/* ===========================================================================
* @path $(IPNCPATH)\include
*
* @desc
* .
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */
/**
* @file Sys_Msg_Def.h
* @brief System message ID definition
* @warning Be sure that one message ID can only be used by one process.
* @see File_Msg_Def.h
*/
#ifndef _SYS_MSG_DEF_H_
#define _SYS_MSG_DEF_H_
/**
* @addtogroup SYS_MSG_DRV
* @{
*/
/* MSG_TYPE_MSG1 is reserved for server */
#define SYS_BOA_MSG		MSG_TYPE_MSG2 ///< message ID used in BOA process to communicate with system server. 
#define SYS_UPNP_MSG		MSG_TYPE_MSG3 ///< message ID used in UPNP process to communicate with system server. 
#define SYS_FCGI_STR_MSG		MSG_TYPE_MSG4
#define SYS_FCGI_NET_MSG	MSG_TYPE_MSG5
#define SYS_FCGI_RST_MSG	MSG_TYPE_MSG6
#define SYS_BOOT_MSG		MSG_TYPE_MSG7 ///< message ID used in boot_proc to communicate with system server.
#define SYS_FCGI_VIDEO_MSG	MSG_TYPE_MSG8
/**
* @}
*/
#endif
