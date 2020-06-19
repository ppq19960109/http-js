/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: HiLink��־�������
 */

#ifndef _HILINK_LOG_MANAGE_H_
#define _HILINK_LOG_MANAGE_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HILINK_LOG_INVALID = -1, /* �Ƿ�ֵ */
    HILINK_LOG_MIN     = 0,
    HILINK_LOG_EMERG   = 0, /* �ǳ�����, ϵͳ������ */
    HILINK_LOG_ALERT   = 1, /* ��������, ����������ȡ��ʩ */
    HILINK_LOG_CRIT    = 2, /* �������� */
    HILINK_LOG_ERR     = 3, /* ���󼶱� */
    HILINK_LOG_WARN    = 4, /* �澯���� */
    HILINK_LOG_NOTICE  = 5, /* ֪ͨ���� */
    HILINK_LOG_INFO    = 6, /* ��Ϣ���� */
    HILINK_LOG_DEBUG   = 7, /* ���Լ��� */
    HILINK_LOG_MAX     = HILINK_LOG_DEBUG
} HiLinkLogLevel;

/*
 * ���õ�ǰ��־��ӡ��߼���, release�汾Ĭ��ΪHILINK_LOG_ERR, debug�汾Ĭ��ΪHILINK_LOG_DEBUG
 * �������õ���־����ΪHILINK_LOG_ERR, ��ʾ������HILINK_LOG_ERR����(HILINK_LOG_WARN��)����־�����ӡ����
 */
void HILINK_SetLogLevel(HiLinkLogLevel level);

/*
 * ��ѯ��ǰ��־��ӡ��߼���
 * �����ѯ����־����ΪHILINK_LOG_ERR, ��ʾ������HILINK_LOG_ERR����(HILINK_LOG_WARN��)����־�����ӡ����
 */
HiLinkLogLevel HILINK_GetLogLevel(void);

#ifdef __cplusplus
}
#endif

#endif /* _HILINK_LOG_MANAGE_H_ */
