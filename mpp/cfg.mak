#
# Automatically generated file; DO NOT EDIT.
# BVT SDK Platform Configuration
#

#
# General Setup
#
# CONFIG_HI3559AV100ES is not set
# CONFIG_HI3559AV100 is not set
# CONFIG_HI3519AV100 is not set
# CONFIG_HI3516CV500 is not set
export CONFIG_HI3516EV200=y
export CONFIG_HI_CHIP_TYPE=0x3516E200
export CONFIG_HI_ARCH=hi3516ev200
export CONFIG_UP=y
export CONFIG_ARM_ARCH_TYPE=up
export CONFIG_A7=y
export CONFIG_CPU_TYPE=a7
# CONFIG_VERSION_FPGA is not set
export CONFIG_VERSION_ASIC=y
export CONFIG_HI_FPGA=n
export CONFIG_LINUX_OS=y
# CONFIG_HUAWEI_LITEOS_OS is not set
export CONFIG_OS_TYPE=linux
export CONFIG_LINUX_4_9_y=y
export CONFIG_KERNEL_VERSION=linux-4.9.y
export CONFIG_KERNEL_ARM_HIMIX100_LINUX=y
# CONFIG_KERNEL_ARM_HIMIX200_LINUX is not set
export CONFIG_HI_CROSS=arm-himix100-linux-
export CONFIG_LIBC_TYPE=uclibc
export CONFIG_KERNEL_BIT=KERNEL_BIT_32
export CONFIG_USER_ARM_HIMIX100_LINUX=y
# CONFIG_USER_ARM_HIMIX200_LINUX is not set
export CONFIG_HI_CROSS_LIB=arm-himix100-linux-
export CONFIG_USER_BIT=USER_BIT_32
export CONFIG_RELEASE_TYPE_RELEASE=y
# CONFIG_RELEASE_TYPE_DEBUG is not set
export CONFIG_HI_RLS_MODE=HI_RELEASE

#
# Media Modules Setup
#

#
# media base config
#
# CONFIG_HI_VB_EXTPOOL_SUPPORT is not set

#
# media sys config
#
export CONFIG_HI_SYS_SUPPORT=y
export CONFIG_HI_SYS_SCALE_COEF_SUPPORT=y
# CONFIG_HI_SYS_SCALE_COEF_ONLINE_SUPPORT is not set

#
# media vi config
#
export CONFIG_HI_VI_SUPPORT=y
export CONFIG_HI_VI_BT656=y
export CONFIG_HI_VI_MIPI=y
export CONFIG_HI_ISP_2DOF_DIS_SUPPORT=y
# CONFIG_HI_VI_MONO_COLOR_FUSION is not set
# CONFIG_HI_VI_LDCV1_SUPPORT is not set
# CONFIG_HI_VI_LDCV2_SUPPORT is not set
export CONFIG_HI_VI_LDCV3_SUPPORT=y
# CONFIG_HI_VI_SENDYUV_SUPPORT is not set
# CONFIG_HI_VI_SRC_SUPPORT is not set
# CONFIG_HI_VI_STITCH_SUPPORT is not set
# CONFIG_HI_VI_CHN_EARLY_INT_SUPPORT is not set
# CONFIG_HI_VI_PARALLEL_SUPPORT is not set
# CONFIG_HI_VI_3DNRX_SUPPORT is not set
export CONFIG_HI_VI_USERPIC_SUPPORT=y
export CONFIG_HI_VI_COVEREX_OVERLAYEX_SUPPORT=y
export CONFIG_HI_VI_LUMA_SUPPORT=y
export CONFIG_HI_VI_FPN_SUPPORT=y
export CONFIG_HI_VI_EXTCHN_SUPPORT=y
# CONFIG_HI_VI_CASCADE_SUPPORT is not set
# CONFIG_HI_SNAP_SUPPORT is not set

