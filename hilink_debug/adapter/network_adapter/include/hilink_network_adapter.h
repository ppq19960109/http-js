/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: ��������ͷ�ļ�
 */
#ifndef HILINK_NETWORK_ADAPTER_H
#define HILINK_NETWORK_ADAPTER_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * ��ȡ����ip
 * localIp��ʾ���Ip�Ļ���
 * len��ʾ���Ip�Ļ��峤��
 * ����0��ʾ�ɹ�������-1��ʾʧ��
 * ע��: localIpΪ���ʮ���Ƹ�ʽ
 */
int HILINK_GetLocalIp(char *localIp, unsigned char len);

/*
 * ��ȡ����mac��ַ
 * mac��ʾ���MAC��ַ�Ļ���
 * len��ʾ���峤��
 * ����0��ʾ�ɹ�������-1��ʾʧ��
 * ע��: mac��ʽΪa1b2c3d4e5f6
 */
int HILINK_GetMacAddr(unsigned char *mac, unsigned char len);

/*
 * ��ȡWiFi ssid
 * ssid��ʾ���WiFi ssid�Ļ���
 * ssidLen��ʾWiFi ssid�ĳ���
 * ����0��ʾ�ɹ�������-1��ʾʧ��
 */
int HILINK_GetWiFiSsid(char *ssid, unsigned int *ssidLen);

/*
 * ����WiFi�˺���Ϣ
 * ssid��ʾWiFi ssid
 * ssidLen��ʾWiFi ssid�ĳ���
 * pwd��ʾWiFi����
 * pwdLen��ʾWiFi���볤��
 * ����0��ʾ�ɹ�������-1��ʾʧ��
 * ע�⣺(1) ssid��pwdΪ�ձ�ʾ���WiFi��Ϣ;
 *       (2) ���õ�WiFi��Ϣ��Ҫ�־û�����ȷ���豸��������Ȼ���Ի��WiFi������Ϣ
 */
int HILINK_SetWiFiInfo(const char *ssid, unsigned int ssidLen, const char *pwd, unsigned int pwdLen);

/* �Ͽ�������WiFi */
void HILINK_ReconnectWiFi(void);

/*
 * ����WiFi
 * ����0��ʾ�ɹ�������-1��ʾʧ��
 */
int HILINK_ConnectWiFi(void);

/*
 * ��ȡ����״̬
 * stateΪ0��ʾ����Ͽ��������ӵ�����δ�����ip��stateΪ1��ʾ�������ҷ����ip
 * ����0��ʾ�ɹ�������-1��ʾʧ��
 */
int HILINK_GetNetworkState(int *state);

/*
 * ��ȡ��ǰ���ӵ�WiFi�� bssid
 * bssid��ʾ���WiFi bssid�Ļ���
 * bssidLen��ʾWiFi bssid����
 * ����0��ʾ�ɹ�������-1��ʾʧ��
 */
int HILINK_GetWiFiBssid(unsigned char *bssid, unsigned char *bssidLen);

/*
 * ��ȡ��ǰ���ӵ�WiFi�ź�ǿ�ȣ���λdb
 * rssi��ʾ�ź�ǿ��
 * ����0��ʾ�ɹ�������-1��ʾʧ��
 */
int HILINK_GetWiFiRssi(signed char *rssi);

/*
 * ����HiLink SDK
 * ��ϵͳ������������������HiLink����
 * ����0��ʾ�ɹ�������-1��ʾʧ��
 */
int HILINK_Restart(void);

/* �������ͬʱ��������station */
void HILINK_SetStationNumLimit(void);

/* SoftAp���������У�����IP�߳���Ӧ��station */
void HILINK_DisconnectStation(const char *ip);

#ifdef __cplusplus
}
#endif
#endif
