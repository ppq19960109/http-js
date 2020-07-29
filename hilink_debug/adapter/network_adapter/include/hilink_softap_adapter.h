/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: softAp����ͷ�ļ�
 */
#ifndef HILINK_SOFTAP_ADAPTER_H
#define HILINK_SOFTAP_ADAPTER_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * ��ȡ�㲥ip
 * broadcastIp��ʾ���Ip�Ļ���
 * len��ʾ���Ip�Ļ��峤��
 * ����0��ʾ�ɹ�������-1��ʾʧ��
 * ע��: broadcastIpΪ���ʮ���Ƹ�ʽ
 */
int HILINK_GetBroadcastIp(char *broadcastIp, unsigned char len);

/*
 * ��������ΪAPģʽ������softAp�ȵ�
 * ssid ��ʾ���ڴ���softAp��ssid
 * ssidLen��ʾssid����, ���ȡֵ64
 * ����0��ʾ�ɹ�������-1��ʾʧ��
 */
int HILINK_StartSoftAp(const char *ssid, unsigned int ssidLen);

/*
 * �ر�softAp�ȵ㲢�������л�stationģʽ
 * ����0��ʾ�ɹ�������-1��ʾʧ��
 */
int HILINK_StopSoftAp(void);

#ifdef __cplusplus
}
#endif
#endif
