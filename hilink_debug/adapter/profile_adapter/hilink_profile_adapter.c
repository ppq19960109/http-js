/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: HiLink产品适配实现源文件
 * Create: 2019-04-20
 * Notes: 该文件中的接口需要对外提供给第三方厂商使用，为了前向兼容，部分老接口暂不按最新编码规范整改.
 */
#include "hilink_profile_adapter.h"
#include "net_info.h"

/* 设备类型定义 */
typedef struct
{
    const char *sn;     /* 设备唯一标识，比如sn号，长度范围（0,40] */
    const char *prodId; /* 设备HiLink认证号，长度范围（0,5] */
    const char *model;  /* 设备型号，长度范围（0,32] */
    const char *dev_t;  /* 设备类型，长度范围（0,4] */
    const char *manu;   /* 设备制造商，长度范围（0,4] */
    const char *mac;    /* 设备MAC地址，固定32字节 */
    const char *hiv;    /* 设备Hilink协议版本，长度范围（0,32] */
    const char *fwv;    /* 设备固件版本，长度范围[0,64] */
    const char *hwv;    /* 设备硬件版本，长度范围[0,64] */
    const char *swv;    /* 设备软件版本，长度范围[0,64] */
    int prot_t;         /* 设备协议类型，取值范围[1,3] */
} dev_info_t;

/* 服务类型定义 */
typedef struct
{
    const char *st;     /* 服务类型，长度范围（0,32] */
    const char *svc_id; /* 服务ID，长度范围（0,64] */
} svc_info_t;

/* 设备信息定义 */
dev_info_t dev_info = {
    "Device SN",
    PRODUCT_ID,
    DEVICE_MODEL,
    DEVICE_TYPE,
    MANUAFACTURER,
    "Device Mac",
    "1.0.0",
    "1.0.0",
    "1.0.0",
    "1.0.0",
    PROTOCOL_TYPE};

// void dev_info_init(dev_info_t *dev_info)
// {
//     dev_info->prodId = PRODUCT_ID;
//     dev_info->hiv = "1.0.0";
//     dev_info->fwv = "1.0.0";
//     dev_info->hwv = "1.0.0";
//     dev_info->swv = "1.0.0";
//     dev_info->prot_t = PROTOCOL_TYPE;
// }
/* 设备类型英文名和厂商英文名长度之和不能超过17字节 */
typedef struct
{
    const char *devTypeName; /* 设备类型英文名称 */
    const char *manuName;    /* 厂商英文名称 */
} DevNameEn;

/* 设备名称定义 */
DevNameEn g_devNameEn = {
    DEVICE_TYPE_NAME,
    MANUAFACTURER_NAME};

/* 服务信息定义 */
int gSvcNum = 1;
svc_info_t gSvcInfo[] = {
    {"binarySwitch", "switch"}};

/* AC信息 */
unsigned char A_C[48] = {
    0x49, 0x3F, 0x45, 0x4A, 0x3A, 0x72, 0x38, 0x7B, 0x36, 0x32, 0x50, 0x3C, 0x49, 0x39, 0x62, 0x38,
    0x72, 0xCB, 0x6D, 0xC5, 0xAE, 0xE5, 0x4A, 0x82, 0xD3, 0xE5, 0x6D, 0xF5, 0x36, 0x82, 0x62, 0xEB,
    0x89, 0x30, 0x6C, 0x88, 0x32, 0x56, 0x23, 0xFD, 0xB8, 0x67, 0x90, 0xA7, 0x7B, 0x61, 0x1E, 0xAE};

/* BI信息 */
char *bi_rsacipher = "611E3824A1C3EC57E2D62BD2CFA1279C"
                     "4AE847BB4EDD69011A68C92C37005BA1"
                     "F8664921ED8FA283870296CA532375D1"
                     "FD13B7E0A5220A2F5D7AFA4AF9E41084"
                     "589D25295A659258432E76D1F12EDEA8"
                     "205816556310DA1FF0462F6C931EF3EC"
                     "CECC34E88B22A32E519FC1DE0D290506"
                     "C463A4BD32EA20479F725453972C8BD8"
                     "9D0EF9FA056B7FC2CE77A19D4D4000E7"
                     "A6EE4FCF81329DEAFDD174627C915125"
                     "FF4C233620497E8B7C8998B070D2FB1F"
                     "CB3FC7640769B2FBB5ADAE7B5B2DFEFF"
                     "E527CF155DED9A73E066694703316AB2"
                     "6B448B19F91BAFE51AD45C71D2B622EB"
                     "206FF15B17F655F23DBA781D1AF110F7"
                     "76E38F60CB11BEFD1CE44169E9B53FE2";

/* 获取加密 AC 参数  */
unsigned char *hilink_get_auto_ac(void)
{
    return A_C;
}

/* 获取加密 BI 参数 */
char *hilink_get_auto_bi_rsa_cipher(void)
{
    return bi_rsacipher;
}

/*
 * 修改服务当前字段值
 * svcId为服务的ID，payload为接收到需要修改的Json格式的字段与其值，len为payload的长度
 * 返回0表示服务状态值修改成功，不需要底层设备主动上报，由Hilink Device SDK上报；
 * 返回-101表示获得报文不符合要求；
 * 返回-111表示服务状态值正在修改中，修改成功后底层设备必须主动上报；
 */
int hilink_put_char_state(const char *svcId, const char *payload, unsigned int len)
{
    log_info("svcId:%d\n", svcId);
    log_info("payload:%s,len:%d\n", payload, len);
    cJSON *root = cJSON_Parse(payload);
    free(root);
    return 0;
}

/*
 * 获取服务字段值
 * svcId表示服务ID。厂商实现该函数时，需要对svcId进行判断；
 * in表示接收到的Json格式的字段与其值；
 * inLen表示接收到的in的长度；
 * out表示保存服务字段值内容的指针,内存由厂商开辟，使用完成后，由Hilink Device SDK释放；
 * outLen表示读取到的payload的长度；
 * 返回0表示服务状态字段值获取成功，返回非0表示获取服务状态字段值不成功。
 */
int hilink_get_char_state(const char *svcId, const char *in, unsigned int inLen, char **out, unsigned int *outLen)
{
    log_info("svcId:%d\n", svcId);
    log_info("in:%s,len:%d\n", in, inLen);
    cJSON *root = cJSON_Parse(in);
    free(root);
    return 0;
}

/*
 * 获取设备sn号
 * 注意: sn指向的字符串长度为0时将使用设备mac地址作为sn
 */
void HilinkGetDeviceSn(unsigned int len, char *sn)
{
    /* 在此处添加实现代码, 将sn赋值给*sn回传 */
    return;
}

/*
 * 获取设备相关版本号
 * 返回0表示版本号获取成功，返回其他表示版本号获取失败
 * 注意，此接口为HiLink内部调用函数
 */
int getDeviceVersion(char **firmwareVer, char **softwareVer, char **hardwareVer)
{
    *firmwareVer = FIRMWARE_VER;
    *softwareVer = SOFTWARE_VER;
    *hardwareVer = HARDWARE_VER;
    return 0;
}

/*
 * 获取SoftAp配网PIN码
 * 返回值为8位数字PIN码, 返回-1表示使用HiLink SDK的默认PIN码
 * 该接口需设备开发者实现
 */
int HiLinkGetPinCode(void)
{
    /* 测试时，这个数字可以随便改，只要是8位数字即可 */
    return 12345678;
}

/*
 * 查询当前设备敏感性标识
 * 返回0为非敏感设备，返回1为敏感设备
 */
int HILINK_IsSensitiveDevice(void)
{
    return 0;
}