/******************************************************************************
  A simple program of Hisilicon Hi35xx video encode implementation.
  Copyright (C), 2010-2011, Hisilicon Tech. Co., Ltd.
 ******************************************************************************
    Modification:  2011-2 Created
******************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "sample_comm.h"

VIDEO_NORM_E gs_enNorm = VIDEO_ENCODING_MODE_NTSC;

/******************************************************************************
* function : show usage
******************************************************************************/
void SAMPLE_VENC_Usage(char* sPrgNm)
{
#ifndef hi3516ev100
    printf("Usage : %s <index>  eg: %s 0 \n", sPrgNm, sPrgNm);
    printf("index:\n");
    printf("\t 0) NormalP:H.265@1080p@30fps+H.264@1080p@30fps\n");
    printf("\t 1) SmartP: H.265@1080p@30fps+H.264@1080p@30fps\n");
    printf("\t 2) DualP:  H.265@1080p@30fps@BigSmallP+H.264@1080p@30fps@BigSmallP\n");
	printf("\t 3) DualP2: H.265@1080p@30fps@SmallP+H.264@1080p@30fps@BigSmallP\n");
	printf("\t 4) IntraRefresh: H.265@1080p@30fps@IntraRefresh+H.264@1080p@30fps@IntraRefresh\n");
    printf("\t 5) Jpege:1*1080p mjpeg encode + 1*1080p jpeg\n");
	printf("\t 6) OnlineLowDelay:H.264@1080p@30fps@OnlineLowDelay + H.265@1080p@30fps\n");
	printf("\t 7) RoiBgFrameRate:H.264@1080p@30fps@RoiBgFrameRate + H.265@1080p@30fps@RoiBgFrameRate\n");
	printf("\t 8) svc-t :H.264@1080p@30fps@svc-t\n");
	printf("\t 9) EPTZ:H.264@1080p@30fps + H.265@1080p@30fps@EPTZ\n");
#else
    printf("Usage : %s <index>  eg: %s 0 \n", sPrgNm, sPrgNm);
    printf("index:\n");
    printf("\t 0) NormalP:H.265@1080p@30fps\n");
    printf("\t 1) SmartP: H.265@1080p@30fps\n");
    printf("\t 2) DualP:  H.265@1080p@30fps@BigSmallP\n");
	printf("\t 3) DualP2: H.265@1080p@30fps@SmallP\n");
	printf("\t 4) IntraRefresh: H.265@1080p@30fps@IntraRefresh\n");
    printf("\t 5) Jpege:1*1080p mjpeg encode + 1*1080p jpeg\n");
	printf("\t 6) OnlineLowDelay:H.264@1080p@30fps@OnlineLowDelay\n");
	printf("\t 7) RoiBgFrameRate:H.264@1080p@30fps@RoiBgFrameRate\n");
	printf("\t 8) svc-t :H.264@1080p@30fps@svc-t\n");
#endif
    return;
}

/******************************************************************************
* function : to process abnormal case
******************************************************************************/
#ifndef __HuaweiLite__
void SAMPLE_VENC_HandleSig(HI_S32 signo)
{
    signal(SIGINT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
    if (SIGINT == signo || SIGTERM == signo)
    {
        SAMPLE_COMM_VENC_StopGetStream();
		SAMPLE_COMM_VPSS_StopChnCropProc();
        SAMPLE_COMM_ISP_Stop();
        SAMPLE_COMM_SYS_Exit();
        printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
    }
    exit(-1);
}
#endif

/******************************************************************************
* function : to process abnormal case - the case of stream venc
******************************************************************************/
void SAMPLE_VENC_StreamHandleSig(HI_S32 signo)
{

    if (SIGINT == signo || SIGTERM == signo)
    {
        SAMPLE_COMM_SYS_Exit();
        printf("\033[0;31mprogram exit abnormally!\033[0;39m\n");
    }

    exit(0);
}

/******************************************************************************
* function :  H.264@1080p@30fps+H.264@VGA@30fps


******************************************************************************/
HI_S32 SAMPLE_VENC_NORMALP_CLASSIC(HI_VOID)
{
    PAYLOAD_TYPE_E enPayLoad[2]= {PT_H265, PT_H264};
    PIC_SIZE_E enSize[2] = {PIC_HD1080, PIC_HD1080};
	HI_U32 u32Profile = 0;

    VB_CONF_S stVbConf;
    SAMPLE_VI_CONFIG_S stViConfig = {0};

    VPSS_GRP VpssGrp;
    VPSS_CHN VpssChn;
    VPSS_GRP_ATTR_S stVpssGrpAttr = {0};
    VPSS_CHN_ATTR_S stVpssChnAttr = {0};
    VPSS_CHN_MODE_S stVpssChnMode;

    VENC_CHN VencChn;
    SAMPLE_RC_E enRcMode= SAMPLE_RC_CBR;

#ifndef hi3516ev100
    HI_S32 s32ChnNum=2;
#else
    HI_S32 s32ChnNum=1;
#endif

    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32BlkSize;
    SIZE_S stSize;
    char c;


    /******************************************
     step  1: init sys variable
    ******************************************/
    memset(&stVbConf,0,sizeof(VB_CONF_S));

    SAMPLE_COMM_VI_GetSizeBySensor(&enSize[0]);

    stVbConf.u32MaxPoolCnt = 128;
	//printf("s32ChnNum:%d,Sensor Size:%d\n",s32ChnNum,enSize[0]);
    /*video buffer*/
	if(s32ChnNum >= 1)
    {
	    u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(gs_enNorm,\
	                enSize[0], SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH);
	    stVbConf.astCommPool[0].u32BlkSize = u32BlkSize;
        #ifndef hi3516ev100
	    stVbConf.astCommPool[0].u32BlkCnt = 10;
        #else
        stVbConf.astCommPool[0].u32BlkCnt = 5;
        #endif
	}
	if(s32ChnNum >= 2)
    {
	    u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(gs_enNorm,\
	                enSize[1], SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH);
	    stVbConf.astCommPool[1].u32BlkSize = u32BlkSize;
	    stVbConf.astCommPool[1].u32BlkCnt =10;
	}
	if(s32ChnNum >= 3)
    {
	    u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(gs_enNorm,\
	                enSize[2], SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH);
	    stVbConf.astCommPool[2].u32BlkSize = u32BlkSize;
	    stVbConf.astCommPool[2].u32BlkCnt = 10;
	}

    /******************************************
     step 2: mpp system init.
    ******************************************/
    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("system init failed with %d!\n", s32Ret);
        goto END_VENC_1080P_CLASSIC_0;
    }

    /******************************************
     step 3: start vi dev & chn to capture
    ******************************************/
    stViConfig.enViMode   = SENSOR_TYPE;
    stViConfig.enRotate   = ROTATE_NONE;
    stViConfig.enNorm     = VIDEO_ENCODING_MODE_AUTO;
    stViConfig.enViChnSet = VI_CHN_SET_NORMAL;
    stViConfig.enWDRMode  = WDR_MODE_NONE;
    s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start vi failed!\n");
        goto END_VENC_1080P_CLASSIC_1;
    }

    /******************************************
     step 4: start vpss and vi bind vpss
    ******************************************/
    s32Ret = SAMPLE_COMM_SYS_GetPicSize(gs_enNorm, enSize[0], &stSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
        goto END_VENC_1080P_CLASSIC_1;
    }

    if(s32ChnNum >= 1)
    {
	    VpssGrp = 0;
	    stVpssGrpAttr.u32MaxW = stSize.u32Width;
	    stVpssGrpAttr.u32MaxH = stSize.u32Height;
	    stVpssGrpAttr.bIeEn = HI_FALSE;
	    stVpssGrpAttr.bNrEn = HI_TRUE;
	    stVpssGrpAttr.bHistEn = HI_FALSE;
	    stVpssGrpAttr.bDciEn = HI_FALSE;
	    stVpssGrpAttr.enDieMode = VPSS_DIE_MODE_NODIE;
	    stVpssGrpAttr.enPixFmt = SAMPLE_PIXEL_FORMAT;
	    stVpssGrpAttr.bSharpenEn = HI_FALSE;

	    s32Ret = SAMPLE_COMM_VPSS_StartGroup(VpssGrp, &stVpssGrpAttr);
	    if (HI_SUCCESS != s32Ret)
	    {
		    SAMPLE_PRT("Start Vpss failed!\n");
		    goto END_VENC_1080P_CLASSIC_2;
	    }

	    s32Ret = SAMPLE_COMM_VI_BindVpss(stViConfig.enViMode);
	    if (HI_SUCCESS != s32Ret)
	    {
		    SAMPLE_PRT("Vi bind Vpss failed!\n");
		    goto END_VENC_1080P_CLASSIC_3;
	    }

	    VpssChn = 0;
	    stVpssChnMode.enChnMode      = VPSS_CHN_MODE_USER;
	    stVpssChnMode.bDouble        = HI_FALSE;
	    stVpssChnMode.enPixelFormat  = SAMPLE_PIXEL_FORMAT;
	    stVpssChnMode.u32Width       = stSize.u32Width;
	    stVpssChnMode.u32Height      = stSize.u32Height;
	    stVpssChnMode.enCompressMode = COMPRESS_MODE_SEG;
	    memset(&stVpssChnAttr, 0, sizeof(stVpssChnAttr));
	    stVpssChnAttr.s32SrcFrameRate = -1;
	    stVpssChnAttr.s32DstFrameRate = -1;
	    s32Ret = SAMPLE_COMM_VPSS_EnableChn(VpssGrp, VpssChn, &stVpssChnAttr, &stVpssChnMode, HI_NULL);
	    if (HI_SUCCESS != s32Ret)
	    {
		    SAMPLE_PRT("Enable vpss chn failed!\n");
		    goto END_VENC_1080P_CLASSIC_4;
	    }
    }
    if(s32ChnNum >= 2)
    {
		s32Ret = SAMPLE_COMM_SYS_GetPicSize(gs_enNorm, enSize[1], &stSize);
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT(	"SAMPLE_COMM_SYS_GetPicSize failed!\n");
			goto END_VENC_1080P_CLASSIC_4;
		}
	    VpssChn = 1;
	    stVpssChnMode.enChnMode       = VPSS_CHN_MODE_USER;
	    stVpssChnMode.bDouble         = HI_FALSE;
	    stVpssChnMode.enPixelFormat   = SAMPLE_PIXEL_FORMAT;
	    stVpssChnMode.u32Width        = stSize.u32Width;
	    stVpssChnMode.u32Height       = stSize.u32Height;
	    stVpssChnMode.enCompressMode  = COMPRESS_MODE_SEG;
	    stVpssChnAttr.s32SrcFrameRate = -1;
	    stVpssChnAttr.s32DstFrameRate = -1;
	    s32Ret = SAMPLE_COMM_VPSS_EnableChn(VpssGrp, VpssChn, &stVpssChnAttr, &stVpssChnMode, HI_NULL);
	    if (HI_SUCCESS != s32Ret)
	    {
		    SAMPLE_PRT("Enable vpss chn failed!\n");
		    goto END_VENC_1080P_CLASSIC_4;
	    }
    }
    if(s32ChnNum >= 3)
    {
		s32Ret = SAMPLE_COMM_SYS_GetPicSize(gs_enNorm, enSize[2], &stSize);
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT(	"SAMPLE_COMM_SYS_GetPicSize failed!\n");
			goto END_VENC_1080P_CLASSIC_4;
		}

	    VpssChn = 2;
	    stVpssChnMode.enChnMode 	= VPSS_CHN_MODE_USER;
	    stVpssChnMode.bDouble		= HI_FALSE;
	    stVpssChnMode.enPixelFormat = SAMPLE_PIXEL_FORMAT;
	    stVpssChnMode.u32Width		= 720;
	    stVpssChnMode.u32Height 	= (VIDEO_ENCODING_MODE_PAL==gs_enNorm)?576:480;;
	    stVpssChnMode.enCompressMode = COMPRESS_MODE_NONE;

	    stVpssChnAttr.s32SrcFrameRate = -1;
	    stVpssChnAttr.s32DstFrameRate = -1;

	    s32Ret = SAMPLE_COMM_VPSS_EnableChn(VpssGrp, VpssChn, &stVpssChnAttr, &stVpssChnMode, HI_NULL);
	    if (HI_SUCCESS != s32Ret)
	    {
		    SAMPLE_PRT("Enable vpss chn failed!\n");
		    goto END_VENC_1080P_CLASSIC_4;
	    }
    }
    /******************************************
     step 5: start stream venc
    ******************************************/
    /*** HD1080P **/
    printf("\t c) cbr.\n");
    printf("\t v) vbr.\n");
	printf("\t a) avbr.\n");
    printf("\t f) fixQp\n");
    printf("please input choose rc mode!\n");
    c = (char)getchar();
    switch(c)
    {
        case 'c':
            enRcMode = SAMPLE_RC_CBR;
            break;
        case 'v':
            enRcMode = SAMPLE_RC_VBR;
            break;
		case 'a':
			enRcMode = SAMPLE_RC_AVBR;
			break;
        case 'f':
            enRcMode = SAMPLE_RC_FIXQP;
            break;
        default:
            printf("rc mode! is invaild!\n");
            goto END_VENC_1080P_CLASSIC_4;
    }

	/*** enSize[0] **/
	if(s32ChnNum >= 1)
	{
		VpssGrp = 0;
	    VpssChn = 0;
	    VencChn = 0;

	    s32Ret = SAMPLE_COMM_VENC_Start(VencChn, enPayLoad[0],\
	                                   gs_enNorm, enSize[0], enRcMode,u32Profile,ROTATE_NONE);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Venc failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }

	    s32Ret = SAMPLE_COMM_VENC_BindVpss(VencChn, VpssGrp, VpssChn);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Venc failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }
	}

	/*** enSize[1] **/
	if(s32ChnNum >= 2)
	{
		VpssChn = 1;
	    VencChn = 1;
	    s32Ret = SAMPLE_COMM_VENC_Start(VencChn, enPayLoad[1], \
	                                   gs_enNorm, enSize[1], enRcMode,u32Profile,ROTATE_NONE);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Venc failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }

	    s32Ret = SAMPLE_COMM_VENC_BindVpss(VencChn, VpssGrp, VpssChn);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Venc failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }
	}
	/*** enSize[2] **/
	if(s32ChnNum >= 3)
	{
	    VpssChn = 2;
	    VencChn = 2;

	    s32Ret = SAMPLE_COMM_VENC_Start(VencChn, enPayLoad[2],\
	                                   gs_enNorm, enSize[2], enRcMode,u32Profile,ROTATE_NONE);

	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Venc failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }

	    s32Ret = SAMPLE_COMM_VENC_BindVpss(VencChn, VpssGrp, VpssChn);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Venc failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }
	}
    /******************************************
     step 6: stream venc process -- get stream, then save it to file.
    ******************************************/
    s32Ret = SAMPLE_COMM_VENC_StartGetStream(s32ChnNum);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Venc failed!\n");
        goto END_VENC_1080P_CLASSIC_5;
    }

    printf("please press twice ENTER to exit this sample\n");
    getchar();
    getchar();

    /******************************************
     step 7: exit process
    ******************************************/
    SAMPLE_COMM_VENC_StopGetStream();

END_VENC_1080P_CLASSIC_5:

    VpssGrp = 0;
	switch(s32ChnNum)
	{
		case 3:
			VpssChn = 2;
		    VencChn = 2;
		    SAMPLE_COMM_VENC_UnBindVpss(VencChn, VpssGrp, VpssChn);
		    SAMPLE_COMM_VENC_Stop(VencChn);
		case 2:
			VpssChn = 1;
		    VencChn = 1;
		    SAMPLE_COMM_VENC_UnBindVpss(VencChn, VpssGrp, VpssChn);
		    SAMPLE_COMM_VENC_Stop(VencChn);
		case 1:
			VpssChn = 0;
		    VencChn = 0;
		    SAMPLE_COMM_VENC_UnBindVpss(VencChn, VpssGrp, VpssChn);
		    SAMPLE_COMM_VENC_Stop(VencChn);
			break;

	}
    SAMPLE_COMM_VI_UnBindVpss(stViConfig.enViMode);

