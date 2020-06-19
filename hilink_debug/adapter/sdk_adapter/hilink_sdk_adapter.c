/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: HiLink SDK����ʵ��Դ�ļ�
 * Create: 2019-04-20
 * Notes: ���ļ��еĽӿ���Ҫ�����ṩ������������ʹ�ã�Ϊ��ǰ����ݣ������Ͻӿ��ݲ������±���淶����.
 */
#include "hilink_sdk_adapter.h"
#include <stdio.h>
/*
 * ֪ͨ�豸��״̬
 * status��ʾ�豸��ǰ��״̬
 * ע�⣬�˺������豸���̸��ݲ�Ʒҵ��ѡ����ʵ��
 */
void hilink_notify_devstatus(int status)
{
    switch (status) {
        case HILINK_M2M_CLOUD_OFFLINE:
            printf("�豸���ƶ����ӶϿ�\n");
            /* �豸���ƶ����ӶϿ������ڴ˴����ʵ�� */
            break;
        case HILINK_M2M_CLOUD_ONLINE:
             printf("�豸�����ƶ˳ɹ�\n");
            /* �豸�����ƶ˳ɹ������ڴ˴����ʵ�� */
            break;
        case HILINK_M2M_LONG_OFFLINE:
            printf("�豸���ƶ����ӳ�ʱ��Ͽ�\n");
            /* �豸���ƶ����ӳ�ʱ��Ͽ������ڴ˴����ʵ�� */
            break;
        case HILINK_M2M_LONG_OFFLINE_REBOOT:
            printf("�豸���ƶ����ӳ�ʱ��Ͽ����������\n");
            /* �豸���ƶ����ӳ�ʱ��Ͽ���������������ڴ˴����ʵ�� */
            break;
        case HILINK_UNINITIALIZED:
            printf("HiLink�߳�δ����\n");
            /* HiLink�߳�δ���������ڴ˴����ʵ�� */
            break;
        case HILINK_LINK_UNDER_AUTO_CONFIG:
            printf("�豸��������ģʽ\n");
            /* �豸��������ģʽ�����ڴ˴����ʵ�� */
            break;
        case HILINK_LINK_CONFIG_TIMEOUT:
            printf("�豸����10���ӳ�ʱ״̬\n");
            /* �豸����10���ӳ�ʱ״̬�����ڴ˴����ʵ�� */
            break;
        case HILINK_LINK_CONNECTTING_WIFI:
            printf("�豸��������·����\n");
            /* �豸��������·���������ڴ˴����ʵ�� */
            break;
        case HILINK_LINK_CONNECTED_WIFI:
            printf("�豸�Ѿ�����·����\n");
            /* �豸�Ѿ�����·���������ڴ˴����ʵ�� */
            break;
        case HILINK_M2M_CONNECTTING_CLOUD:
            printf("�豸���������ƶ�\n");
            /* �豸���������ƶˣ����ڴ˴����ʵ�� */
            break;
        case HILINK_M2M_CLOUD_DISCONNECT:
            printf("�豸��·���������ӶϿ�\n");
            /* �豸��·���������ӶϿ������ڴ˴����ʵ�� */
            break;
        case HILINK_DEVICE_REGISTERED:
            printf("�豸��ע��\n");
            /* �豸��ע�ᣬ���ڴ˴����ʵ�� */
            break;
        case HILINK_DEVICE_UNREGISTER:
            printf("�豸�����\n");
            /* �豸��������ڴ˴����ʵ�� */
            break;
        case HILINK_REVOKE_FLAG_SET:
            printf("�豸��λ�����λ\n");
            /* �豸��λ�����λ�����ڴ˴����ʵ�� */
            break;
        case HILINK_NEGO_REG_INFO_FAIL:
            printf("�豸Э��������Ϣʧ��\n");
            /* �豸Э��������Ϣʧ�� */
            break;
        default:
            break;
    }

    return;
}

/*
 * ʵ��ģ������ǰ���豸����
 * flagΪ0��ʾHiLink SDK �߳̿��Ź�����ģ������; Ϊ1��ʾAPPɾ���豸����ģ������
 * ����0��ʾ����ɹ�, ϵͳ����������ʹ��Ӳ����; ����1��ʾ����ɹ�, ϵͳ����������ʹ��������;
 * ���ظ�ֵ��ʾ����ʧ��, ϵͳ��������
 * ע�⣬�˺������豸����ʵ�֣���APPɾ���豸����ģ������ʱ���豸��������ط���0������ᵼ��ɾ���豸�쳣
 */
int hilink_process_before_restart(int flag)
{
    /* HiLink SDK�߳̿��Ź���ʱ����ģ������ */
    if (flag == HILINK_REBOOT_WATCHDOG) {
        /* ʵ��ģ������ǰ�Ĳ���(��:����ϵͳ״̬��) */
        return -1;
    }

    /* APPɾ���豸����ģ������ */
    if (flag == HILINK_REBOOT_DEVDELETE) {
        /* ʵ��ģ������ǰ�Ĳ���(��:����ϵͳ״̬��) */
        return 0;
    }

    return -1;
}

/*
 * ��ȡ�豸�����룬��֪ͨAPP
 * status��ʾ�Ƿ��͹��ϣ�0��ʾ�����ͣ�1��ʾ���ͣ�code��ʾ������
 * ����0��ʾ�ɹ������ط�0ʧ��
 */
int get_faultDetection_state(int *status, int *code)
{
    /* ���豸����ʵ�֣�������faultDetection���Ե�ǰֵ������� */
    return 0;
}
