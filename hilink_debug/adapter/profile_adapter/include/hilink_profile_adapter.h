/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: HiLink��Ʒ����ͷ�ļ�
 * Create: 2019-04-20
 * Notes: ���ļ��еĽӿ���Ҫ�����ṩ������������ʹ�ã�Ϊ��ǰ����ݣ������Ͻӿ��ݲ������±���淶����.
 */
#ifndef __HILINK_PROFILE_ADAPTER_H__
#define __HILINK_PROFILE_ADAPTER_H__

#ifdef __cplusplus
extern "C" {
#endif

/* �豸�汾��Ϣ */
#define FIRMWARE_VER  "20000"
#define SOFTWARE_VER  "1.0.3"
#define HARDWARE_VER  "20000"

/* �豸��Ʒ��Ϣ */
#define PRODUCT_ID    "1011"
#define DEVICE_TYPE   "01D"
#define MANUAFACTURER "011"
#define DEVICE_MODEL  "SP mini3-HL"
#define HILINK_VER    "1.0.0"

/* ��ȷ���豸����Ӣ�����ͳ���Ӣ��������֮�Ͳ�����17�ֽ� */
#define DEVICE_TYPE_NAME   "Switch"
#define MANUAFACTURER_NAME "Broadlink"

/* ͨ��Э������: WiFi */
#define PROTOCOL_TYPE 1

/* ��ȡBI �����ӿں��� */
char *hilink_get_auto_bi_rsa_cipher(void);

/* ��ȡAC �����ӿں��� */
unsigned char *hilink_get_auto_ac(void);

/*
 * �޸ķ���ǰ�ֶ�ֵ
 * svcIdΪ�����ID��payloadΪ���յ���Ҫ�޸ĵ�Json��ʽ���ֶ�����ֵ��lenΪpayload�ĳ���
 * ����0��ʾ����״ֵ̬�޸ĳɹ�������Ҫ�ײ��豸�����ϱ�����Hilink Device SDK�ϱ���
 * ����-101��ʾ��ñ��Ĳ�����Ҫ��
 * ����-111��ʾ����״ֵ̬�����޸��У��޸ĳɹ���ײ��豸���������ϱ���
 */
int hilink_put_char_state(const char *svcId, const char *payload, unsigned int len);

/*
 * ��ȡ�����ֶ�ֵ
 * svcId��ʾ����ID������ʵ�ָú���ʱ����Ҫ��svcId�����жϣ�
 * in��ʾ���յ���Json��ʽ���ֶ�����ֵ��
 * inLen��ʾ���յ���in�ĳ��ȣ�
 * out��ʾ��������ֶ�ֵ���ݵ�ָ��,�ڴ��ɳ��̿��٣�ʹ����ɺ���Hilink Device SDK�ͷţ�
 * outLen��ʾ��ȡ����payload�ĳ��ȣ�
 * ����0��ʾ����״̬�ֶ�ֵ��ȡ�ɹ������ط�0��ʾ��ȡ����״̬�ֶ�ֵ���ɹ���
 */
int hilink_get_char_state(const char *svcId, const char *in, unsigned int inLen, char **out, unsigned int *outLen);

/*
 * ��ȡ�豸sn��
 * ע��: snָ����ַ�������Ϊ0ʱ��ʹ���豸mac��ַ��Ϊsn
 */
void HilinkGetDeviceSn(unsigned int len, char *sn);

/* ��ȡ��ǰ�豸�汾�� */
int getDeviceVersion(char **firmwareVer, char **softwareVer, char **hardwareVer);

#ifdef __cplusplus
}
#endif
#endif