END_VENC_1080P_CLASSIC_4:	//vpss stop

    VpssGrp = 0;
	switch(s32ChnNum)
	{
		case 3:
			VpssChn = 2;
			SAMPLE_COMM_VPSS_DisableChn(VpssGrp, VpssChn);
		case 2:
			VpssChn = 1;
			SAMPLE_COMM_VPSS_DisableChn(VpssGrp, VpssChn);
		case 1:
			VpssChn = 0;
			SAMPLE_COMM_VPSS_DisableChn(VpssGrp, VpssChn);
		break;

	}

END_VENC_1080P_CLASSIC_3:    //vpss stop
    SAMPLE_COMM_VI_UnBindVpss(stViConfig.enViMode);
END_VENC_1080P_CLASSIC_2:    //vpss stop
    SAMPLE_COMM_VPSS_StopGroup(VpssGrp);
END_VENC_1080P_CLASSIC_1:	//vi stop
    SAMPLE_COMM_VI_StopVi(&stViConfig);
END_VENC_1080P_CLASSIC_0:	//system exit
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}



/******************************************************************************
* function :  Gop_Mode_SmartP for H.265@1080P@normalP +H.265@1080P@smartP


******************************************************************************/
HI_S32 SAMPLE_VENC_SMARTP_CLASSIC(HI_VOID)
{
    PAYLOAD_TYPE_E enPayLoad[2]= {PT_H265, PT_H264};
    PIC_SIZE_E enSize[2] = {PIC_HD1080, PIC_HD1080};
	VENC_GOP_ATTR_S stGopAttr;
	VENC_GOP_MODE_E enGopMode[2] = {VENC_GOPMODE_SMARTP,VENC_GOPMODE_SMARTP};
	HI_U32 u32Profile = 0;

	VB_CONF_S stVbConf;
	SAMPLE_VI_CONFIG_S stViConfig = {0};

	VPSS_GRP VpssGrp;
	VPSS_CHN VpssChn;
    VPSS_GRP_ATTR_S stVpssGrpAttr = {0};
    VPSS_CHN_ATTR_S stVpssChnAttr = {0};
	VPSS_CHN_MODE_S stVpssChnMode;

	VENC_CHN VencChn;
	SAMPLE_RC_E enRcMode= SAMPLE_RC_CBR;

#ifndef hi3516ev100
    HI_S32 s32ChnNum=2;
#else
    HI_S32 s32ChnNum=1;
#endif

	HI_S32 s32Ret = HI_SUCCESS;
	HI_U32 u32BlkSize;
	SIZE_S stSize;
	char c;


	/******************************************
	 step  1: init sys variable
	******************************************/
	memset(&stVbConf,0,sizeof(VB_CONF_S));

	SAMPLE_COMM_VI_GetSizeBySensor(&enSize[0]);

	stVbConf.u32MaxPoolCnt = 128;
//	printf("s32ChnNum:%d,Sensor Size:%d\n",s32ChnNum,enSize[0]);
    /*video buffer*/
	if(s32ChnNum >= 1)
	{
		u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(gs_enNorm,\
					enSize[0], SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH);
		stVbConf.astCommPool[0].u32BlkSize = u32BlkSize;
    #ifndef hi3516ev100
        stVbConf.astCommPool[0].u32BlkCnt = 10;
    #else
        stVbConf.astCommPool[0].u32BlkCnt = 4;
    #endif
	}
	if(s32ChnNum >= 2)
	{
		u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(gs_enNorm,\
					enSize[1], SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH);
		stVbConf.astCommPool[1].u32BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt =10;
	}
	if(s32ChnNum >= 3)
	{
		u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(gs_enNorm,\
					enSize[2], SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH);
		stVbConf.astCommPool[2].u32BlkSize = u32BlkSize;
		stVbConf.astCommPool[2].u32BlkCnt = 10;
	}

	/******************************************
	 step 2: mpp system init.
	******************************************/
	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
	if (HI_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("system init failed with %d!\n", s32Ret);
		goto END_VENC_1080P_CLASSIC_0;
	}

	/******************************************
	 step 3: start vi dev & chn to capture
	******************************************/
	stViConfig.enViMode   = SENSOR_TYPE;
	stViConfig.enRotate   = ROTATE_NONE;
	stViConfig.enNorm	  = VIDEO_ENCODING_MODE_AUTO;
	stViConfig.enViChnSet = VI_CHN_SET_NORMAL;
	stViConfig.enWDRMode  = WDR_MODE_NONE;
	s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
	if (HI_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("start vi failed!\n");
		goto END_VENC_1080P_CLASSIC_1;
	}

	/******************************************
	 step 4: start vpss and vi bind vpss
	******************************************/
	s32Ret = SAMPLE_COMM_SYS_GetPicSize(gs_enNorm, enSize[0], &stSize);
	if (HI_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
		goto END_VENC_1080P_CLASSIC_1;
	}

	if(s32ChnNum >= 1)
	{
		VpssGrp = 0;
		stVpssGrpAttr.u32MaxW = stSize.u32Width;
		stVpssGrpAttr.u32MaxH = stSize.u32Height;
		stVpssGrpAttr.bIeEn = HI_FALSE;
		stVpssGrpAttr.bNrEn = HI_TRUE;
		stVpssGrpAttr.bHistEn = HI_FALSE;
		stVpssGrpAttr.bDciEn = HI_FALSE;
		stVpssGrpAttr.enDieMode = VPSS_DIE_MODE_NODIE;
		stVpssGrpAttr.enPixFmt = SAMPLE_PIXEL_FORMAT;
        stVpssGrpAttr.bSharpenEn = HI_FALSE;

		s32Ret = SAMPLE_COMM_VPSS_StartGroup(VpssGrp, &stVpssGrpAttr);
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("Start Vpss failed!\n");
			goto END_VENC_1080P_CLASSIC_2;
		}

		s32Ret = SAMPLE_COMM_VI_BindVpss(stViConfig.enViMode);
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("Vi bind Vpss failed!\n");
			goto END_VENC_1080P_CLASSIC_3;
		}

		VpssChn = 0;
		stVpssChnMode.enChnMode 	 = VPSS_CHN_MODE_USER;
		stVpssChnMode.bDouble		 = HI_FALSE;
		stVpssChnMode.enPixelFormat  = SAMPLE_PIXEL_FORMAT;
		stVpssChnMode.u32Width		 = stSize.u32Width;
		stVpssChnMode.u32Height 	 = stSize.u32Height;
		stVpssChnMode.enCompressMode = COMPRESS_MODE_SEG;
		memset(&stVpssChnAttr, 0, sizeof(stVpssChnAttr));
		stVpssChnAttr.s32SrcFrameRate = -1;
		stVpssChnAttr.s32DstFrameRate = -1;
		s32Ret = SAMPLE_COMM_VPSS_EnableChn(VpssGrp, VpssChn, &stVpssChnAttr, &stVpssChnMode, HI_NULL);
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("Enable vpss chn failed!\n");
			goto END_VENC_1080P_CLASSIC_4;
		}
	}
	if(s32ChnNum >= 2)
	{
		s32Ret = SAMPLE_COMM_SYS_GetPicSize(gs_enNorm, enSize[1], &stSize);
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT( "SAMPLE_COMM_SYS_GetPicSize failed!\n");
			goto END_VENC_1080P_CLASSIC_4;
		}
		VpssChn = 1;
		stVpssChnMode.enChnMode 	  = VPSS_CHN_MODE_USER;
		stVpssChnMode.bDouble		  = HI_FALSE;
		stVpssChnMode.enPixelFormat   = SAMPLE_PIXEL_FORMAT;
		stVpssChnMode.u32Width		  = stSize.u32Width;
		stVpssChnMode.u32Height 	  = stSize.u32Height;
		stVpssChnMode.enCompressMode  = COMPRESS_MODE_SEG;
		stVpssChnAttr.s32SrcFrameRate = -1;
		stVpssChnAttr.s32DstFrameRate = -1;
		s32Ret = SAMPLE_COMM_VPSS_EnableChn(VpssGrp, VpssChn, &stVpssChnAttr, &stVpssChnMode, HI_NULL);
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("Enable vpss chn failed!\n");
			goto END_VENC_1080P_CLASSIC_4;
		}
	}
	if(s32ChnNum >= 3)
	{
		s32Ret = SAMPLE_COMM_SYS_GetPicSize(gs_enNorm, enSize[2], &stSize);
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT( "SAMPLE_COMM_SYS_GetPicSize failed!\n");
			goto END_VENC_1080P_CLASSIC_4;
		}

		VpssChn = 2;
		stVpssChnMode.enChnMode 	= VPSS_CHN_MODE_USER;
		stVpssChnMode.bDouble		= HI_FALSE;
		stVpssChnMode.enPixelFormat = SAMPLE_PIXEL_FORMAT;
		stVpssChnMode.u32Width		= stSize.u32Width;   //720;
		stVpssChnMode.u32Height 	= stSize.u32Height;  //(VIDEO_ENCODING_MODE_PAL==gs_enNorm)?576:480;;
		stVpssChnMode.enCompressMode = COMPRESS_MODE_NONE;

		stVpssChnAttr.s32SrcFrameRate = -1;
		stVpssChnAttr.s32DstFrameRate = -1;

		s32Ret = SAMPLE_COMM_VPSS_EnableChn(VpssGrp, VpssChn, &stVpssChnAttr, &stVpssChnMode, HI_NULL);
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("Enable vpss chn failed!\n");
			goto END_VENC_1080P_CLASSIC_4;
		}
	}
	/******************************************
	 step 5: start stream venc
	******************************************/
	/*** HD1080P **/
	printf("\t c) cbr.\n");
	printf("\t v) vbr.\n");
	printf("\t a) avbr.\n");
	printf("\t f) fixQp\n");
	printf("please input choose rc mode!\n");
	c = (char)getchar();
	switch(c)
	{
		case 'c':
			enRcMode = SAMPLE_RC_CBR;
			break;
		case 'v':
			enRcMode = SAMPLE_RC_VBR;
			break;
		case 'a':
			enRcMode = SAMPLE_RC_AVBR;
			break;
		case 'f':
			enRcMode = SAMPLE_RC_FIXQP;
			break;
		default:
			printf("rc mode! is invaild!\n");
			goto END_VENC_1080P_CLASSIC_4;
	}

	/*** enSize[0] **/
	if(s32ChnNum >= 1)
	{
		VpssGrp = 0;
		VpssChn = 0;
	    VencChn = 0;

		s32Ret = SAMPLE_COMM_VENC_GetGopAttr(enGopMode[0],&stGopAttr,gs_enNorm);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Get GopAttr failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }

	    s32Ret = SAMPLE_COMM_VENC_StartEx(VencChn, enPayLoad[0],\
	                                   gs_enNorm, enSize[0], enRcMode,u32Profile,&stGopAttr);
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("Start Venc failed!\n");
			goto END_VENC_1080P_CLASSIC_5;
		}

		s32Ret = SAMPLE_COMM_VENC_BindVpss(VencChn, VpssGrp, VpssChn);
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("Start Venc failed!\n");
			goto END_VENC_1080P_CLASSIC_5;
		}
	}

	/*** enSize[1] **/
	if(s32ChnNum >= 2)
	{
		VpssChn = 1;
	    VencChn = 1;

		s32Ret = SAMPLE_COMM_VENC_GetGopAttr(enGopMode[1],&stGopAttr,gs_enNorm);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Get GopAttr failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }

	    s32Ret = SAMPLE_COMM_VENC_StartEx(VencChn, enPayLoad[1],\
	                                   gs_enNorm, enSize[1], enRcMode,u32Profile,&stGopAttr);
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("Start Venc failed!\n");
			goto END_VENC_1080P_CLASSIC_5;
		}

		s32Ret = SAMPLE_COMM_VENC_BindVpss(VencChn, VpssGrp, VpssChn);
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("Start Venc failed!\n");
			goto END_VENC_1080P_CLASSIC_5;
		}
	}
	/*** enSize[2] **/
	if(s32ChnNum >= 3)
	{
	    VpssChn = 2;
	    VencChn = 2;

		s32Ret = SAMPLE_COMM_VENC_GetGopAttr(enGopMode[2],&stGopAttr,gs_enNorm);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Get GopAttr failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }

	    s32Ret = SAMPLE_COMM_VENC_StartEx(VencChn, enPayLoad[2],\
	                                   gs_enNorm, enSize[2], enRcMode,u32Profile,&stGopAttr);
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("Start Venc failed!\n");
			goto END_VENC_1080P_CLASSIC_5;
		}

		s32Ret = SAMPLE_COMM_VENC_BindVpss(VencChn, VpssGrp, VpssChn);
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("Start Venc failed!\n");
			goto END_VENC_1080P_CLASSIC_5;
		}
	}

/******************************************
 step 6: stream venc process -- get stream, then save it to file.
  ******************************************/
    s32Ret = SAMPLE_COMM_VENC_StartGetStream(s32ChnNum);
		if (HI_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("Start Venc failed!\n");
		goto END_VENC_1080P_CLASSIC_5;
	}

	printf("please press twice ENTER to exit this sample\n");
		getchar();
	getchar();

	/******************************************
	 step 7: exit process
	******************************************/

	SAMPLE_COMM_VENC_StopGetStream();

END_VENC_1080P_CLASSIC_5:
	VpssGrp = 0;
	switch(s32ChnNum)
	{
		case 3:
			VpssChn = 2;
			VencChn = 2;
			SAMPLE_COMM_VENC_UnBindVpss(VencChn, VpssGrp, VpssChn);
			SAMPLE_COMM_VENC_Stop(VencChn);
		case 2:
			VpssChn = 1;
			VencChn = 1;
			SAMPLE_COMM_VENC_UnBindVpss(VencChn, VpssGrp, VpssChn);
			SAMPLE_COMM_VENC_Stop(VencChn);
		case 1:
			VpssChn = 0;
			VencChn = 0;
			SAMPLE_COMM_VENC_UnBindVpss(VencChn, VpssGrp, VpssChn);
			SAMPLE_COMM_VENC_Stop(VencChn);
			break;
	}
			SAMPLE_COMM_VI_UnBindVpss(stViConfig.enViMode);
END_VENC_1080P_CLASSIC_4:	//vpss stop
    VpssGrp = 0;
	switch(s32ChnNum)
	{
		case 3:
			VpssChn = 2;
			SAMPLE_COMM_VPSS_DisableChn(VpssGrp, VpssChn);
		case 2:
			VpssChn = 1;
			SAMPLE_COMM_VPSS_DisableChn(VpssGrp, VpssChn);
		case 1:
			VpssChn = 0;
			SAMPLE_COMM_VPSS_DisableChn(VpssGrp, VpssChn);
		break;

	}

END_VENC_1080P_CLASSIC_3:    //vpss stop
    SAMPLE_COMM_VI_UnBindVpss(stViConfig.enViMode);
END_VENC_1080P_CLASSIC_2:    //vpss stop
    SAMPLE_COMM_VPSS_StopGroup(VpssGrp);
END_VENC_1080P_CLASSIC_1:	//vi stop
    SAMPLE_COMM_VI_StopVi(&stViConfig);
END_VENC_1080P_CLASSIC_0:	//system exit
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}


