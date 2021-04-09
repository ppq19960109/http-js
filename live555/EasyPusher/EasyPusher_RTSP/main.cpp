/*
	Copyright (c) 2013-2018 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
*本Key为3个月临时授权License，如需商业使用，请邮件至support@easydarwin.org咨询商业授权方案。
*/

#define EasyRTSPClient_KEY "79393674363469576B5A7541725370636F3956524576426C59584E356348567A6147567958334A3063334258444661672F704C6A5257467A65555268636E6470626C526C5957314A6331526F5A554A6C633352414D6A41784F47566863336B3D"
#include "unistd.h"
#include <signal.h>

#include "EasyPusherAPI.h"
#include "EasyRTSPClientAPI.h"

char *ConfigIP = "192.168.1.177";						   //Default EasyDarwin Address
char *ConfigPort = "8554";								   //Default EasyDarwin Port
char *ConfigName = "easypusher_rtsp.sdp";				   //Default Stream Name
char *ConfigRTSPURL = "rtsp://p:q@192.168.1.45:8554/test"; //RTSP Source URL(With username:password@)
char *ProgName;											   //Program Name

Easy_Handle fPusherHandle = 0; //libEasyPusher Handle
Easy_Handle fRTSPHandle = 0;   //libEasyRTSPClient Handle

EASY_MEDIA_INFO_T *fSourceMediaInfo = NULL;

/* EasyPusher Callback */
int __EasyPusher_Callback(int _id, EASY_PUSH_STATE_T _state, EASY_AV_Frame *_frame, void *_userptr)
{
	printf("__EasyPusher_Callback\n");
	if (_state == EASY_PUSH_STATE_CONNECTING)
		printf("Connecting...\n");
	else if (_state == EASY_PUSH_STATE_CONNECTED)
		printf("Connected\n");
	else if (_state == EASY_PUSH_STATE_CONNECT_FAILED)
		printf("Connect failed\n");
	else if (_state == EASY_PUSH_STATE_CONNECT_ABORT)
		printf("Connect abort\n");
	else if (_state == EASY_PUSH_STATE_PUSHING)
		printf("\r Pushing to rtsp://%s:%d/%s ...", ConfigIP, atoi(ConfigPort), ConfigName);
	else if (_state == EASY_PUSH_STATE_DISCONNECTED)
		printf("Disconnect.\n");

	return 0;
}

