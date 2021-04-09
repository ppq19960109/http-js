/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
#ifndef _Easy_RTSPClient_API_H
#define _Easy_RTSPClient_API_H

#include "EasyTypes.h"

#define	RTSP_PROG_NAME	"EasyRTSPClient v1.16.1028"

/*
	_channelId:		通道号,暂时不用
	_channelPtr:	通道对应对象,暂时不用
	_frameType:		EASY_SDK_VIDEO_FRAME_FLAG/EASY_SDK_AUDIO_FRAME_FLAG/EASY_SDK_EVENT_FRAME_FLAG/...	
	_pBuf:			回调的数据部分，具体用法看Demo
	_frameInfo:		帧结构数据
*/
typedef int (Easy_APICALL *RTSPSourceCallBack)( int _channelId, void *_channelPtr, int _frameType, char *pBuf, EASY_FRAME_INFO* _frameInfo);

#ifdef __cplusplus
extern "C"
{
#endif
	/* 获取最后一次错误的错误码 */
	Easy_API int Easy_APICALL EasyRTSP_GetErrCode(Easy_Handle handle);

	/* 激活 */
	Easy_API int Easy_APICALL EasyRTSP_Activate(char *license);

	/* 创建RTSPClient句柄  返回0表示成功，返回非0表示失败 */
	Easy_API int Easy_APICALL EasyRTSP_Init(Easy_Handle *handle);

	/* 释放RTSPClient 参数为RTSPClient句柄 */
	Easy_API int Easy_APICALL EasyRTSP_Deinit(Easy_Handle *handle);

	/* 设置数据回调 */
	Easy_API int Easy_APICALL EasyRTSP_SetCallback(Easy_Handle handle, RTSPSourceCallBack _callback);

	/* 打开网络流 */
	Easy_API int Easy_APICALL EasyRTSP_OpenStream(Easy_Handle handle, int _channelid, char *_url, EASY_RTP_CONNECT_TYPE _connType, unsigned int _mediaType, char *_username, char *_password, void *userPtr, int _reconn/*1000表示长连接,即如果网络断开自动重连, 其它值为连接次数*/, int outRtpPacket/*默认为0,即回调输出完整的帧, 如果为1,则输出RTP包*/, int heartbeatType/*0x00:不发送心跳 0x01:OPTIONS 0x02:GET_PARAMETER*/, int _verbosity/*日志打印输出等级，0表示不输出*/);
	
	/* 关闭网络流 */
	Easy_API int Easy_APICALL EasyRTSP_CloseStream(Easy_Handle handle);

#ifdef __cplusplus
}
#endif

#endif