/******************************************************************************
* function :   function DUALP :  H.265@1080@30fps+H.264@1080@30fps

******************************************************************************/
HI_S32 SAMPLE_VENC_DUALP_CLASSIC(HI_VOID)
{
    PAYLOAD_TYPE_E enPayLoad[2]= {PT_H265,PT_H264};
    PIC_SIZE_E enSize[2] = {PIC_HD1080, PIC_HD1080};
	VENC_GOP_ATTR_S stGopAttr;
	VENC_GOP_MODE_E enGopMode[2] = {VENC_GOPMODE_DUALP,VENC_GOPMODE_DUALP};
	HI_U32 u32Profile = 0;

    VB_CONF_S stVbConf;
    SAMPLE_VI_CONFIG_S stViConfig = {0};

    VPSS_GRP VpssGrp;
    VPSS_CHN VpssChn;
    VPSS_GRP_ATTR_S stVpssGrpAttr = {0};
    VPSS_CHN_ATTR_S stVpssChnAttr = {0};
    VPSS_CHN_MODE_S stVpssChnMode;

    VENC_CHN VencChn;
    SAMPLE_RC_E enRcMode= SAMPLE_RC_CBR;

#ifndef hi3516ev100
    HI_S32 s32ChnNum=2;
#else
    HI_S32 s32ChnNum=1;
#endif

    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32BlkSize;
    SIZE_S stSize;
    char c;


    /******************************************
     step  1: init sys variable
    ******************************************/
    memset(&stVbConf,0,sizeof(VB_CONF_S));

    SAMPLE_COMM_VI_GetSizeBySensor(&enSize[0]);

    stVbConf.u32MaxPoolCnt = 128;
//	printf("s32ChnNum = %d\n",s32ChnNum);
    /*video buffer*/
	if(s32ChnNum >= 1)
    {
	    u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(gs_enNorm,\
	                enSize[0], SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH);
	    stVbConf.astCommPool[0].u32BlkSize = u32BlkSize;
    #ifndef hi3516ev100
        stVbConf.astCommPool[0].u32BlkCnt = 10;
    #else
        stVbConf.astCommPool[0].u32BlkCnt = 4;
    #endif
	}
	if(s32ChnNum >= 2)
    {
	    u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(gs_enNorm,\
	                enSize[1], SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH);
	    stVbConf.astCommPool[1].u32BlkSize = u32BlkSize;
	    stVbConf.astCommPool[1].u32BlkCnt =10;
	}
	if(s32ChnNum >= 3)
    {
	    u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(gs_enNorm,\
	                enSize[2], SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH);
	    stVbConf.astCommPool[2].u32BlkSize = u32BlkSize;
	    stVbConf.astCommPool[2].u32BlkCnt = 10;
	}

    /******************************************
     step 2: mpp system init.
    ******************************************/
    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("system init failed with %d!\n", s32Ret);
        goto END_VENC_1080P_CLASSIC_0;
    }

    /******************************************
     step 3: start vi dev & chn to capture
    ******************************************/
    stViConfig.enViMode   = SENSOR_TYPE;
    stViConfig.enRotate   = ROTATE_NONE;
    stViConfig.enNorm     = VIDEO_ENCODING_MODE_AUTO;
    stViConfig.enViChnSet = VI_CHN_SET_NORMAL;
    stViConfig.enWDRMode  = WDR_MODE_NONE;
    s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start vi failed!\n");
        goto END_VENC_1080P_CLASSIC_1;
    }

    /******************************************
     step 4: start vpss and vi bind vpss
    ******************************************/
    s32Ret = SAMPLE_COMM_SYS_GetPicSize(gs_enNorm, enSize[0], &stSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
        goto END_VENC_1080P_CLASSIC_1;
    }

    if(s32ChnNum >= 1)
    {
	    VpssGrp = 0;
	    stVpssGrpAttr.u32MaxW = stSize.u32Width;
	    stVpssGrpAttr.u32MaxH = stSize.u32Height;
	    stVpssGrpAttr.bIeEn = HI_FALSE;
	    stVpssGrpAttr.bNrEn = HI_TRUE;
	    stVpssGrpAttr.bHistEn = HI_FALSE;
	    stVpssGrpAttr.bDciEn = HI_FALSE;
	    stVpssGrpAttr.enDieMode = VPSS_DIE_MODE_NODIE;
	    stVpssGrpAttr.enPixFmt = SAMPLE_PIXEL_FORMAT;
        stVpssGrpAttr.bSharpenEn = HI_FALSE;

	    s32Ret = SAMPLE_COMM_VPSS_StartGroup(VpssGrp, &stVpssGrpAttr);
	    if (HI_SUCCESS != s32Ret)
	    {
		    SAMPLE_PRT("Start Vpss failed!\n");
		    goto END_VENC_1080P_CLASSIC_2;
	    }

	    s32Ret = SAMPLE_COMM_VI_BindVpss(stViConfig.enViMode);
	    if (HI_SUCCESS != s32Ret)
	    {
		    SAMPLE_PRT("Vi bind Vpss failed!\n");
		    goto END_VENC_1080P_CLASSIC_3;
	    }

	    VpssChn = 0;
	    stVpssChnMode.enChnMode      = VPSS_CHN_MODE_USER;
	    stVpssChnMode.bDouble        = HI_FALSE;
	    stVpssChnMode.enPixelFormat  = SAMPLE_PIXEL_FORMAT;
	    stVpssChnMode.u32Width       = stSize.u32Width;
	    stVpssChnMode.u32Height      = stSize.u32Height;
	    stVpssChnMode.enCompressMode = COMPRESS_MODE_SEG;
	    memset(&stVpssChnAttr, 0, sizeof(stVpssChnAttr));
	    stVpssChnAttr.s32SrcFrameRate = -1;
	    stVpssChnAttr.s32DstFrameRate = -1;
	    s32Ret = SAMPLE_COMM_VPSS_EnableChn(VpssGrp, VpssChn, &stVpssChnAttr, &stVpssChnMode, HI_NULL);
	    if (HI_SUCCESS != s32Ret)
	    {
		    SAMPLE_PRT("Enable vpss chn failed!\n");
		    goto END_VENC_1080P_CLASSIC_4;
	    }
    }
    if(s32ChnNum >= 2)
    {
		s32Ret = SAMPLE_COMM_SYS_GetPicSize(gs_enNorm, enSize[1], &stSize);
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT(	"SAMPLE_COMM_SYS_GetPicSize failed!\n");
			goto END_VENC_1080P_CLASSIC_4;
		}
	    VpssChn = 1;
	    stVpssChnMode.enChnMode       = VPSS_CHN_MODE_USER;
	    stVpssChnMode.bDouble         = HI_FALSE;
	    stVpssChnMode.enPixelFormat   = SAMPLE_PIXEL_FORMAT;
	    stVpssChnMode.u32Width        = stSize.u32Width;
	    stVpssChnMode.u32Height       = stSize.u32Height;
	    stVpssChnMode.enCompressMode  = COMPRESS_MODE_SEG;
	    stVpssChnAttr.s32SrcFrameRate = -1;
	    stVpssChnAttr.s32DstFrameRate = -1;
	    s32Ret = SAMPLE_COMM_VPSS_EnableChn(VpssGrp, VpssChn, &stVpssChnAttr, &stVpssChnMode, HI_NULL);
	    if (HI_SUCCESS != s32Ret)
	    {
		    SAMPLE_PRT("Enable vpss chn failed!\n");
		    goto END_VENC_1080P_CLASSIC_4;
	    }
    }
    if(s32ChnNum >= 3)
    {
		s32Ret = SAMPLE_COMM_SYS_GetPicSize(gs_enNorm, enSize[2], &stSize);
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT(	"SAMPLE_COMM_SYS_GetPicSize failed!\n");
			goto END_VENC_1080P_CLASSIC_4;
		}

	    VpssChn = 2;
	    stVpssChnMode.enChnMode 	= VPSS_CHN_MODE_USER;
	    stVpssChnMode.bDouble		= HI_FALSE;
	    stVpssChnMode.enPixelFormat = SAMPLE_PIXEL_FORMAT;
	    stVpssChnMode.u32Width		= stSize.u32Width;  //720;
	    stVpssChnMode.u32Height 	= stSize.u32Height; //(VIDEO_ENCODING_MODE_PAL==gs_enNorm)?576:480;;
	    stVpssChnMode.enCompressMode = COMPRESS_MODE_NONE;

	    stVpssChnAttr.s32SrcFrameRate = -1;
	    stVpssChnAttr.s32DstFrameRate = -1;

	    s32Ret = SAMPLE_COMM_VPSS_EnableChn(VpssGrp, VpssChn, &stVpssChnAttr, &stVpssChnMode, HI_NULL);
	    if (HI_SUCCESS != s32Ret)
	    {
		    SAMPLE_PRT("Enable vpss chn failed!\n");
		    goto END_VENC_1080P_CLASSIC_4;
	    }
    }
    /******************************************
     step 5: start stream venc
    ******************************************/
    /*** HD1080P **/
    printf("\t c) cbr.\n");
    printf("\t v) vbr.\n");
	printf("\t a) avbr.\n");
    printf("\t f) fixQp\n");
    printf("please input choose rc mode!\n");
    c = (char)getchar();
    switch(c)
    {
        case 'c':
            enRcMode = SAMPLE_RC_CBR;
            break;
        case 'v':
            enRcMode = SAMPLE_RC_VBR;
            break;
		case 'a':
			enRcMode = SAMPLE_RC_AVBR;
			break;
        case 'f':
            enRcMode = SAMPLE_RC_FIXQP;
            break;
        default:
            printf("rc mode! is invaild!\n");
            goto END_VENC_1080P_CLASSIC_4;
    }

	/*** enSize[0] **/
	if(s32ChnNum >= 1)
	{
		VpssGrp = 0;
	    VpssChn = 0;
	    VencChn = 0;

		s32Ret = SAMPLE_COMM_VENC_GetGopAttr(enGopMode[0],&stGopAttr,gs_enNorm);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Get GopAttr failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }

	    s32Ret = SAMPLE_COMM_VENC_StartEx(VencChn, enPayLoad[0],\
	                                   gs_enNorm, enSize[0], enRcMode,u32Profile,&stGopAttr);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Venc failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }

	    s32Ret = SAMPLE_COMM_VENC_BindVpss(VencChn, VpssGrp, VpssChn);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Venc failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }
	}

	/*** enSize[1] **/
	if(s32ChnNum >= 2)
	{
		VpssChn = 1;
	    VencChn = 1;
		s32Ret = SAMPLE_COMM_VENC_GetGopAttr(enGopMode[1],&stGopAttr,gs_enNorm);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Get GopAttr failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }

	    s32Ret = SAMPLE_COMM_VENC_StartEx(VencChn, enPayLoad[1],\
	                                   gs_enNorm, enSize[1], enRcMode,u32Profile,&stGopAttr);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Venc failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }

	    s32Ret = SAMPLE_COMM_VENC_BindVpss(VencChn, VpssGrp, VpssChn);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Venc failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }
	}
	/*** enSize[2] **/
	if(s32ChnNum >= 3)
	{
	    VpssChn = 2;
	    VencChn = 2;
		s32Ret = SAMPLE_COMM_VENC_GetGopAttr(enGopMode[2],&stGopAttr,gs_enNorm);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Get GopAttr failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }

	    s32Ret = SAMPLE_COMM_VENC_StartEx(VencChn, enPayLoad[2],\
	                                   gs_enNorm, enSize[2], enRcMode,u32Profile,&stGopAttr);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Venc failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }

	    s32Ret = SAMPLE_COMM_VENC_BindVpss(VencChn, VpssGrp, VpssChn);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Venc failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }
	}
    /******************************************
     step 6: stream venc process -- get stream, then save it to file.
    ******************************************/
    s32Ret = SAMPLE_COMM_VENC_StartGetStream(s32ChnNum);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Venc failed!\n");
        goto END_VENC_1080P_CLASSIC_5;
    }

    printf("please press twice ENTER to exit this sample\n");
    getchar();
    getchar();

    /******************************************
     step 7: exit process
    ******************************************/
    SAMPLE_COMM_VENC_StopGetStream();

END_VENC_1080P_CLASSIC_5:

    VpssGrp = 0;
	switch(s32ChnNum)
	{
		case 3:
			VpssChn = 2;
		    VencChn = 2;
		    SAMPLE_COMM_VENC_UnBindVpss(VencChn, VpssGrp, VpssChn);
		    SAMPLE_COMM_VENC_Stop(VencChn);
		case 2:
			VpssChn = 1;
		    VencChn = 1;
		    SAMPLE_COMM_VENC_UnBindVpss(VencChn, VpssGrp, VpssChn);
		    SAMPLE_COMM_VENC_Stop(VencChn);
		case 1:
			VpssChn = 0;
		    VencChn = 0;
		    SAMPLE_COMM_VENC_UnBindVpss(VencChn, VpssGrp, VpssChn);
		    SAMPLE_COMM_VENC_Stop(VencChn);
			break;

	}
    SAMPLE_COMM_VI_UnBindVpss(stViConfig.enViMode);

END_VENC_1080P_CLASSIC_4:	//vpss stop

    VpssGrp = 0;
	switch(s32ChnNum)
	{
		case 3:
			VpssChn = 2;
			SAMPLE_COMM_VPSS_DisableChn(VpssGrp, VpssChn);
		case 2:
			VpssChn = 1;
			SAMPLE_COMM_VPSS_DisableChn(VpssGrp, VpssChn);
		case 1:
			VpssChn = 0;
			SAMPLE_COMM_VPSS_DisableChn(VpssGrp, VpssChn);
		break;

	}

END_VENC_1080P_CLASSIC_3:    //vpss stop
    SAMPLE_COMM_VI_UnBindVpss(stViConfig.enViMode);
END_VENC_1080P_CLASSIC_2:    //vpss stop
    SAMPLE_COMM_VPSS_StopGroup(VpssGrp);
END_VENC_1080P_CLASSIC_1:	//vi stop
    SAMPLE_COMM_VI_StopVi(&stViConfig);
END_VENC_1080P_CLASSIC_0:	//system exit
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}