/* EasyRTSPClient Callback */
int Easy_APICALL __RTSPSourceCallBack(int _chid, void *_chPtr, int _mediatype, char *pbuf, EASY_FRAME_INFO *frameinfo)
{
	printf("__RTSPSourceCallBack\n");
	if (_mediatype == EASY_SDK_VIDEO_FRAME_FLAG)
	{
		if ((fPusherHandle == 0) && (fSourceMediaInfo != NULL) && (frameinfo->type == EASY_SDK_VIDEO_FRAME_I))
		{
			EASY_MEDIA_INFO_T mediainfo;
			memset(&mediainfo, 0x00, sizeof(EASY_MEDIA_INFO_T));
			memcpy(&mediainfo, fSourceMediaInfo, sizeof(EASY_MEDIA_INFO_T));
			mediainfo.u32SpsLength = frameinfo->reserved1 - 4;
			mediainfo.u32PpsLength = frameinfo->reserved2 - frameinfo->reserved1 - 4;
			memcpy(mediainfo.u8Sps, pbuf + 4, mediainfo.u32SpsLength);
			memcpy(mediainfo.u8Pps, pbuf + 4 + mediainfo.u32SpsLength + 4, mediainfo.u32PpsLength);

			fPusherHandle = EasyPusher_Create();
			EasyPusher_SetEventCallback(fPusherHandle, __EasyPusher_Callback, 0, NULL);
			EasyPusher_StartStream(fPusherHandle, ConfigIP, atoi(ConfigPort), ConfigName, EASY_RTP_OVER_TCP, "admin", "admin", &mediainfo, 1024, false); //1M缓冲区
			printf("*** live streaming url:rtsp://%s:%d/%s ***\n", ConfigIP, atoi(ConfigPort), ConfigName);
		}

		if (frameinfo && frameinfo->length)
		{
			EASY_AV_Frame avFrame;
			memset(&avFrame, 0x00, sizeof(EASY_AV_Frame));
			avFrame.u32AVFrameLen = frameinfo->length;
			avFrame.pBuffer = (unsigned char *)pbuf;
			avFrame.u32VFrameType = frameinfo->type;
			avFrame.u32AVFrameFlag = EASY_SDK_VIDEO_FRAME_FLAG;
			avFrame.u32TimestampSec = frameinfo->timestamp_sec;
			avFrame.u32TimestampUsec = frameinfo->timestamp_usec;
			EasyPusher_PushFrame(fPusherHandle, &avFrame);
		}
	}

	if (_mediatype == EASY_SDK_AUDIO_FRAME_FLAG)
	{
		if (fPusherHandle == 0)
			return 0;

		if (frameinfo && frameinfo->length)
		{
			EASY_AV_Frame avFrame;
			memset(&avFrame, 0x00, sizeof(EASY_AV_Frame));
			avFrame.u32AVFrameLen = frameinfo->length;
			avFrame.pBuffer = (unsigned char *)pbuf;
			avFrame.u32VFrameType = frameinfo->type;
			avFrame.u32AVFrameFlag = EASY_SDK_AUDIO_FRAME_FLAG;
			avFrame.u32TimestampSec = frameinfo->timestamp_sec;
			avFrame.u32TimestampUsec = frameinfo->timestamp_usec;
			EasyPusher_PushFrame(fPusherHandle, &avFrame);
		}
	}

	if (_mediatype == EASY_SDK_MEDIA_INFO_FLAG)
	{
		if ((pbuf != NULL) && (fSourceMediaInfo == NULL))
		{
			fSourceMediaInfo = new EASY_MEDIA_INFO_T();
			memcpy(fSourceMediaInfo, pbuf, sizeof(EASY_MEDIA_INFO_T));
		}
	}
	return 0;
}
void PrintUsage()
{
	printf("Usage:\n");
	printf("------------------------------------------------------\n");
	printf("%s [-d <host> -p <port> -n <streamName> -u <RTSPURL>]\n", ProgName);
	printf("Help Mode:   %s -h \n", ProgName);
	printf("For example: %s -d 115.29.139.20 -p 554 -n easypusher_rtsp.sdp -u rtsp://admin:admin@anfengde.f3322.org/22\n", ProgName);
	printf("------------------------------------------------------\n");
}
int main(int argc, char *argv[])
{
	int isActivated = 0;

	signal(SIGPIPE, SIG_IGN);

	int ch;
	ProgName = argv[0];
	PrintUsage();

	if( EasyRTSP_Activate(EasyRTSPClient_KEY)<0)
	{
		printf("EasyRTSP_Activate error...\n");
		getchar();
		return -1;
	}

	//Create RTSPClient Handle
	EasyRTSP_Init(&fRTSPHandle);

	if (NULL == fRTSPHandle)
	{
		printf("EasyRTSP_Init exit...\n");
		return 0;
	}

	unsigned int mediaType = EASY_SDK_VIDEO_FRAME_FLAG | EASY_SDK_AUDIO_FRAME_FLAG; //Get Video & Audio

	EasyRTSP_SetCallback(fRTSPHandle, __RTSPSourceCallBack);
	EasyRTSP_OpenStream(fRTSPHandle, 0, ConfigRTSPURL, EASY_RTP_OVER_UDP, mediaType, 0, 0, NULL, 1000, 0, 0x01, 1);

	printf("Press Enter exit...\n");
	getchar();
	getchar();
	getchar();

	if (fPusherHandle)
	{
		EasyPusher_StopStream(fPusherHandle);
		EasyPusher_Release(fPusherHandle);
		fPusherHandle = NULL;
	}

	EasyRTSP_CloseStream(fRTSPHandle);
	EasyRTSP_Deinit(&fRTSPHandle);
	fRTSPHandle = NULL;

	return 0;
}

int main()
{
	EASY_MEDIA_INFO_T mediainfo;
	memset(&mediainfo, 0x00, sizeof(EASY_MEDIA_INFO_T));
	memcpy(&mediainfo, fSourceMediaInfo, sizeof(EASY_MEDIA_INFO_T));
	mediainfo.u32SpsLength = frameinfo->reserved1 - 4;
	mediainfo.u32PpsLength = frameinfo->reserved2 - frameinfo->reserved1 - 4;
	memcpy(mediainfo.u8Sps, pbuf + 4, mediainfo.u32SpsLength);
	memcpy(mediainfo.u8Pps, pbuf + 4 + mediainfo.u32SpsLength + 4, mediainfo.u32PpsLength);

	fPusherHandle = EasyPusher_Create();
	EasyPusher_SetEventCallback(fPusherHandle, __EasyPusher_Callback, 0, NULL);
	EasyPusher_StartStream(fPusherHandle, ConfigIP, atoi(ConfigPort), ConfigName, EASY_RTP_OVER_TCP, "admin", "admin", &mediainfo, 1024, false); //1M缓冲区
	printf("*** live streaming url:rtsp://%s:%d/%s ***\n", ConfigIP, atoi(ConfigPort), ConfigName);
}