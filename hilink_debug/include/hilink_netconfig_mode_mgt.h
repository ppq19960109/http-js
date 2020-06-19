/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description: HiLink SDK����ģʽ����
 */
#ifndef HILINK_NETCONFIG_MODE_MGT_H
#define HILINK_NETCONFIG_MODE_MGT_H

#ifdef __cplusplus
extern "C" {
#endif

/* HiLink SDK֧�ֵ�����ģʽ */
enum HILINK_NetConfigMode {
    HILINK_NETCONFIG_NONE, /* ��������, ͨ�����ߵ��ֶ����ӵ����� */
    HILINK_NETCONFIG_WIFI, /* HiLink SDK�ṩ��WiFi�Զ����� */
    HILINK_NETCONFIG_OTHER, /* ��������ģʽ, APP����WiFi����Ϣ, ���ɷ��յ�WiFi��Ϣ���ݺ�, ���õ�HiLink SDK */
    HILINK_NETCONFIG_BOTH, /* ��������ģʽ��WiFi������� */
    HILINK_NETCONFIG_REGISTER_ONLY, /* HiLink SDK SoftAp����������ע����Ϣ */
    HILINK_NETCONFIG_NO_SOFTAP_REGISTER_ONLY, /* ������SoftAp, PIN������������ע����Ϣ(ͨ������/4G/5G�Ƚ�������) */
    HILINK_NETCONFIG_BUTT /* �Ƿ�����ģʽ */
};

/* ���ò�Ʒ����ģʽ, ע��: ��Ҫ������HiLink SDK����֮ǰ���ñ��ӿ���������ģʽ */
int HILINK_SetNetConfigMode(enum HILINK_NetConfigMode netConfigMode);

/* ��ѯ��ǰ��Ʒ������ģʽ, ����ֵΪ��ǰ��Ʒ������ģʽ */
enum HILINK_NetConfigMode HILINK_GetNetConfigMode(void);

#ifdef __cplusplus
}
#endif

#endif