/******************************************************************************
* function :   function DUALP :  H.265@1080@30fps+H.264@1080@30fps

******************************************************************************/
HI_S32 SAMPLE_VENC_DUALPP_CLASSIC(HI_VOID)
{
    PAYLOAD_TYPE_E enPayLoad[2]= {PT_H265,PT_H264};
    PIC_SIZE_E enSize[2] = {PIC_HD1080, PIC_HD1080};
	VENC_GOP_ATTR_S stGopAttr;
	VENC_GOP_MODE_E enGopMode[2] = {VENC_GOPMODE_DUALP,VENC_GOPMODE_DUALP};
	HI_U32 u32Profile = 0;

    VB_CONF_S stVbConf;
    SAMPLE_VI_CONFIG_S stViConfig = {0};

    VPSS_GRP VpssGrp;
    VPSS_CHN VpssChn;
    VPSS_GRP_ATTR_S stVpssGrpAttr = {0};
    VPSS_CHN_ATTR_S stVpssChnAttr = {0};
    VPSS_CHN_MODE_S stVpssChnMode;

    VENC_CHN VencChn;
    SAMPLE_RC_E enRcMode= SAMPLE_RC_CBR;

#ifndef hi3516ev100
    HI_S32 s32ChnNum=2;
#else
    HI_S32 s32ChnNum=1;
#endif

    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32BlkSize;
    SIZE_S stSize;
    char c;


    /******************************************
     step  1: init sys variable
    ******************************************/
    memset(&stVbConf,0,sizeof(VB_CONF_S));

    SAMPLE_COMM_VI_GetSizeBySensor(&enSize[0]);

    stVbConf.u32MaxPoolCnt = 128;
//	printf("s32ChnNum = %d\n",s32ChnNum);
    /*video buffer*/
	if(s32ChnNum >= 1)
    {
	    u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(gs_enNorm,\
	                enSize[0], SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH);
	    stVbConf.astCommPool[0].u32BlkSize = u32BlkSize;
    #ifndef hi3516ev100
        stVbConf.astCommPool[0].u32BlkCnt = 10;
    #else
        stVbConf.astCommPool[0].u32BlkCnt = 4;
    #endif
    }
	if(s32ChnNum >= 2)
    {
	    u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(gs_enNorm,\
	                enSize[1], SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH);
	    stVbConf.astCommPool[1].u32BlkSize = u32BlkSize;
	    stVbConf.astCommPool[1].u32BlkCnt =10;
	}
	if(s32ChnNum >= 3)
    {
	    u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(gs_enNorm,\
	                enSize[2], SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH);
	    stVbConf.astCommPool[2].u32BlkSize = u32BlkSize;
	    stVbConf.astCommPool[2].u32BlkCnt = 10;
	}

    /******************************************
     step 2: mpp system init.
    ******************************************/
    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("system init failed with %d!\n", s32Ret);
        goto END_VENC_1080P_CLASSIC_0;
    }

    /******************************************
     step 3: start vi dev & chn to capture
    ******************************************/
    stViConfig.enViMode   = SENSOR_TYPE;
    stViConfig.enRotate   = ROTATE_NONE;
    stViConfig.enNorm     = VIDEO_ENCODING_MODE_AUTO;
    stViConfig.enViChnSet = VI_CHN_SET_NORMAL;
    stViConfig.enWDRMode  = WDR_MODE_NONE;
    s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start vi failed!\n");
        goto END_VENC_1080P_CLASSIC_1;
    }

    /******************************************
     step 4: start vpss and vi bind vpss
    ******************************************/
    s32Ret = SAMPLE_COMM_SYS_GetPicSize(gs_enNorm, enSize[0], &stSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
        goto END_VENC_1080P_CLASSIC_1;
    }

    if(s32ChnNum >= 1)
    {
	    VpssGrp = 0;
	    stVpssGrpAttr.u32MaxW = stSize.u32Width;
	    stVpssGrpAttr.u32MaxH = stSize.u32Height;
	    stVpssGrpAttr.bIeEn = HI_FALSE;
	    stVpssGrpAttr.bNrEn = HI_TRUE;
	    stVpssGrpAttr.bHistEn = HI_FALSE;
	    stVpssGrpAttr.bDciEn = HI_FALSE;
	    stVpssGrpAttr.enDieMode = VPSS_DIE_MODE_NODIE;
	    stVpssGrpAttr.enPixFmt = SAMPLE_PIXEL_FORMAT;
        stVpssGrpAttr.bSharpenEn = HI_FALSE;

	    s32Ret = SAMPLE_COMM_VPSS_StartGroup(VpssGrp, &stVpssGrpAttr);
	    if (HI_SUCCESS != s32Ret)
	    {
		    SAMPLE_PRT("Start Vpss failed!\n");
		    goto END_VENC_1080P_CLASSIC_2;
	    }

	    s32Ret = SAMPLE_COMM_VI_BindVpss(stViConfig.enViMode);
	    if (HI_SUCCESS != s32Ret)
	    {
		    SAMPLE_PRT("Vi bind Vpss failed!\n");
		    goto END_VENC_1080P_CLASSIC_3;
	    }

	    VpssChn = 0;
	    stVpssChnMode.enChnMode      = VPSS_CHN_MODE_USER;
	    stVpssChnMode.bDouble        = HI_FALSE;
	    stVpssChnMode.enPixelFormat  = SAMPLE_PIXEL_FORMAT;
	    stVpssChnMode.u32Width       = stSize.u32Width;
	    stVpssChnMode.u32Height      = stSize.u32Height;
	    stVpssChnMode.enCompressMode = COMPRESS_MODE_SEG;
	    memset(&stVpssChnAttr, 0, sizeof(stVpssChnAttr));
	    stVpssChnAttr.s32SrcFrameRate = -1;
	    stVpssChnAttr.s32DstFrameRate = -1;
	    s32Ret = SAMPLE_COMM_VPSS_EnableChn(VpssGrp, VpssChn, &stVpssChnAttr, &stVpssChnMode, HI_NULL);
	    if (HI_SUCCESS != s32Ret)
	    {
		    SAMPLE_PRT("Enable vpss chn failed!\n");
		    goto END_VENC_1080P_CLASSIC_4;
	    }
    }
    if(s32ChnNum >= 2)
    {
		s32Ret = SAMPLE_COMM_SYS_GetPicSize(gs_enNorm, enSize[1], &stSize);
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT(	"SAMPLE_COMM_SYS_GetPicSize failed!\n");
			goto END_VENC_1080P_CLASSIC_4;
		}
	    VpssChn = 1;
	    stVpssChnMode.enChnMode       = VPSS_CHN_MODE_USER;
	    stVpssChnMode.bDouble         = HI_FALSE;
	    stVpssChnMode.enPixelFormat   = SAMPLE_PIXEL_FORMAT;
	    stVpssChnMode.u32Width        = stSize.u32Width;
	    stVpssChnMode.u32Height       = stSize.u32Height;
	    stVpssChnMode.enCompressMode  = COMPRESS_MODE_SEG;
	    stVpssChnAttr.s32SrcFrameRate = -1;
	    stVpssChnAttr.s32DstFrameRate = -1;
	    s32Ret = SAMPLE_COMM_VPSS_EnableChn(VpssGrp, VpssChn, &stVpssChnAttr, &stVpssChnMode, HI_NULL);
	    if (HI_SUCCESS != s32Ret)
	    {
		    SAMPLE_PRT("Enable vpss chn failed!\n");
		    goto END_VENC_1080P_CLASSIC_4;
	    }
    }
    if(s32ChnNum >= 3)
    {
		s32Ret = SAMPLE_COMM_SYS_GetPicSize(gs_enNorm, enSize[2], &stSize);
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT(	"SAMPLE_COMM_SYS_GetPicSize failed!\n");
			goto END_VENC_1080P_CLASSIC_4;
		}

	    VpssChn = 2;
	    stVpssChnMode.enChnMode 	= VPSS_CHN_MODE_USER;
	    stVpssChnMode.bDouble		= HI_FALSE;
	    stVpssChnMode.enPixelFormat = SAMPLE_PIXEL_FORMAT;
	    stVpssChnMode.u32Width		= stSize.u32Width;  //720;
	    stVpssChnMode.u32Height 	= stSize.u32Height; //(VIDEO_ENCODING_MODE_PAL==gs_enNorm)?576:480;;
	    stVpssChnMode.enCompressMode = COMPRESS_MODE_NONE;

	    stVpssChnAttr.s32SrcFrameRate = -1;
	    stVpssChnAttr.s32DstFrameRate = -1;

	    s32Ret = SAMPLE_COMM_VPSS_EnableChn(VpssGrp, VpssChn, &stVpssChnAttr, &stVpssChnMode, HI_NULL);
	    if (HI_SUCCESS != s32Ret)
	    {
		    SAMPLE_PRT("Enable vpss chn failed!\n");
		    goto END_VENC_1080P_CLASSIC_4;
	    }
    }
    /******************************************
     step 5: start stream venc
    ******************************************/
    /*** HD1080P **/
    printf("\t c) cbr.\n");
    printf("\t v) vbr.\n");
	printf("\t a) avbr.\n");
    printf("\t f) fixQp\n");
    printf("please input choose rc mode!\n");
    c = (char)getchar();
    switch(c)
    {
        case 'c':
            enRcMode = SAMPLE_RC_CBR;
            break;
        case 'v':
            enRcMode = SAMPLE_RC_VBR;
            break;
		case 'a':
			enRcMode = SAMPLE_RC_AVBR;
			break;
        case 'f':
            enRcMode = SAMPLE_RC_FIXQP;
            break;
        default:
            printf("rc mode! is invaild!\n");
            goto END_VENC_1080P_CLASSIC_4;
    }

	/*** enSize[0] **/
	if(s32ChnNum >= 1)
	{
		VpssGrp = 0;
	    VpssChn = 0;
	    VencChn = 0;

		s32Ret = SAMPLE_COMM_VENC_GetGopAttr(enGopMode[0],&stGopAttr,gs_enNorm);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Get GopAttr failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }
		stGopAttr.stDualP.u32SPInterval = 0;
	    s32Ret = SAMPLE_COMM_VENC_StartEx(VencChn, enPayLoad[0],\
	                                   gs_enNorm, enSize[0], enRcMode,u32Profile,&stGopAttr);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Venc failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }

	    s32Ret = SAMPLE_COMM_VENC_BindVpss(VencChn, VpssGrp, VpssChn);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Venc failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }
	}

	/*** enSize[1] **/
	if(s32ChnNum >= 2)
	{
		VpssChn = 1;
	    VencChn = 1;
		s32Ret = SAMPLE_COMM_VENC_GetGopAttr(enGopMode[1],&stGopAttr,gs_enNorm);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Get GopAttr failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }
		stGopAttr.stDualP.u32SPInterval = 0;
	    s32Ret = SAMPLE_COMM_VENC_StartEx(VencChn, enPayLoad[1],\
	                                   gs_enNorm, enSize[1], enRcMode,u32Profile,&stGopAttr);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Venc failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }

	    s32Ret = SAMPLE_COMM_VENC_BindVpss(VencChn, VpssGrp, VpssChn);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Venc failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }
	}
	/*** enSize[2] **/
	if(s32ChnNum >= 3)
	{
	    VpssChn = 2;
	    VencChn = 2;
		s32Ret = SAMPLE_COMM_VENC_GetGopAttr(enGopMode[2],&stGopAttr,gs_enNorm);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Get GopAttr failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }

	    s32Ret = SAMPLE_COMM_VENC_StartEx(VencChn, enPayLoad[2],\
	                                   gs_enNorm, enSize[2], enRcMode,u32Profile,&stGopAttr);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Venc failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }

	    s32Ret = SAMPLE_COMM_VENC_BindVpss(VencChn, VpssGrp, VpssChn);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Venc failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }
	}
    /******************************************
     step 6: stream venc process -- get stream, then save it to file.
    ******************************************/
    s32Ret = SAMPLE_COMM_VENC_StartGetStream(s32ChnNum);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Venc failed!\n");
        goto END_VENC_1080P_CLASSIC_5;
    }

    printf("please press twice ENTER to exit this sample\n");
    getchar();
    getchar();

    /******************************************
     step 7: exit process
    ******************************************/
    SAMPLE_COMM_VENC_StopGetStream();

END_VENC_1080P_CLASSIC_5:

    VpssGrp = 0;
	switch(s32ChnNum)
	{
		case 3:
			VpssChn = 2;
		    VencChn = 2;
		    SAMPLE_COMM_VENC_UnBindVpss(VencChn, VpssGrp, VpssChn);
		    SAMPLE_COMM_VENC_Stop(VencChn);
		case 2:
			VpssChn = 1;
		    VencChn = 1;
		    SAMPLE_COMM_VENC_UnBindVpss(VencChn, VpssGrp, VpssChn);
		    SAMPLE_COMM_VENC_Stop(VencChn);
		case 1:
			VpssChn = 0;
		    VencChn = 0;
		    SAMPLE_COMM_VENC_UnBindVpss(VencChn, VpssGrp, VpssChn);
		    SAMPLE_COMM_VENC_Stop(VencChn);
			break;

	}
    SAMPLE_COMM_VI_UnBindVpss(stViConfig.enViMode);

END_VENC_1080P_CLASSIC_4:	//vpss stop

    VpssGrp = 0;
	switch(s32ChnNum)
	{
		case 3:
			VpssChn = 2;
			SAMPLE_COMM_VPSS_DisableChn(VpssGrp, VpssChn);
		case 2:
			VpssChn = 1;
			SAMPLE_COMM_VPSS_DisableChn(VpssGrp, VpssChn);
		case 1:
			VpssChn = 0;
			SAMPLE_COMM_VPSS_DisableChn(VpssGrp, VpssChn);
		break;

	}

END_VENC_1080P_CLASSIC_3:    //vpss stop
    SAMPLE_COMM_VI_UnBindVpss(stViConfig.enViMode);
END_VENC_1080P_CLASSIC_2:    //vpss stop
    SAMPLE_COMM_VPSS_StopGroup(VpssGrp);
END_VENC_1080P_CLASSIC_1:	//vi stop
    SAMPLE_COMM_VI_StopVi(&stViConfig);
END_VENC_1080P_CLASSIC_0:	//system exit
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}