#
# media isp config
#
export CONFIG_HI_ISP_SUPPORT=y
export CONFIG_HI_ISP_AF_SUPPORT=y
export CONFIG_HI_ISP_CR_SUPPORT=y
export CONFIG_HI_ISP_PREGAMMA_SUPPORT=y
export CONFIG_HI_ISP_GCAC_SUPPORT=y
export CONFIG_HI_ISP_CA_SUPPORT=y
export CONFIG_HI_ISP_EDGEMARK_SUPPORT=y
export CONFIG_HI_ISP_HLC_SUPPORT=y

#
# media dis config
#

#
# media vpss config
#
export CONFIG_HI_VPSS_SUPPORT=y
export CONFIG_HI_VPSS_3DNR=y
export CONFIG_HI_VPSS_CHN0_BUF_WRAP=y
# CONFIG_HI_VPSS_AUTO_SUPPORT is not set
# CONFIG_HI_VPSS_BACKUP_SUPPORT is not set
export CONFIG_HI_VPSS_LUMA_STAT_SUPPORT=y
export CONFIG_HI_VPSS_VGS_LDC_SUPPORT=y
export CONFIG_HI_VPSS_VGS_ROTATION_SUPPORT=y
export CONFIG_HI_VPSS_COVER_SUPPORT=y
export CONFIG_HI_VPSS_MOSAIC_SUPPORT=y
export CONFIG_HI_VPSS_3DNRT_SUPPORT=y
# CONFIG_HI_VPSS_DELAY_SUPPORT is not set

#
# media avs config
#

#
# media gdc config
#

#
# media vgs config
#
export CONFIG_HI_VGS_SUPPORT=y
# CONFIG_HI_VGS_STITCH_SUPPORT is not set
export CONFIG_HI_VGS_IVE_PRE_SUPPORT=y
export CONFIG_HI_VGS_LUMA_STAT_SUPPORT=y

#
# media chnl config
#
export CONFIG_HI_CHNL_SUPPORT=y

#
# media venc config
#
export CONFIG_HI_VENC_SUPPORT=y
export CONFIG_HI_H265E_SUPPORT=y
# CONFIG_HI_H265E_QUANTIZATION_SUPPORT is not set
export CONFIG_HI_H265E_USERDATA_SUPPORT=y
export CONFIG_HI_H265E_INTRA_REFRESH_SUPPORT=y
export CONFIG_HI_H264E_SUPPORT=y
export CONFIG_HI_H264E_SVC_SUPPORT=y
# CONFIG_HI_H264E_QUANTIZATION_SUPPORT is not set
export CONFIG_HI_H264E_USERDATA_SUPPORT=y
export CONFIG_HI_H264E_INTRA_REFRESH_SUPPORT=y
export CONFIG_HI_JPEGE_SUPPORT=y
export CONFIG_HI_MJPEGE_SUPPORT=y
export CONFIG_HI_JPEGE_DCF_SUPPORT=y
export CONFIG_HI_JPEGE_USERDATA_SUPPORT=y
export CONFIG_HI_VENC_LOWDELAY_SUPPORT=y
# CONFIG_HI_VENC_VPSSAUTO_SUPPORT is not set
export CONFIG_HI_VENC_OVERLAYEX_SUPPORT=y
export CONFIG_HI_VENC_FRAMEBUF_RECYCLE_SUPPORT=y
export CONFIG_HI_VENC_MPF_VGS_SUPPORT=y
export CONFIG_HI_VENC_VGS_SUPPORT=y
# CONFIG_HI_VENC_ADVSMARTP_SUPPORT is not set
export CONFIG_HI_VENC_SMARTP_SUPPORT=y
# CONFIG_HI_VENC_BIPREDB_SUPPORT is not set
export CONFIG_HI_VENC_DUALP_SUPPORT=y
export CONFIG_HI_VENC_RCNREF_SHARE_SUPPORT=y
# CONFIG_HI_VENC_TWOPASS_SUPPORT is not set
export CONFIG_HI_VENC_SKIPREF_SUPPORT=y
export CONFIG_HI_VENC_SCENE0_SUPPORT=y
export CONFIG_HI_VENC_SCENE1_SUPPORT=y
export CONFIG_HI_VENC_SCENE2_SUPPORT=y
export CONFIG_HI_RC_AVBR_SUPPORT=y
export CONFIG_HI_RC_QPMAP_SUPPORT=y
export CONFIG_HI_RC_QVBR_SUPPORT=y
export CONFIG_HI_RC_CVBR_SUPPORT=y

