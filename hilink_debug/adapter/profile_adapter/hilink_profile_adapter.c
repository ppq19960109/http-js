/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: HiLink��Ʒ����ʵ��Դ�ļ�
 * Create: 2019-04-20
 * Notes: ���ļ��еĽӿ���Ҫ�����ṩ������������ʹ�ã�Ϊ��ǰ����ݣ������Ͻӿ��ݲ������±���淶����.
 */
#include "hilink_profile_adapter.h"
#include "net_info.h"

/* �豸���Ͷ��� */
typedef struct
{
    const char *sn;     /* �豸Ψһ��ʶ������sn�ţ����ȷ�Χ��0,40] */
    const char *prodId; /* �豸HiLink��֤�ţ����ȷ�Χ��0,5] */
    const char *model;  /* �豸�ͺţ����ȷ�Χ��0,32] */
    const char *dev_t;  /* �豸���ͣ����ȷ�Χ��0,4] */
    const char *manu;   /* �豸�����̣����ȷ�Χ��0,4] */
    const char *mac;    /* �豸MAC��ַ���̶�32�ֽ� */
    const char *hiv;    /* �豸HilinkЭ��汾�����ȷ�Χ��0,32] */
    const char *fwv;    /* �豸�̼��汾�����ȷ�Χ[0,64] */
    const char *hwv;    /* �豸Ӳ���汾�����ȷ�Χ[0,64] */
    const char *swv;    /* �豸����汾�����ȷ�Χ[0,64] */
    int prot_t;         /* �豸Э�����ͣ�ȡֵ��Χ[1,3] */
} dev_info_t;

/* �������Ͷ��� */
typedef struct
{
    const char *st;     /* �������ͣ����ȷ�Χ��0,32] */
    const char *svc_id; /* ����ID�����ȷ�Χ��0,64] */
} svc_info_t;

/* �豸��Ϣ���� */
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

/* �豸����Ӣ�����ͳ���Ӣ��������֮�Ͳ��ܳ���17�ֽ� */
typedef struct
{
    const char *devTypeName; /* �豸����Ӣ������ */
    const char *manuName;    /* ����Ӣ������ */
} DevNameEn;

/* �豸���ƶ��� */
DevNameEn g_devNameEn = {
    DEVICE_TYPE_NAME,
    MANUAFACTURER_NAME};

/* ������Ϣ���� */
int gSvcNum = 1;
svc_info_t gSvcInfo[] = {
    {"binarySwitch", "switch"}};

/* AC��Ϣ */
unsigned char A_C[48] = {
    0x49, 0x3F, 0x45, 0x4A, 0x3A, 0x72, 0x38, 0x7B, 0x36, 0x32, 0x50, 0x3C, 0x49, 0x39, 0x62, 0x38,
    0x72, 0xCB, 0x6D, 0xC5, 0xAE, 0xE5, 0x4A, 0x82, 0xD3, 0xE5, 0x6D, 0xF5, 0x36, 0x82, 0x62, 0xEB,
    0x89, 0x30, 0x6C, 0x88, 0x32, 0x56, 0x23, 0xFD, 0xB8, 0x67, 0x90, 0xA7, 0x7B, 0x61, 0x1E, 0xAE};

/* BI��Ϣ */
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

/* ��ȡ���� AC ����  */
unsigned char *hilink_get_auto_ac(void)
{
    return A_C;
}

/* ��ȡ���� BI ���� */
char *hilink_get_auto_bi_rsa_cipher(void)
{
    return bi_rsacipher;
}

/*
 * �޸ķ���ǰ�ֶ�ֵ
 * svcIdΪ�����ID��payloadΪ���յ���Ҫ�޸ĵ�Json��ʽ���ֶ�����ֵ��lenΪpayload�ĳ���
 * ����0��ʾ����״ֵ̬�޸ĳɹ�������Ҫ�ײ��豸�����ϱ�����Hilink Device SDK�ϱ���
 * ����-101��ʾ��ñ��Ĳ�����Ҫ��
 * ����-111��ʾ����״ֵ̬�����޸��У��޸ĳɹ���ײ��豸���������ϱ���
 */
int hilink_put_char_state(const char *svcId, const char *payload, unsigned int len)
{
    log_info("svcId:%d\n", svcId);
    log_info("payload:%s,len:%d\n", payload, len);
    cJSON *root = cJSON_Parse(payload);

    return 0;
}

/*
 * ��ȡ�����ֶ�ֵ
 * svcId��ʾ����ID������ʵ�ָú���ʱ����Ҫ��svcId�����жϣ�
 * in��ʾ���յ���Json��ʽ���ֶ�����ֵ��
 * inLen��ʾ���յ���in�ĳ��ȣ�
 * out��ʾ��������ֶ�ֵ���ݵ�ָ��,�ڴ��ɳ��̿��٣�ʹ����ɺ���Hilink Device SDK�ͷţ�
 * outLen��ʾ��ȡ����payload�ĳ��ȣ�
 * ����0��ʾ����״̬�ֶ�ֵ��ȡ�ɹ������ط�0��ʾ��ȡ����״̬�ֶ�ֵ���ɹ���
 */
int hilink_get_char_state(const char *svcId, const char *in, unsigned int inLen, char **out, unsigned int *outLen)
{
    log_info("svcId:%d\n", svcId);
    log_info("in:%s,len:%d\n", in, inLen);
    cJSON *root = cJSON_Parse(in);
    
    return 0;
}

/*
 * ��ȡ�豸sn��
 * ע��: snָ����ַ�������Ϊ0ʱ��ʹ���豸mac��ַ��Ϊsn
 */
void HilinkGetDeviceSn(unsigned int len, char *sn)
{
    /* �ڴ˴����ʵ�ִ���, ��sn��ֵ��*sn�ش� */
    return;
}

/*
 * ��ȡ�豸��ذ汾��
 * ����0��ʾ�汾�Ż�ȡ�ɹ�������������ʾ�汾�Ż�ȡʧ��
 * ע�⣬�˽ӿ�ΪHiLink�ڲ����ú���
 */
int getDeviceVersion(char **firmwareVer, char **softwareVer, char **hardwareVer)
{
    *firmwareVer = FIRMWARE_VER;
    *softwareVer = SOFTWARE_VER;
    *hardwareVer = HARDWARE_VER;
    return 0;
}

/*
 * ��ȡSoftAp����PIN��
 * ����ֵΪ8λ����PIN��, ����-1��ʾʹ��HiLink SDK��Ĭ��PIN��
 * �ýӿ����豸������ʵ��
 */
int HiLinkGetPinCode(void)
{
    /* ����ʱ��������ֿ������ģ�ֻҪ��8λ���ּ��� */
    return 12345678;
}

/*
 * ��ѯ��ǰ�豸�����Ա�ʶ
 * ����0Ϊ�������豸������1Ϊ�����豸
 */
int HILINK_IsSensitiveDevice(void)
{
    return 0;
}