/******************************************************************************
* function :  H264 P refurbish I macro block/I slice,normal@1080+I macro block@1080+Islice @1080
******************************************************************************/
HI_S32 SAMPLE_VENC_IntraRefresh_CLASSIC(HI_VOID)
{
    PAYLOAD_TYPE_E enPayLoad[2]= {PT_H265, PT_H264};
    PIC_SIZE_E enSize[2] = {PIC_HD1080, PIC_HD1080};
	HI_U32 u32Profile = 0;

    VB_CONF_S stVbConf;
    SAMPLE_VI_CONFIG_S stViConfig = {0};

    VPSS_GRP VpssGrp;
    VPSS_CHN VpssChn;
    VPSS_GRP_ATTR_S stVpssGrpAttr = {0};
    VPSS_CHN_ATTR_S stVpssChnAttr = {0};
    VPSS_CHN_MODE_S stVpssChnMode;

    VENC_CHN VencChn;
    SAMPLE_RC_E enRcMode= SAMPLE_RC_CBR;
	VENC_PARAM_INTRA_REFRESH_S stIntraRefresh;
#ifndef hi3516ev100
    HI_S32 s32ChnNum=2;
#else
    HI_S32 s32ChnNum=1;
#endif

    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32BlkSize;
    SIZE_S stSize;
    char c;


    /******************************************
     step  1: init sys variable
    ******************************************/
    memset(&stVbConf,0,sizeof(VB_CONF_S));

    SAMPLE_COMM_VI_GetSizeBySensor(&enSize[0]);
    stVbConf.u32MaxPoolCnt = 128;
    //printf("s32ChnNum:%d,Sensor Size:%d\n",s32ChnNum,enSize[0]);
    /*video buffer*/
	if(s32ChnNum >= 1)
    {
	    u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(gs_enNorm,\
	                enSize[0], SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH);
	    stVbConf.astCommPool[0].u32BlkSize = u32BlkSize;
    #ifndef hi3516ev100
        stVbConf.astCommPool[0].u32BlkCnt = 10;
    #else
        stVbConf.astCommPool[0].u32BlkCnt = 4;
    #endif
    }
	if(s32ChnNum >= 2)
    {
	    u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(gs_enNorm,\
	                enSize[1], SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH);
	    stVbConf.astCommPool[1].u32BlkSize = u32BlkSize;
	    stVbConf.astCommPool[1].u32BlkCnt =10;
	}


    /******************************************
     step 2: mpp system init.
    ******************************************/
    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("system init failed with %d!\n", s32Ret);
        goto END_VENC_1080P_CLASSIC_0;
    }

    /******************************************
     step 3: start vi dev & chn to capture
    ******************************************/
    stViConfig.enViMode   = SENSOR_TYPE;
    stViConfig.enRotate   = ROTATE_NONE;
    stViConfig.enNorm     = VIDEO_ENCODING_MODE_AUTO;
    stViConfig.enViChnSet = VI_CHN_SET_NORMAL;
    stViConfig.enWDRMode  = WDR_MODE_NONE;
    s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start vi failed!\n");
        goto END_VENC_1080P_CLASSIC_1;
    }

    /******************************************
     step 4: start vpss and vi bind vpss
    ******************************************/
    s32Ret = SAMPLE_COMM_SYS_GetPicSize(gs_enNorm, enSize[0], &stSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
        goto END_VENC_1080P_CLASSIC_1;
    }

    if(s32ChnNum >= 1)
    {
	    VpssGrp = 0;
	    stVpssGrpAttr.u32MaxW = stSize.u32Width;
	    stVpssGrpAttr.u32MaxH = stSize.u32Height;
	    stVpssGrpAttr.bIeEn = HI_FALSE;
	    stVpssGrpAttr.bNrEn = HI_TRUE;
	    stVpssGrpAttr.bHistEn = HI_FALSE;
	    stVpssGrpAttr.bDciEn = HI_FALSE;
	    stVpssGrpAttr.enDieMode = VPSS_DIE_MODE_NODIE;
	    stVpssGrpAttr.enPixFmt = SAMPLE_PIXEL_FORMAT;
        stVpssGrpAttr.bSharpenEn = HI_FALSE;

	    s32Ret = SAMPLE_COMM_VPSS_StartGroup(VpssGrp, &stVpssGrpAttr);
	    if (HI_SUCCESS != s32Ret)
	    {
		    SAMPLE_PRT("Start Vpss failed!\n");
		    goto END_VENC_1080P_CLASSIC_2;
	    }

	    s32Ret = SAMPLE_COMM_VI_BindVpss(stViConfig.enViMode);
	    if (HI_SUCCESS != s32Ret)
	    {
		    SAMPLE_PRT("Vi bind Vpss failed!\n");
		    goto END_VENC_1080P_CLASSIC_3;
	    }

	    VpssChn = 0;
	    stVpssChnMode.enChnMode      = VPSS_CHN_MODE_USER;
	    stVpssChnMode.bDouble        = HI_FALSE;
	    stVpssChnMode.enPixelFormat  = SAMPLE_PIXEL_FORMAT;
	    stVpssChnMode.u32Width       = stSize.u32Width;
	    stVpssChnMode.u32Height      = stSize.u32Height;
	    stVpssChnMode.enCompressMode = COMPRESS_MODE_SEG;
	    memset(&stVpssChnAttr, 0, sizeof(stVpssChnAttr));
	    stVpssChnAttr.s32SrcFrameRate = -1;
	    stVpssChnAttr.s32DstFrameRate = -1;
	    s32Ret = SAMPLE_COMM_VPSS_EnableChn(VpssGrp, VpssChn, &stVpssChnAttr, &stVpssChnMode, HI_NULL);
	    if (HI_SUCCESS != s32Ret)
	    {
		    SAMPLE_PRT("Enable vpss chn failed!\n");
		    goto END_VENC_1080P_CLASSIC_4;
	    }
    }
    if(s32ChnNum >= 2)
    {
		s32Ret = SAMPLE_COMM_SYS_GetPicSize(gs_enNorm, enSize[1], &stSize);
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT(	"SAMPLE_COMM_SYS_GetPicSize failed!\n");
			goto END_VENC_1080P_CLASSIC_4;
		}
	    VpssChn = 1;
	    stVpssChnMode.enChnMode       = VPSS_CHN_MODE_USER;
	    stVpssChnMode.bDouble         = HI_FALSE;
	    stVpssChnMode.enPixelFormat   = SAMPLE_PIXEL_FORMAT;
	    stVpssChnMode.u32Width        = stSize.u32Width;
	    stVpssChnMode.u32Height       = stSize.u32Height;
	    stVpssChnMode.enCompressMode  = COMPRESS_MODE_SEG;
	    stVpssChnAttr.s32SrcFrameRate = -1;
	    stVpssChnAttr.s32DstFrameRate = -1;
	    s32Ret = SAMPLE_COMM_VPSS_EnableChn(VpssGrp, VpssChn, &stVpssChnAttr, &stVpssChnMode, HI_NULL);
	    if (HI_SUCCESS != s32Ret)
	    {
		    SAMPLE_PRT("Enable vpss chn failed!\n");
		    goto END_VENC_1080P_CLASSIC_4;
	    }
    }

    /******************************************
     step 5: start stream venc
    ******************************************/
    /*** HD1080P **/
    printf("\t c) cbr.\n");
    printf("\t v) vbr.\n");
	printf("\t a) avbr.\n");
    printf("\t f) fixQp\n");
    printf("please input choose rc mode!\n");
    c = (char)getchar();
    switch(c)
    {
        case 'c':
            enRcMode = SAMPLE_RC_CBR;
            break;
        case 'v':
            enRcMode = SAMPLE_RC_VBR;
            break;
		case 'a':
			enRcMode = SAMPLE_RC_AVBR;
			break;
        case 'f':
            enRcMode = SAMPLE_RC_FIXQP;
            break;
        default:
            printf("rc mode! is invaild!\n");
            goto END_VENC_1080P_CLASSIC_4;
    }

	/*** enSize[0] **/
	if(s32ChnNum >= 1)
	{
		VpssGrp = 0;
	    VpssChn = 0;
	    VencChn = 0;

	    s32Ret = SAMPLE_COMM_VENC_Start(VencChn, enPayLoad[0],\
	                                   gs_enNorm, enSize[0], enRcMode,u32Profile,ROTATE_NONE);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Venc failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }

	    s32Ret = SAMPLE_COMM_VENC_BindVpss(VencChn, VpssGrp, VpssChn);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Venc failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }

		s32Ret = HI_MPI_VENC_GetIntraRefresh(VencChn,&stIntraRefresh);
		stIntraRefresh.bISliceEnable  = HI_TRUE;
		stIntraRefresh.bRefreshEnable = HI_TRUE;
		stIntraRefresh.u32RefreshLineNum = (stSize.u32Height + 63)>>8;
		stIntraRefresh.u32ReqIQp = 30;

		s32Ret = HI_MPI_VENC_SetIntraRefresh(VencChn,&stIntraRefresh);
	}

	/*** enSize[1] **/
	if(s32ChnNum >= 2)
	{
		VpssChn = 1;
	    VencChn = 1;

	    s32Ret = SAMPLE_COMM_VENC_Start(VencChn, enPayLoad[1],\
	                                   gs_enNorm, enSize[1], enRcMode,u32Profile,ROTATE_NONE);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Venc failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }

	    s32Ret = SAMPLE_COMM_VENC_BindVpss(VencChn, VpssGrp, VpssChn);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Venc failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(gs_enNorm, enSize[1], &stSize);
	    if (HI_SUCCESS != s32Ret)
	    {
			SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
			goto END_VENC_1080P_CLASSIC_5;
		}

		s32Ret = HI_MPI_VENC_GetIntraRefresh(VencChn,&stIntraRefresh);
		stIntraRefresh.bISliceEnable = HI_TRUE;
		stIntraRefresh.bRefreshEnable = HI_TRUE;
		stIntraRefresh.u32RefreshLineNum = (stSize.u32Height + 15)>>6;
		stIntraRefresh.u32ReqIQp = 30;
		s32Ret = HI_MPI_VENC_SetIntraRefresh(VencChn,&stIntraRefresh);

	}

    /******************************************
     step 6: stream venc process -- get stream, then save it to file.
    ******************************************/
    s32Ret = SAMPLE_COMM_VENC_StartGetStream(s32ChnNum);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Venc failed!\n");
        goto END_VENC_1080P_CLASSIC_5;
    }

    printf("please press twice ENTER to exit this sample\n");
    getchar();
    getchar();

    /******************************************
     step 7: exit process
    ******************************************/
    SAMPLE_COMM_VENC_StopGetStream();

END_VENC_1080P_CLASSIC_5:

    VpssGrp = 0;
	switch(s32ChnNum)
	{
		case 3:
			VpssChn = 2;
		    VencChn = 2;
		    SAMPLE_COMM_VENC_UnBindVpss(VencChn, VpssGrp, VpssChn);
		    SAMPLE_COMM_VENC_Stop(VencChn);
		case 2:
			VpssChn = 1;
		    VencChn = 1;
		    SAMPLE_COMM_VENC_UnBindVpss(VencChn, VpssGrp, VpssChn);
		    SAMPLE_COMM_VENC_Stop(VencChn);
		case 1:
			VpssChn = 0;
		    VencChn = 0;
		    SAMPLE_COMM_VENC_UnBindVpss(VencChn, VpssGrp, VpssChn);
		    SAMPLE_COMM_VENC_Stop(VencChn);
			break;

	}
    SAMPLE_COMM_VI_UnBindVpss(stViConfig.enViMode);

END_VENC_1080P_CLASSIC_4:	//vpss stop

    VpssGrp = 0;
	switch(s32ChnNum)
	{
		case 3:
			VpssChn = 2;
			SAMPLE_COMM_VPSS_DisableChn(VpssGrp, VpssChn);
		case 2:
			VpssChn = 1;
			SAMPLE_COMM_VPSS_DisableChn(VpssGrp, VpssChn);
		case 1:
			VpssChn = 0;
			SAMPLE_COMM_VPSS_DisableChn(VpssGrp, VpssChn);
		break;

	}

END_VENC_1080P_CLASSIC_3:    //vpss stop
    SAMPLE_COMM_VI_UnBindVpss(stViConfig.enViMode);
END_VENC_1080P_CLASSIC_2:    //vpss stop
    SAMPLE_COMM_VPSS_StopGroup(VpssGrp);
END_VENC_1080P_CLASSIC_1:	//vi stop
    SAMPLE_COMM_VI_StopVi(&stViConfig);
END_VENC_1080P_CLASSIC_0:	//system exit
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}

/******************************************************************************
* function :  1 chn 1080p MJPEG encode and 1 chn 1080p JPEG snap
******************************************************************************/
HI_S32 SAMPLE_VENC_1080P_MJPEG_JPEG(HI_VOID)
{
    PAYLOAD_TYPE_E enPayLoad = PT_MJPEG;
    PIC_SIZE_E enSize[2] = {PIC_HD1080,PIC_HD1080};
    HI_U32 u32Profile = 0;

    VB_CONF_S stVbConf;
    SAMPLE_VI_CONFIG_S stViConfig = {0};

    VPSS_GRP VpssGrp;
    VPSS_CHN VpssChn;
    VPSS_GRP_ATTR_S stVpssGrpAttr = {0};
    VPSS_CHN_ATTR_S stVpssChnAttr = {0};
    VPSS_CHN_MODE_S stVpssChnMode;

    VENC_CHN VencChn;
    SAMPLE_RC_E enRcMode = SAMPLE_RC_CBR;
    HI_S32 s32ChnNum = 1;

    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32BlkSize;
    SIZE_S stSize;
    HI_S32 i = 0;
    char ch;

    /******************************************
     step  1: init sys variable
    ******************************************/
    memset(&stVbConf, 0, sizeof(VB_CONF_S));

    stVbConf.u32MaxPoolCnt = 128;

    SAMPLE_COMM_VI_GetSizeBySensor(&enSize[1]);

    /*video buffer*/
    {
    u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(gs_enNorm, \
	                enSize[0], SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH);
	    stVbConf.astCommPool[0].u32BlkSize = u32BlkSize;
    #ifndef hi3516ev100
        stVbConf.astCommPool[0].u32BlkCnt = 10;
    #else
        stVbConf.astCommPool[0].u32BlkCnt = 5;
    #endif
    }

#ifndef hi3516ev100
    {
	    u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(gs_enNorm,\
	                enSize[1], SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH);
	    stVbConf.astCommPool[1].u32BlkSize = u32BlkSize;
	    stVbConf.astCommPool[1].u32BlkCnt =5;
	}
#endif


    /******************************************
     step 2: mpp system init.
    ******************************************/
    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("system init failed with %d!\n", s32Ret);
        goto END_VENC_MJPEG_JPEG_0;
    }

    /******************************************
     step 3: start vi dev & chn to capture
    ******************************************/
    stViConfig.enViMode   = SENSOR_TYPE;
    stViConfig.enRotate   = ROTATE_NONE;
    stViConfig.enNorm     = VIDEO_ENCODING_MODE_AUTO;
    stViConfig.enViChnSet = VI_CHN_SET_NORMAL;
    s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start vi failed!\n");
        goto END_VENC_MJPEG_JPEG_1;
    }

    /******************************************
     step 4: start vpss and vi bind vpss
    ******************************************/
    s32Ret = SAMPLE_COMM_SYS_GetPicSize(gs_enNorm, enSize[1], &stSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
        goto END_VENC_MJPEG_JPEG_1;
    }

    VpssGrp = 0;
    stVpssGrpAttr.u32MaxW = stSize.u32Width;
    stVpssGrpAttr.u32MaxH = stSize.u32Height;
    stVpssGrpAttr.bIeEn = HI_FALSE;
    stVpssGrpAttr.bNrEn = HI_TRUE;
    stVpssGrpAttr.bHistEn = HI_FALSE;
    stVpssGrpAttr.enDieMode = VPSS_DIE_MODE_NODIE;
    stVpssGrpAttr.enPixFmt = SAMPLE_PIXEL_FORMAT;
    stVpssGrpAttr.bDciEn = HI_FALSE;
    stVpssGrpAttr.bSharpenEn = HI_FALSE;
    s32Ret = SAMPLE_COMM_VPSS_StartGroup(VpssGrp, &stVpssGrpAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Vpss failed!\n");
        goto END_VENC_MJPEG_JPEG_2;
    }

    s32Ret = SAMPLE_COMM_VI_BindVpss(stViConfig.enViMode);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Vi bind Vpss failed!\n");
        goto END_VENC_MJPEG_JPEG_3;
    }


    VpssChn = 0;
    stVpssChnMode.enChnMode     = VPSS_CHN_MODE_USER;
    stVpssChnMode.bDouble       = HI_FALSE;
    stVpssChnMode.enPixelFormat = SAMPLE_PIXEL_FORMAT;
    stVpssChnMode.u32Width      = stSize.u32Width;
    stVpssChnMode.u32Height     = stSize.u32Height;
    stVpssChnMode.enCompressMode = COMPRESS_MODE_NONE;

    memset(&stVpssChnAttr, 0, sizeof(stVpssChnAttr));
    stVpssChnAttr.s32SrcFrameRate = -1;
    stVpssChnAttr.s32DstFrameRate = -1;
    s32Ret = SAMPLE_COMM_VPSS_EnableChn(VpssGrp, VpssChn, &stVpssChnAttr, &stVpssChnMode, HI_NULL);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Enable vpss chn failed!\n");
        goto END_VENC_MJPEG_JPEG_4;
    }

#ifndef hi3516ev100
    VpssChn = 1;
    stVpssChnMode.enChnMode     = VPSS_CHN_MODE_USER;
    stVpssChnMode.bDouble       = HI_FALSE;
    stVpssChnMode.enPixelFormat = SAMPLE_PIXEL_FORMAT;
    stVpssChnMode.u32Width      = stSize.u32Width;
    stVpssChnMode.u32Height     = stSize.u32Height;
    stVpssChnMode.enCompressMode = COMPRESS_MODE_NONE;
#endif

    memset(&stVpssChnAttr, 0, sizeof(stVpssChnAttr));
    stVpssChnAttr.s32SrcFrameRate = -1;
    stVpssChnAttr.s32DstFrameRate = -1;
    s32Ret = SAMPLE_COMM_VPSS_EnableChn(VpssGrp, VpssChn, &stVpssChnAttr, &stVpssChnMode, HI_NULL);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Enable vpss chn failed!\n");
        goto END_VENC_MJPEG_JPEG_4;
    }

    /******************************************
     step 5: start stream venc
    ******************************************/
    VpssGrp = 0;
    VpssChn = 0;
    VencChn = 0;
    s32Ret = SAMPLE_COMM_VENC_Start(VencChn, enPayLoad, \
                                    gs_enNorm, enSize[0], enRcMode, u32Profile,ROTATE_NONE);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Venc failed!\n");
        goto END_VENC_MJPEG_JPEG_5;
    }

    s32Ret = SAMPLE_COMM_VENC_BindVpss(VencChn, VpssGrp, VpssChn);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Venc failed!\n");
        goto END_VENC_MJPEG_JPEG_5;
    }


	s32Ret = SAMPLE_COMM_SYS_GetPicSize(gs_enNorm, enSize[1], &stSize);
	if (HI_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
		goto END_VENC_MJPEG_JPEG_5;
	}

    VpssGrp = 0;
#ifndef hi3516ev100
    VpssChn = 1;
#else
    VpssChn = 0;