#
# media vdec config
#

#
# media vo config
#
export CONFIG_HI_VO_SUPPORT=y
# CONFIG_HI_VO_PLAY_CTL is not set
# CONFIG_HI_VO_LUMA is not set
# CONFIG_HI_VO_FB_SEPARATE is not set
export CONFIG_HI_VO_COVER_OSD_SUPPORT=y
export CONFIG_HI_VO_VGS=y
export CONFIG_HI_VO_GRAPH=y
# CONFIG_HI_VO_BATCH is not set

#
# media region config
#
export CONFIG_HI_REGION_SUPPORT=y

#
# media audio config
#
export CONFIG_HI_AUDIO_SUPPORT=y
export CONFIG_HI_ACODEC_SUPPORT=y
export CONFIG_HI_ACODEC_VERSION=V750
export CONFIG_HI_ACODEC_MAX_GAIN=56
export CONFIG_HI_ACODEC_MIN_GAIN=0
export CONFIG_HI_ACODEC_GAIN_STEP=3
export CONFIG_HI_ACODEC_FAST_POWER_SUPPORT=y
export CONFIG_HI_DOWNVQE_SUPPORT=y
export CONFIG_HI_TALKVQE_SUPPORT=y
export CONFIG_HI_RECORDVQE_SUPPORT=y

#
# media hdr config
#

#
# Device Driver Setup
#

#
# drv config
#
export CONFIG_DRV=y
export CONFIG_EXTDRV=y
export CONFIG_INTERDRV=y
export CONFIG_CIPHER=y
export CONFIG_HIUSER=y
# CONFIG_MIPI_TX is not set
export CONFIG_MIPI_RX=y
export CONFIG_HI_IR=y
export CONFIG_HI_WDG=y
export CONFIG_HI_SYSCFG=y

#
# Component Setup
#

#
# Component hdmi Config
#
# CONFIG_HI_HDMI_SUPPORT is not set

#
# Component hifb Config
#
export CONFIG_HI_HIFB_SUPPORT=y

#
# Component svp Config
#
export CONFIG_HI_SVP_IVE=y
export CONFIG_HI_SVP_IVE_NORM_GRAD=y
export CONFIG_HI_SVP_IVE_ST_CANDI_CORNER=y
export CONFIG_HI_SVP_IVE_CNN_EX=y

#
# Component photo Config
#

#
# Component tde Config
#
export CONFIG_HI_TDE_SUPPORT=y

#
# Component pciv Config
#
# CONFIG_HI_PCIV_SUPPORT is not set

#
# Component avs lut Config
#
# CONFIG_HI_AVS_LUT_SUPPORT is not set

#
# Component pos_query Config
#
# CONFIG_HI_POS_QUERY_SUPPORT is not set

#
# Component tzasc Config
#
# CONFIG_HI_TZASC_SUPPORT is not set

#
# Component motionfusion config
#
# CONFIG_HI_MOTIONFUSION_SUPPORT is not set

#
# HISYSLINK Setup
#

#
# hisyslink config
#
# CONFIG_HISYSLINK is not set

#
# Debug Config
#
export CONFIG_HI_GDB_NO=y
# CONFIG_HI_GDB_YES is not set
export CONFIG_HI_GDB=n
export CONFIG_HI_LOG_TRACE_SUPPORT=y
export CONFIG_HI_LOG_TRACE_ALL=y
# CONFIG_HI_LOG_TRACE_ERR is not set
# CONFIG_HI_LOG_TRACE_WARN is not set
# CONFIG_HI_LOG_TRACE_INFO is not set
export CONFIG_HI_LOG_TRACE_LEVEL=7

#
# Test Config
#
