/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: softAp����ʵ�� (���豸����ʵ��)
 */
#include "hilink_softap_adapter.h"
#include <stdio.h>

#include "net_info.h"
/*
 * ��ȡ�㲥ip
 * broadcastIp��ʾ���Ip�Ļ���
 * len��ʾ���Ip�Ļ��峤��
 * ����0��ʾ�ɹ�������-1��ʾʧ��
 * ע��: broadcastIpΪ���ʮ���Ƹ�ʽ
 */
int HILINK_GetBroadcastIp(char *broadcastIp, unsigned char len)
{
    return get_local_broadcastIp(ETH_NAME,broadcastIp,len);
}

/*
 * ��������ΪAPģʽ������softAp�ȵ�
 * ssid ��ʾ���ڴ���softAp��ssid
 * ssidLen��ʾssid����, ���ȡֵ64
 * ����0��ʾ�ɹ�������-1��ʾʧ��
 */
int HILINK_StartSoftAp(const char *ssid, unsigned int ssidLen)
{
    return 0;
}

/*
 * �ر�softAp�ȵ㲢�������л�stationģʽ
 * ����0��ʾ�ɹ�������-1��ʾʧ��
 */
int HILINK_StopSoftAp(void)
{
    return 0;
}