#endif
    VencChn = 1;
    s32Ret = SAMPLE_COMM_VENC_SnapStart(VencChn, &stSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start snap failed!\n");
        goto END_VENC_MJPEG_JPEG_5;
    }


    s32Ret = SAMPLE_COMM_VENC_BindVpss(VencChn, VpssGrp, VpssChn);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Venc failed!\n");
        goto END_VENC_MJPEG_JPEG_5;
    }

    /******************************************
     step 6: stream venc process -- get stream, then save it to file.
    ******************************************/
    s32Ret = SAMPLE_COMM_VENC_StartGetStream(s32ChnNum);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Venc failed!\n");
        goto END_VENC_MJPEG_JPEG_5;
    }

    printf("press 'q' to exit sample!\npress ENTER to capture one picture to file\n");
    i = 0;
    while ((ch = (char)getchar()) != 'q')
    {
        s32Ret = SAMPLE_COMM_VENC_SnapProcess(VencChn);
        if (HI_SUCCESS != s32Ret)
        {
            printf("%s: sanp process failed!\n", __FUNCTION__);
            break;
        }
        printf("snap %d success!\n", i);
        i++;
    }

    printf("please press twice ENTER to exit this sample\n");
    getchar();
    getchar();

    /******************************************
     step 8: exit process
    ******************************************/
    SAMPLE_COMM_VENC_StopGetStream();

END_VENC_MJPEG_JPEG_5:
    VpssGrp = 0;
    VpssChn = 0;
    VencChn = 0;
    SAMPLE_COMM_VENC_UnBindVpss(VencChn, VpssGrp, VpssChn);
    SAMPLE_COMM_VENC_Stop(VencChn);

#ifndef hi3516ev100
    VpssChn = 1;
#else
    VpssChn = 0;
#endif
    VencChn = 1;
    SAMPLE_COMM_VENC_UnBindVpss(VencChn, VpssGrp, VpssChn);
    SAMPLE_COMM_VENC_Stop(VencChn);
END_VENC_MJPEG_JPEG_4:    //vpss stop
    VpssGrp = 0;
    VpssChn = 0;
    SAMPLE_COMM_VPSS_DisableChn(VpssGrp, VpssChn);
    VpssChn = 1;
    SAMPLE_COMM_VPSS_DisableChn(VpssGrp, VpssChn);
END_VENC_MJPEG_JPEG_3:    //vpss stop
    SAMPLE_COMM_VI_UnBindVpss(stViConfig.enViMode);
END_VENC_MJPEG_JPEG_2:    //vpss stop
    SAMPLE_COMM_VPSS_StopGroup(VpssGrp);
END_VENC_MJPEG_JPEG_1:    //vi stop
    SAMPLE_COMM_VI_StopVi(&stViConfig);
END_VENC_MJPEG_JPEG_0:	//system exit
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}

/******************************************************************************
* function :  SAMPLE_VENC_LOW_DELAY
H.264@NormalP@1080p@30fps@OnlineLowDelay + H.265@NormalP@1080p@30fps@OnlineLowDelay

******************************************************************************/
HI_S32 SAMPLE_VENC_LOW_DELAY(HI_VOID)
{
    PAYLOAD_TYPE_E enPayLoad[2] = {PT_H264, PT_H265};
    PIC_SIZE_E enSize[2] = {PIC_HD1080, PIC_HD1080};
    HI_U32 u32Profile = 0;

    VB_CONF_S stVbConf;
    SAMPLE_VI_CONFIG_S stViConfig = {0};
    //HI_U32 u32Priority;

    VPSS_GRP VpssGrp;
    VPSS_CHN VpssChn;
    VPSS_GRP_ATTR_S stVpssGrpAttr = {0};
    VPSS_CHN_ATTR_S stVpssChnAttr = {0};
    VPSS_CHN_MODE_S stVpssChnMode;
    VPSS_LOW_DELAY_INFO_S stLowDelayInfo;

    VENC_CHN VencChn;
    SAMPLE_RC_E enRcMode = SAMPLE_RC_CBR;
#ifndef hi3516ev100
    HI_S32 s32ChnNum=2;
#else
    HI_S32 s32ChnNum=1;
#endif

    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32BlkSize;
    SIZE_S stSize;
    char c;

    /******************************************
     step  1: init sys variable
    ******************************************/
    memset(&stVbConf, 0, sizeof(VB_CONF_S));
    stVbConf.u32MaxPoolCnt = 128;

    SAMPLE_COMM_VI_GetSizeBySensor(&enSize[0]);

    /*video buffer*/
    u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(gs_enNorm, \
                 enSize[0], SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH);

    printf("u32BlkSize: %d\n", u32BlkSize);
    stVbConf.astCommPool[0].u32BlkSize = u32BlkSize;
#ifndef hi3516ev100
    stVbConf.astCommPool[0].u32BlkCnt = 10;
#else
    stVbConf.astCommPool[0].u32BlkCnt = 4;
#endif

#ifndef hi3516ev100
    u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(gs_enNorm, \
                 enSize[1], SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH);
    stVbConf.astCommPool[1].u32BlkSize = u32BlkSize;
    stVbConf.astCommPool[1].u32BlkCnt = 10;
#endif
    /******************************************
     step 2: mpp system init.
    ******************************************/
    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("system init failed with %d!\n", s32Ret);
        goto END_VENC_LOW_DELAY_0;
    }

    /******************************************
     step 3: start vi dev & chn to capture
    ******************************************/
    stViConfig.enViMode   = SENSOR_TYPE;
    stViConfig.enRotate   = ROTATE_NONE;
    stViConfig.enNorm     = VIDEO_ENCODING_MODE_AUTO;
    stViConfig.enViChnSet = VI_CHN_SET_NORMAL;
    s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start vi failed!\n");
        goto END_VENC_LOW_DELAY_1;
    }

    /******************************************
     step 4: start vpss and vi bind vpss
    ******************************************/
    s32Ret = SAMPLE_COMM_SYS_GetPicSize(gs_enNorm, enSize[0], &stSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
        goto END_VENC_LOW_DELAY_1;
    }


    VpssGrp = 0;
    stVpssGrpAttr.u32MaxW = stSize.u32Width;
    stVpssGrpAttr.u32MaxH = stSize.u32Height;
    stVpssGrpAttr.bIeEn = HI_FALSE;
    stVpssGrpAttr.bNrEn = HI_FALSE;
    stVpssGrpAttr.bHistEn = HI_FALSE;
    stVpssGrpAttr.bDciEn = HI_FALSE;
    stVpssGrpAttr.enDieMode = VPSS_DIE_MODE_NODIE;
    stVpssGrpAttr.enPixFmt = SAMPLE_PIXEL_FORMAT;
        stVpssGrpAttr.bSharpenEn = HI_FALSE;
        s32Ret = SAMPLE_COMM_VPSS_StartGroup(VpssGrp, &stVpssGrpAttr);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("Start Vpss failed!\n");
            goto END_VENC_LOW_DELAY_2;
        }

    s32Ret = SAMPLE_COMM_VI_BindVpss(stViConfig.enViMode);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Vi bind Vpss failed!\n");
        goto END_VENC_LOW_DELAY_3;
    }

    VpssChn = 0;
    stVpssChnMode.enChnMode     = VPSS_CHN_MODE_USER;
    stVpssChnMode.bDouble       = HI_FALSE;
    stVpssChnMode.enPixelFormat = SAMPLE_PIXEL_FORMAT;
    stVpssChnMode.u32Width      = stSize.u32Width;
    stVpssChnMode.u32Height     = stSize.u32Height;
    stVpssChnMode.enCompressMode = COMPRESS_MODE_NONE;
    memset(&stVpssChnAttr, 0, sizeof(stVpssChnAttr));
    stVpssChnAttr.s32SrcFrameRate = -1;
    stVpssChnAttr.s32DstFrameRate = -1;
    s32Ret = SAMPLE_COMM_VPSS_EnableChn(VpssGrp, VpssChn, &stVpssChnAttr, &stVpssChnMode, HI_NULL);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Enable vpss chn failed!\n");
        goto END_VENC_LOW_DELAY_4;
    }

#ifndef hi3516ev100
    VpssChn = 1;
    stVpssChnMode.enChnMode     = VPSS_CHN_MODE_USER;
    stVpssChnMode.bDouble       = HI_FALSE;
    stVpssChnMode.enPixelFormat = SAMPLE_PIXEL_FORMAT;
    stVpssChnMode.u32Width      = stSize.u32Width;
    stVpssChnMode.u32Height     = stSize.u32Height;
    stVpssChnMode.enCompressMode = COMPRESS_MODE_NONE;
    stVpssChnAttr.s32SrcFrameRate = -1;
    stVpssChnAttr.s32DstFrameRate = -1;
    s32Ret = SAMPLE_COMM_VPSS_EnableChn(VpssGrp, VpssChn, &stVpssChnAttr, &stVpssChnMode, HI_NULL);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Enable vpss chn failed!\n");
        goto END_VENC_LOW_DELAY_4;
    }
#endif
    /******************************************
     step 5: start stream venc
    ******************************************/
    /*** HD1080P **/
    printf("\t c) cbr.\n");
    printf("\t v) vbr.\n");
	printf("\t a) avbr.\n");
    printf("\t f) fixQp\n");
    printf("please input choose rc mode!\n");
    c = (char)getchar();
    switch (c)
    {
        case 'c':
            enRcMode = SAMPLE_RC_CBR;
            break;
        case 'v':
            enRcMode = SAMPLE_RC_VBR;
            break;
		case 'a':
			enRcMode = SAMPLE_RC_AVBR;
			break;
        case 'f':
            enRcMode = SAMPLE_RC_FIXQP;
            break;
        default:
            printf("rc mode! is invaild!\n");
            goto END_VENC_LOW_DELAY_4;
    }

        VpssGrp = 0;
    VpssChn = 0;
    VencChn = 0;


    s32Ret = SAMPLE_COMM_VENC_Start(VencChn, enPayLoad[0], \
                                gs_enNorm, enSize[0], enRcMode, u32Profile,ROTATE_NONE);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("Start Venc failed!\n");
            goto END_VENC_LOW_DELAY_5;
        }

        s32Ret = SAMPLE_COMM_VENC_BindVpss(VencChn, VpssGrp, VpssChn);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("Start Venc failed!\n");
            goto END_VENC_LOW_DELAY_5;
        }

#if 0
    /*set chnl Priority*/
    s32Ret = HI_MPI_VENC_GetChnlPriority(VencChn, &u32Priority);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Get Chnl Priority failed!\n");
        goto END_VENC_LOW_DELAY_5;
    }

    u32Priority = 1;

    s32Ret = HI_MPI_VENC_SetChnlPriority(VencChn, u32Priority);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Set Chnl Priority failed!\n");
        goto END_VENC_LOW_DELAY_5;
    }
#endif

        /*set low delay,only one which low delay channel was enabled*/
#if 1
        s32Ret = HI_MPI_VPSS_GetLowDelayAttr(VpssGrp, VpssChn, &stLowDelayInfo);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("HI_MPI_VPSS_GetLowDelayAttr failed!\n");
            goto END_VENC_LOW_DELAY_5;
        }
        stLowDelayInfo.bEnable = HI_TRUE;
        stLowDelayInfo.u32LineCnt = stVpssChnMode.u32Height / 2;
        s32Ret = HI_MPI_VPSS_SetLowDelayAttr(VpssGrp, VpssChn, &stLowDelayInfo);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("HI_MPI_VPSS_SetLowDelayAttr failed!\n");
            goto END_VENC_LOW_DELAY_5;
        }
#endif
        /*** 1080p **/

#ifndef hi3516ev100
            VpssChn = 1;
    VencChn = 1;

    s32Ret = SAMPLE_COMM_VENC_Start(VencChn, enPayLoad[1], \
                                    gs_enNorm, enSize[1], enRcMode, u32Profile,ROTATE_NONE);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("Start Venc failed!\n");
            goto END_VENC_LOW_DELAY_5;
        }

        s32Ret = SAMPLE_COMM_VENC_BindVpss(VencChn, VpssGrp, VpssChn);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("Start Venc failed!\n");
            goto END_VENC_LOW_DELAY_5;
    }
#endif
    /******************************************
     step 6: stream venc process -- get stream, then save it to file.
    ******************************************/
    s32Ret = SAMPLE_COMM_VENC_StartGetStream(s32ChnNum);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Venc failed!\n");
        goto END_VENC_LOW_DELAY_5;
    }

    printf("please press twice ENTER to exit this sample\n");
    getchar();
    getchar();

    /******************************************
     step 7: exit process
    ******************************************/
    SAMPLE_COMM_VENC_StopGetStream();

END_VENC_LOW_DELAY_5:
    VpssGrp = 0;

    VpssChn = 0;
    VencChn = 0;
    SAMPLE_COMM_VENC_UnBindVpss(VencChn, VpssGrp, VpssChn);
    SAMPLE_COMM_VENC_Stop(VencChn);

#ifndef hi3516ev100
    VpssChn = 1;
    VencChn = 1;
    SAMPLE_COMM_VENC_UnBindVpss(VencChn, VpssGrp, VpssChn);
    SAMPLE_COMM_VENC_Stop(VencChn);
#endif

    SAMPLE_COMM_VI_UnBindVpss(stViConfig.enViMode);
END_VENC_LOW_DELAY_4:   //vpss stop
    VpssGrp = 0;
    VpssChn = 0;
    SAMPLE_COMM_VPSS_DisableChn(VpssGrp, VpssChn);
#ifndef hi3516ev100
    VpssChn = 1;
    SAMPLE_COMM_VPSS_DisableChn(VpssGrp, VpssChn);
#endif
END_VENC_LOW_DELAY_3:    //vpss stop
    SAMPLE_COMM_VI_UnBindVpss(stViConfig.enViMode);
END_VENC_LOW_DELAY_2:    //vpss stop
    SAMPLE_COMM_VPSS_StopGroup(VpssGrp);
END_VENC_LOW_DELAY_1:   //vi stop
    SAMPLE_COMM_VI_StopVi(&stViConfig);
END_VENC_LOW_DELAY_0:   //system exit
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}



HI_S32 SAMPLE_VENC_ROIBG_CLASSIC(HI_VOID)
{
    PAYLOAD_TYPE_E enPayLoad[2] = {PT_H264,PT_H265};
    PIC_SIZE_E enSize[3] = {PIC_HD1080, PIC_HD1080, PIC_D1};
    HI_U32 u32Profile = 0;

    VB_CONF_S stVbConf;
    SAMPLE_VI_CONFIG_S stViConfig = {0};

    VPSS_GRP VpssGrp;
    VPSS_CHN VpssChn;
    VPSS_GRP_ATTR_S stVpssGrpAttr = {0};
    VPSS_CHN_ATTR_S stVpssChnAttr = {0};
    VPSS_CHN_MODE_S stVpssChnMode;
    VENC_ROI_CFG_S  stVencRoiCfg;
    VENC_ROIBG_FRAME_RATE_S stRoiBgFrameRate;

    VENC_CHN VencChn;
    SAMPLE_RC_E enRcMode = SAMPLE_RC_CBR;
#ifndef hi3516ev100
    HI_S32 s32ChnNum=2;
#else
    HI_S32 s32ChnNum=1;
#endif

    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32BlkSize;
    SIZE_S stSize;
    char c;

    /******************************************
     step  1: init sys variable
    ******************************************/
    memset(&stVbConf, 0, sizeof(VB_CONF_S));
    SAMPLE_COMM_VI_GetSizeBySensor(&enSize[0]);

    stVbConf.u32MaxPoolCnt = 128;

    /*video buffer*/
    u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(gs_enNorm, \
                 enSize[0], SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH);
    stVbConf.astCommPool[0].u32BlkSize = u32BlkSize;
#ifndef hi3516ev100
    stVbConf.astCommPool[0].u32BlkCnt = 10;
#else
    stVbConf.astCommPool[0].u32BlkCnt = 5;
#endif

#ifndef hi3516ev100
    u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(gs_enNorm, \
                 enSize[1], SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH);
    stVbConf.astCommPool[1].u32BlkSize = u32BlkSize;
    stVbConf.astCommPool[1].u32BlkCnt = 10;

    u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(gs_enNorm, \
                 enSize[2], SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH);
    stVbConf.astCommPool[2].u32BlkSize = u32BlkSize;
    stVbConf.astCommPool[2].u32BlkCnt = 0;
#endif

    /******************************************
     step 2: mpp system init.
    ******************************************/
    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("system init failed with %d!\n", s32Ret);
        goto END_VENC_1080P_CLASSIC_0;
    }

    /******************************************
     step 3: start vi dev & chn to capture
    ******************************************/
    stViConfig.enViMode   = SENSOR_TYPE;
    stViConfig.enRotate   = ROTATE_NONE;
    stViConfig.enNorm     = VIDEO_ENCODING_MODE_AUTO;
    stViConfig.enViChnSet = VI_CHN_SET_NORMAL;
    s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start vi failed!\n");
        goto END_VENC_1080P_CLASSIC_1;
    }

    /******************************************
     step 4: start vpss and vi bind vpss
    ******************************************/
    s32Ret = SAMPLE_COMM_SYS_GetPicSize(gs_enNorm, enSize[0], &stSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
        goto END_VENC_1080P_CLASSIC_1;
    }
	if(s32ChnNum >= 1)
	{
	    VpssGrp = 0;
	    stVpssGrpAttr.u32MaxW = stSize.u32Width;
	    stVpssGrpAttr.u32MaxH = stSize.u32Height;
	    stVpssGrpAttr.bIeEn = HI_FALSE;
	    stVpssGrpAttr.bNrEn = HI_TRUE;
	    stVpssGrpAttr.bHistEn = HI_FALSE;
	    stVpssGrpAttr.bDciEn = HI_FALSE;
	    stVpssGrpAttr.enDieMode = VPSS_DIE_MODE_NODIE;
	    stVpssGrpAttr.enPixFmt = SAMPLE_PIXEL_FORMAT;
	    stVpssGrpAttr.bSharpenEn = HI_FALSE;
	    s32Ret = SAMPLE_COMM_VPSS_StartGroup(VpssGrp, &stVpssGrpAttr);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Vpss failed!\n");
	        goto END_VENC_1080P_CLASSIC_2;
	    }

	    s32Ret = SAMPLE_COMM_VI_BindVpss(stViConfig.enViMode);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Vi bind Vpss failed!\n");
	        goto END_VENC_1080P_CLASSIC_3;
	    }

	    VpssChn = 0;
	    stVpssChnMode.enChnMode     = VPSS_CHN_MODE_USER;
	    stVpssChnMode.bDouble       = HI_FALSE;
	    stVpssChnMode.enPixelFormat = SAMPLE_PIXEL_FORMAT;
	    stVpssChnMode.u32Width      = stSize.u32Width;
	    stVpssChnMode.u32Height     = stSize.u32Height;
	    stVpssChnMode.enCompressMode = COMPRESS_MODE_NONE;
	    memset(&stVpssChnAttr, 0, sizeof(stVpssChnAttr));
	    stVpssChnAttr.s32SrcFrameRate = -1;
	    stVpssChnAttr.s32DstFrameRate = -1;
	    s32Ret = SAMPLE_COMM_VPSS_EnableChn(VpssGrp, VpssChn, &stVpssChnAttr, &stVpssChnMode, HI_NULL);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Enable vpss chn failed!\n");
	        goto END_VENC_1080P_CLASSIC_4;
	    }
	}

	if(s32ChnNum >= 2)
	{
		s32Ret = SAMPLE_COMM_SYS_GetPicSize(gs_enNorm, enSize[1], &stSize);
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT( "SAMPLE_COMM_SYS_GetPicSize failed!\n");
			goto END_VENC_1080P_CLASSIC_4;
		}
		VpssChn = 1;
	    stVpssChnMode.enChnMode     = VPSS_CHN_MODE_USER;
	    stVpssChnMode.bDouble       = HI_FALSE;
	    stVpssChnMode.enPixelFormat = SAMPLE_PIXEL_FORMAT;
	    stVpssChnMode.u32Width      = stSize.u32Width;
	    stVpssChnMode.u32Height     = stSize.u32Height;
	    stVpssChnMode.enCompressMode = COMPRESS_MODE_NONE;
	    memset(&stVpssChnAttr, 0, sizeof(stVpssChnAttr));
	    stVpssChnAttr.s32SrcFrameRate = -1;
	    stVpssChnAttr.s32DstFrameRate = -1;
	    s32Ret = SAMPLE_COMM_VPSS_EnableChn(VpssGrp, VpssChn, &stVpssChnAttr, &stVpssChnMode, HI_NULL);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Enable vpss chn failed!\n");
	        goto END_VENC_1080P_CLASSIC_4;
	    }
	}
    /******************************************
     step 5: start stream venc
    ******************************************/
    /*** HD1080P **/
    printf("\t c) cbr.\n");
    printf("\t v) vbr.\n");
	printf("\t a) avbr.\n");
    printf("\t f) fixQp\n");
    printf("please input choose rc mode!\n");
    c = (char)getchar();
    switch (c)
    {
        case 'c':
            enRcMode = SAMPLE_RC_CBR;
            break;
        case 'v':
            enRcMode = SAMPLE_RC_VBR;
            break;
		case 'a':
			enRcMode = SAMPLE_RC_AVBR;
			break;
        case 'f':
            enRcMode = SAMPLE_RC_FIXQP;
            break;
        default:
            printf("rc mode! is invaild!\n");
            goto END_VENC_1080P_CLASSIC_4;
    }
	if(s32ChnNum >= 1)
	{
	    VpssGrp = 0;
	    VpssChn = 0;
	    VencChn = 0;

	    s32Ret = SAMPLE_COMM_VENC_Start(VencChn, enPayLoad[0], \
	                                    gs_enNorm, enSize[0], enRcMode, u32Profile,ROTATE_NONE);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Venc failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }

	    s32Ret = SAMPLE_COMM_VENC_BindVpss(VencChn, VpssGrp, VpssChn);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Venc failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }
	    stVencRoiCfg.bAbsQp   = HI_TRUE;
	    stVencRoiCfg.bEnable  = HI_TRUE;
	    stVencRoiCfg.s32Qp    = 30;
	    stVencRoiCfg.u32Index = 0;
	    stVencRoiCfg.stRect.s32X = 64;
	    stVencRoiCfg.stRect.s32Y = 64;
	    stVencRoiCfg.stRect.u32Height = 256;
	    stVencRoiCfg.stRect.u32Width = 256;
	    s32Ret = HI_MPI_VENC_SetRoiCfg(VencChn, &stVencRoiCfg);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Venc failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }

	    s32Ret = HI_MPI_VENC_GetRoiBgFrameRate(VencChn, &stRoiBgFrameRate);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("HI_MPI_VENC_GetRoiBgFrameRate failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }
	    stRoiBgFrameRate.s32SrcFrmRate = (VIDEO_ENCODING_MODE_PAL == gs_enNorm) ? 25 : 30;
	    stRoiBgFrameRate.s32DstFrmRate = (VIDEO_ENCODING_MODE_PAL == gs_enNorm) ? 5 : 15;

	    s32Ret = HI_MPI_VENC_SetRoiBgFrameRate(VencChn, &stRoiBgFrameRate);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("HI_MPI_VENC_SetRoiBgFrameRate!\n");
	        goto END_VENC_1080P_CLASSIC_5;
		}
	}
	if(s32ChnNum >= 2)
	{
		VpssGrp = 0;
	    VpssChn = 1;
	    VencChn = 1;
	    s32Ret = SAMPLE_COMM_VENC_Start(VencChn, enPayLoad[1], \
	                                    gs_enNorm, enSize[1], enRcMode, u32Profile,ROTATE_NONE);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Venc failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }
	    s32Ret = SAMPLE_COMM_VENC_BindVpss(VencChn, VpssGrp, VpssChn);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Venc failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }
	    stVencRoiCfg.bAbsQp   = HI_TRUE;
	    stVencRoiCfg.bEnable  = HI_TRUE;
	    stVencRoiCfg.s32Qp    = 30;
	    stVencRoiCfg.u32Index = 0;
	    stVencRoiCfg.stRect.s32X = 64;
	    stVencRoiCfg.stRect.s32Y = 64;
	    stVencRoiCfg.stRect.u32Height = 256;
	    stVencRoiCfg.stRect.u32Width = 256;
	    s32Ret = HI_MPI_VENC_SetRoiCfg(VencChn, &stVencRoiCfg);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("Start Venc failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }
	    s32Ret = HI_MPI_VENC_GetRoiBgFrameRate(VencChn, &stRoiBgFrameRate);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("HI_MPI_VENC_GetRoiBgFrameRate failed!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }
	    stRoiBgFrameRate.s32SrcFrmRate = (VIDEO_ENCODING_MODE_PAL == gs_enNorm) ? 25 : 30;
	    stRoiBgFrameRate.s32DstFrmRate = (VIDEO_ENCODING_MODE_PAL == gs_enNorm) ? 5 : 15;
	    s32Ret = HI_MPI_VENC_SetRoiBgFrameRate(VencChn, &stRoiBgFrameRate);
	    if (HI_SUCCESS != s32Ret)
	    {
	        SAMPLE_PRT("HI_MPI_VENC_SetRoiBgFrameRate!\n");
	        goto END_VENC_1080P_CLASSIC_5;
	    }
    }
    /******************************************
     step 6: stream venc process -- get stream, then save it to file.
    ******************************************/
    s32Ret = SAMPLE_COMM_VENC_StartGetStream(s32ChnNum);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Venc failed!\n");
        goto END_VENC_1080P_CLASSIC_5;
    }

    printf("please press twice ENTER to exit this sample\n");
    getchar();
    getchar();

    /******************************************
     step 7: exit process
    ******************************************/
    SAMPLE_COMM_VENC_StopGetStream();

END_VENC_1080P_CLASSIC_5:
    VpssGrp = 0;

    VpssChn = 0;
    VencChn = 0;
    SAMPLE_COMM_VENC_UnBindVpss(VencChn, VpssGrp, VpssChn);
    SAMPLE_COMM_VENC_Stop(VencChn);



    SAMPLE_COMM_VI_UnBindVpss(stViConfig.enViMode);
END_VENC_1080P_CLASSIC_4:	//vpss stop
    VpssGrp = 0;
    VpssChn = 0;
    SAMPLE_COMM_VPSS_DisableChn(VpssGrp, VpssChn);
END_VENC_1080P_CLASSIC_3:    //vpss stop
    SAMPLE_COMM_VI_UnBindVpss(stViConfig.enViMode);
END_VENC_1080P_CLASSIC_2:    //vpss stop
    SAMPLE_COMM_VPSS_StopGroup(VpssGrp);
END_VENC_1080P_CLASSIC_1:	//vi stop
    SAMPLE_COMM_VI_StopVi(&stViConfig);
END_VENC_1080P_CLASSIC_0:	//system exit
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}

HI_S32 SAMPLE_VENC_SVC_H264(HI_VOID)
{
    PAYLOAD_TYPE_E enPayLoad = PT_H264;
    PIC_SIZE_E enSize[3] = {PIC_HD1080, PIC_HD720, PIC_D1};
    HI_U32 u32Profile = 3;/* Svc-t */

    VB_CONF_S stVbConf;
    SAMPLE_VI_CONFIG_S stViConfig = {0};

    VPSS_GRP VpssGrp;
    VPSS_CHN VpssChn;
    VPSS_GRP_ATTR_S stVpssGrpAttr = {0};
    VPSS_CHN_ATTR_S stVpssChnAttr = {0};
    VPSS_CHN_MODE_S stVpssChnMode;


    VENC_CHN VencChn;
    SAMPLE_RC_E enRcMode = SAMPLE_RC_CBR;
    HI_S32 s32ChnNum = 1;

    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32BlkSize;
    SIZE_S stSize;
    char c;

    /******************************************
     step  1: init sys variable
    ******************************************/
    memset(&stVbConf, 0, sizeof(VB_CONF_S));

    SAMPLE_COMM_VI_GetSizeBySensor(&enSize[0]);

    stVbConf.u32MaxPoolCnt = 128;

    /*video buffer*/
    u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(gs_enNorm, \
                 enSize[0], SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH);
    stVbConf.astCommPool[0].u32BlkSize = u32BlkSize;
#ifndef hi3516ev100
    stVbConf.astCommPool[0].u32BlkCnt = 10;
#else
    stVbConf.astCommPool[0].u32BlkCnt = 4;
#endif

#ifndef hi3516ev100
    u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(gs_enNorm, \
                 enSize[1], SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH);
    stVbConf.astCommPool[1].u32BlkSize = u32BlkSize;
    stVbConf.astCommPool[1].u32BlkCnt = 6;

    u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(gs_enNorm, \
                 enSize[2], SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH);
    stVbConf.astCommPool[2].u32BlkSize = u32BlkSize;
    stVbConf.astCommPool[2].u32BlkCnt = 6;
#endif


    /******************************************
     step 2: mpp system init.
    ******************************************/
    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("system init failed with %d!\n", s32Ret);
        goto END_VENC_1080P_CLASSIC_0;
    }

    /******************************************
     step 3: start vi dev & chn to capture
    ******************************************/
    stViConfig.enViMode   = SENSOR_TYPE;
    stViConfig.enRotate   = ROTATE_NONE;
    stViConfig.enNorm	  = VIDEO_ENCODING_MODE_AUTO;
    stViConfig.enViChnSet = VI_CHN_SET_NORMAL;
    s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start vi failed!\n");
        goto END_VENC_1080P_CLASSIC_1;
    }

    /******************************************
     step 4: start vpss and vi bind vpss
    ******************************************/
    s32Ret = SAMPLE_COMM_SYS_GetPicSize(gs_enNorm, enSize[0], &stSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
        goto END_VENC_1080P_CLASSIC_1;
    }

    VpssGrp = 0;
    stVpssGrpAttr.u32MaxW = stSize.u32Width;
    stVpssGrpAttr.u32MaxH = stSize.u32Height;
    stVpssGrpAttr.bIeEn = HI_FALSE;
    stVpssGrpAttr.bNrEn = HI_TRUE;
    stVpssGrpAttr.bHistEn = HI_FALSE;
    stVpssGrpAttr.bDciEn = HI_FALSE;
    stVpssGrpAttr.enDieMode = VPSS_DIE_MODE_NODIE;
    stVpssGrpAttr.enPixFmt = SAMPLE_PIXEL_FORMAT;
    stVpssGrpAttr.bSharpenEn = HI_FALSE;
    s32Ret = SAMPLE_COMM_VPSS_StartGroup(VpssGrp, &stVpssGrpAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Vpss failed!\n");
        goto END_VENC_1080P_CLASSIC_2;
    }

    s32Ret = SAMPLE_COMM_VI_BindVpss(stViConfig.enViMode);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Vi bind Vpss failed!\n");
        goto END_VENC_1080P_CLASSIC_3;
    }

    VpssChn = 0;
    stVpssChnMode.enChnMode 	= VPSS_CHN_MODE_USER;
    stVpssChnMode.bDouble		= HI_FALSE;
    stVpssChnMode.enPixelFormat = SAMPLE_PIXEL_FORMAT;
    stVpssChnMode.u32Width		= stSize.u32Width;
    stVpssChnMode.u32Height 	= stSize.u32Height;
    stVpssChnMode.enCompressMode = COMPRESS_MODE_NONE;
    memset(&stVpssChnAttr, 0, sizeof(stVpssChnAttr));
    stVpssChnAttr.s32SrcFrameRate = -1;
    stVpssChnAttr.s32DstFrameRate = -1;
    s32Ret = SAMPLE_COMM_VPSS_EnableChn(VpssGrp, VpssChn, &stVpssChnAttr, &stVpssChnMode, HI_NULL);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Enable vpss chn failed!\n");
        goto END_VENC_1080P_CLASSIC_4;
    }

    /******************************************
     step 5: start stream venc
    ******************************************/
    /*** HD1080P **/
    printf("\t c) cbr.\n");
    printf("\t v) vbr.\n");
	printf("\t a) avbr.\n");
    printf("\t f) fixQp\n");
    printf("please input choose rc mode!\n");
    c = (char)getchar();
    switch (c)
    {
        case 'c':
            enRcMode = SAMPLE_RC_CBR;
            break;
        case 'v':
            enRcMode = SAMPLE_RC_VBR;
            break;
		case 'a':
			enRcMode = SAMPLE_RC_AVBR;
			break;
        case 'f':
            enRcMode = SAMPLE_RC_FIXQP;
            break;
        default:
            printf("rc mode! is invaild!\n");
            goto END_VENC_1080P_CLASSIC_4;
    }
    VpssGrp = 0;
    VpssChn = 0;
    VencChn = 0;

    s32Ret = SAMPLE_COMM_VENC_Start(VencChn, enPayLoad, \
                                    gs_enNorm, enSize[0], enRcMode, u32Profile,ROTATE_NONE);

    printf("SAMPLE_COMM_VENC_Start is ok\n");

    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Venc failed!\n");
        goto END_VENC_1080P_CLASSIC_5;
    }

    s32Ret = SAMPLE_COMM_VENC_BindVpss(VencChn, VpssGrp, VpssChn);

    printf("SAMPLE_COMM_VENC_BindVpss is ok\n");


    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Venc failed!\n");
        goto END_VENC_1080P_CLASSIC_5;
    }

    /******************************************
     step 6: stream venc process -- get stream, then save it to file.
    ******************************************/
    s32Ret = SAMPLE_COMM_VENC_StartGetStream_Svc_t(s32ChnNum);

    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Venc failed!\n");
        goto END_VENC_1080P_CLASSIC_5;
    }

    printf("please press twice ENTER to exit this sample\n");
    getchar();
    getchar();

    /******************************************
     step 7: exit process
    ******************************************/
    SAMPLE_COMM_VENC_StopGetStream();

    printf("SAMPLE_COMM_VENC_StopGetStream is ok\n");
END_VENC_1080P_CLASSIC_5:
    VpssGrp = 0;

    VpssChn = 0;
    VencChn = 0;
    SAMPLE_COMM_VENC_UnBindVpss(VencChn, VpssGrp, VpssChn);
    SAMPLE_COMM_VENC_Stop(VencChn);



    SAMPLE_COMM_VI_UnBindVpss(stViConfig.enViMode);
END_VENC_1080P_CLASSIC_4:	//vpss stop
    VpssGrp = 0;
    VpssChn = 0;
    SAMPLE_COMM_VPSS_DisableChn(VpssGrp, VpssChn);
END_VENC_1080P_CLASSIC_3:	 //vpss stop
    SAMPLE_COMM_VI_UnBindVpss(stViConfig.enViMode);
END_VENC_1080P_CLASSIC_2:	 //vpss stop
    SAMPLE_COMM_VPSS_StopGroup(VpssGrp);
END_VENC_1080P_CLASSIC_1:	//vi stop
    SAMPLE_COMM_VI_StopVi(&stViConfig);
END_VENC_1080P_CLASSIC_0:	//system exit
    SAMPLE_COMM_SYS_Exit();
    return s32Ret;
}


/******************************************************************************
* function :  SAMPLE_VENC_EPTZ
H.264@NormalP@1080p@30fps + H.265@NormalP@1080p@30fps@EPTZ

******************************************************************************/
HI_S32 SAMPLE_VENC_EPTZ(HI_VOID)
{
    PAYLOAD_TYPE_E enPayLoad[2] = {PT_H264, PT_H265};
    PIC_SIZE_E enSize[2] = {PIC_HD1080, PIC_HD720};
    HI_U32 u32Profile = 0;

    VB_CONF_S stVbConf;
    SAMPLE_VI_CONFIG_S stViConfig = {0};
    //HI_U32 u32Priority;

    VPSS_GRP VpssGrp;
    VPSS_CHN VpssChn;
    VPSS_GRP_ATTR_S stVpssGrpAttr;
    VPSS_CHN_ATTR_S stVpssChnAttr;
    VPSS_CHN_MODE_S stVpssChnMode;
	HI_U32 u32SrcWidth,u32SrcHeigth,u32DstWidth,u32DstHeigth;

    VENC_CHN VencChn;
    SAMPLE_RC_E enRcMode = SAMPLE_RC_CBR;
    HI_S32 s32ChnNum = 2;

    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32BlkSize;
    SIZE_S stSize;
    char c;

    /******************************************
     step  1: init sys variable
    ******************************************/
    memset(&stVbConf, 0, sizeof(VB_CONF_S));
    stVbConf.u32MaxPoolCnt = 128;

    SAMPLE_COMM_VI_GetSizeBySensor(&enSize[0]);

    /*video buffer*/
    u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(gs_enNorm, \
                 enSize[0], SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH);

    printf("u32BlkSize: %d\n", u32BlkSize);
    stVbConf.astCommPool[0].u32BlkSize = u32BlkSize;
    stVbConf.astCommPool[0].u32BlkCnt = 10;

    u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(gs_enNorm, \
                 enSize[1], SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH);
    stVbConf.astCommPool[1].u32BlkSize = u32BlkSize;
    stVbConf.astCommPool[1].u32BlkCnt = 10;

    /******************************************
     step 2: mpp system init.
    ******************************************/
    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("system init failed with %d!\n", s32Ret);
        goto END_VENC_EPTZ_0;
    }

    /******************************************
     step 3: start vi dev & chn to capture
    ******************************************/
    stViConfig.enViMode   = SENSOR_TYPE;
    stViConfig.enRotate   = ROTATE_NONE;
    stViConfig.enNorm     = VIDEO_ENCODING_MODE_AUTO;
    stViConfig.enViChnSet = VI_CHN_SET_NORMAL;
    s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start vi failed!\n");
        goto END_VENC_EPTZ_1;
    }

    /******************************************
     step 4: start vpss and vi bind vpss
    ******************************************/
    s32Ret = SAMPLE_COMM_SYS_GetPicSize(gs_enNorm, enSize[0], &stSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
        goto END_VENC_EPTZ_1;
    }


    VpssGrp = 0;
    stVpssGrpAttr.u32MaxW = stSize.u32Width;
    stVpssGrpAttr.u32MaxH = stSize.u32Height;
    stVpssGrpAttr.bIeEn = HI_FALSE;
    stVpssGrpAttr.bNrEn = HI_FALSE;
    stVpssGrpAttr.bHistEn = HI_FALSE;
    stVpssGrpAttr.bDciEn = HI_FALSE;
    stVpssGrpAttr.enDieMode = VPSS_DIE_MODE_NODIE;
    stVpssGrpAttr.enPixFmt = SAMPLE_PIXEL_FORMAT;
    stVpssGrpAttr.bSharpenEn = HI_FALSE;
    s32Ret = SAMPLE_COMM_VPSS_StartGroup(VpssGrp, &stVpssGrpAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Vpss failed!\n");
        goto END_VENC_EPTZ_2;
    }

    s32Ret = SAMPLE_COMM_VI_BindVpss(stViConfig.enViMode);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Vi bind Vpss failed!\n");
        goto END_VENC_EPTZ_3;
    }

    VpssChn = 0;
    stVpssChnMode.enChnMode     = VPSS_CHN_MODE_USER;
    stVpssChnMode.bDouble       = HI_FALSE;
    stVpssChnMode.enPixelFormat = SAMPLE_PIXEL_FORMAT;
    stVpssChnMode.u32Width      = stSize.u32Width;
    stVpssChnMode.u32Height     = stSize.u32Height;
    stVpssChnMode.enCompressMode = COMPRESS_MODE_NONE;
    memset(&stVpssChnAttr, 0, sizeof(stVpssChnAttr));
    stVpssChnAttr.s32SrcFrameRate = -1;
    stVpssChnAttr.s32DstFrameRate = -1;
    s32Ret = SAMPLE_COMM_VPSS_EnableChn(VpssGrp, VpssChn, &stVpssChnAttr, &stVpssChnMode, HI_NULL);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Enable vpss chn failed!\n");
        goto END_VENC_EPTZ_4;
    }

    VpssChn = 1;
    stVpssChnMode.enChnMode     = VPSS_CHN_MODE_USER;
    stVpssChnMode.bDouble       = HI_FALSE;
    stVpssChnMode.enPixelFormat = SAMPLE_PIXEL_FORMAT;
    stVpssChnMode.u32Width      = stSize.u32Width;
    stVpssChnMode.u32Height     = stSize.u32Height;
    stVpssChnMode.enCompressMode = COMPRESS_MODE_NONE;
    stVpssChnAttr.s32SrcFrameRate = -1;
    stVpssChnAttr.s32DstFrameRate = -1;
    s32Ret = SAMPLE_COMM_VPSS_EnableChn(VpssGrp, VpssChn, &stVpssChnAttr, &stVpssChnMode, HI_NULL);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Enable vpss chn failed!\n");
        goto END_VENC_EPTZ_4;
    }

    /******************************************
     step 5: start stream venc
    ******************************************/
    /*** HD1080P **/
    printf("\t c) cbr.\n");
    printf("\t v) vbr.\n");
	printf("\t a) avbr.\n");
    printf("\t f) fixQp\n");
    printf("please input choose rc mode!\n");
    c = (char)getchar();
    switch (c)
    {
        case 'c':
            enRcMode = SAMPLE_RC_CBR;
            break;
        case 'v':
            enRcMode = SAMPLE_RC_VBR;
            break;
		case 'a':
			enRcMode = SAMPLE_RC_AVBR;
			break;
        case 'f':
            enRcMode = SAMPLE_RC_FIXQP;
            break;
        default:
            printf("rc mode! is invaild!\n");
            goto END_VENC_EPTZ_4;
    }

        VpssGrp = 0;
        VpssChn = 0;
        VencChn = 0;

        s32Ret = SAMPLE_COMM_VENC_Start(VencChn, enPayLoad[0], \
                                        gs_enNorm, enSize[0], enRcMode, u32Profile,ROTATE_NONE);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("Start Venc failed!\n");
            goto END_VENC_EPTZ_5;
        }

        s32Ret = SAMPLE_COMM_VENC_BindVpss(VencChn, VpssGrp, VpssChn);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("Start Venc failed!\n");
            goto END_VENC_EPTZ_5;
        }


        /*** 1080p **/
        VpssChn = 1;
        VencChn = 1;
        s32Ret = SAMPLE_COMM_VENC_Start(VencChn, enPayLoad[1], \
                                        gs_enNorm, enSize[1], enRcMode, u32Profile,ROTATE_NONE);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("Start Venc failed!\n");
            goto END_VENC_EPTZ_5;
        }

        s32Ret = SAMPLE_COMM_VENC_BindVpss(VencChn, VpssGrp, VpssChn);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("Start Venc failed!\n");
            goto END_VENC_EPTZ_5;
        }


   /******************************************
     step 6: stream venc process -- get stream, then save it to file.
    ******************************************/
    s32Ret = SAMPLE_COMM_VENC_StartGetStream(s32ChnNum);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Venc failed!\n");
        goto END_VENC_EPTZ_5;
    }

	/*set vpss chnl crop proc*/
	VpssGrp = 0;
	VpssChn = 1;
	u32SrcWidth = stVpssChnMode.u32Width;
	u32SrcHeigth = stVpssChnMode.u32Height;
	u32DstWidth = 1280;
	u32DstHeigth = 720;
	s32Ret = SAMPLE_COMM_VPSS_SetChnCropProc(VpssGrp,VpssChn,u32SrcWidth,u32SrcHeigth,u32DstWidth,u32DstHeigth);
	if (HI_SUCCESS != s32Ret)
	{
	 SAMPLE_PRT("Start Venc failed!\n");
	 goto END_VENC_EPTZ_5;
	}

    printf("please press twice ENTER to exit this sample\n");
    getchar();
    getchar();

    /******************************************
     step 7: exit process
    ******************************************/
    SAMPLE_COMM_VENC_StopGetStream();
	SAMPLE_COMM_VPSS_StopChnCropProc();

END_VENC_EPTZ_5:
    VpssGrp = 0;

    VpssChn = 0;
    VencChn = 0;
    SAMPLE_COMM_VENC_UnBindVpss(VencChn, VpssGrp, VpssChn);
    SAMPLE_COMM_VENC_Stop(VencChn);

    VpssChn = 1;
    VencChn = 1;
    SAMPLE_COMM_VENC_UnBindVpss(VencChn, VpssGrp, VpssChn);
    SAMPLE_COMM_VENC_Stop(VencChn);

    SAMPLE_COMM_VI_UnBindVpss(stViConfig.enViMode);
END_VENC_EPTZ_4:   //vpss stop
    VpssGrp = 0;
    VpssChn = 0;
    SAMPLE_COMM_VPSS_DisableChn(VpssGrp, VpssChn);
    VpssChn = 1;
    SAMPLE_COMM_VPSS_DisableChn(VpssGrp, VpssChn);
END_VENC_EPTZ_3:    //vpss stop
    SAMPLE_COMM_VI_UnBindVpss(stViConfig.enViMode);
END_VENC_EPTZ_2:    //vpss stop
    SAMPLE_COMM_VPSS_StopGroup(VpssGrp);
END_VENC_EPTZ_1:   //vi stop
    SAMPLE_COMM_VI_StopVi(&stViConfig);
END_VENC_EPTZ_0:   //system exit
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}

/******************************************************************************
* function    : main()
* Description : video venc sample
******************************************************************************/
#if 0
#ifdef __HuaweiLite__
int app_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    HI_S32 s32Ret;
    if ( (argc < 2) || (1 != strlen(argv[1])))
    {
        SAMPLE_VENC_Usage(argv[0]);
        return HI_FAILURE;
    }

#ifndef __HuaweiLite__
    signal(SIGINT, SAMPLE_VENC_HandleSig);
    signal(SIGTERM, SAMPLE_VENC_HandleSig);
#endif

    switch (*argv[1])
    {
        case '0':/* H.264@NormalP@1080p@30fps+H.265NormalP@1080p@30fps*/
            s32Ret = SAMPLE_VENC_NORMALP_CLASSIC();
            break;

		case '1':/*H.264@DualP@1080p@30fps + H.265@SmartP@1080p@30fps*/
    		s32Ret = SAMPLE_VENC_SMARTP_CLASSIC();
            break;

		case '2':/*H.264@SmartP@1080p@30fps + H.265@DualP@1080p@30fps*/
			s32Ret = SAMPLE_VENC_DUALP_CLASSIC();
			break;

		case '3':/* H.265@1080@30fps+H.265@1080@30fps */
            s32Ret = SAMPLE_VENC_DUALPP_CLASSIC();
            break;

        case '4':/* H.264@NormalP@1080p@30fps@P_Islice + H.265@NormalP@1080p@30fps@P_Islice*/
            s32Ret = SAMPLE_VENC_IntraRefresh_CLASSIC();
            break;

		case '5':/* 1*1080p mjpeg encode + 1*1080p jpeg  */
            s32Ret = SAMPLE_VENC_1080P_MJPEG_JPEG();
            break;

        case '6':/* H.264@NormalP@1080p@30fps@OnlineLowDelay + H.265@NormalP@1080p@30fps@OnlineLowDelay*/
            s32Ret = SAMPLE_VENC_LOW_DELAY();
            break;

        case '7':/* H.264@NormalP@1080p@30fps@RoiBgFrameRate+ H.265@NormalP@1080p@30fps@RoiBgFrameRate*/
            s32Ret = SAMPLE_VENC_ROIBG_CLASSIC();
            break;
		case '8':/* H.264 Svc-t */
            s32Ret = SAMPLE_VENC_SVC_H264();
            break;
#ifndef hi3516ev100
		case '9':/* H.264@NormalP@1080p@30fps+ H.265@NormalP@1080p@30fps@EPTZ */
            s32Ret = SAMPLE_VENC_EPTZ();
            break;
#endif
        default:
            SAMPLE_VENC_Usage(argv[0]);
            return HI_FAILURE;
    }

    if (HI_SUCCESS == s32Ret)
    {
        printf("program exit normally!\n");
    }
    else
    {
        printf("program exit abnormally!\n");
    }

    exit(s32Ret);

    return s32Ret;
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
