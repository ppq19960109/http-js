/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: HiLink SDK����ͷ�ļ�
 * Create: 2019-04-20
 * Notes: ���ļ��еĽӿ���Ҫ�����ṩ������������ʹ�ã�Ϊ��ǰ����ݣ������Ͻӿ��ݲ������±���淶����.
 */
#ifndef __HILINK_SDK_ADAPTER_H__
#define __HILINK_SDK_ADAPTER_H__

#ifdef __cplusplus
extern "C" {
#endif

/* HiLink SDK ֪ͨ����ģ������ԭ�� */
#define HILINK_REBOOT_WATCHDOG  0
#define HILINK_REBOOT_DEVDELETE 1

/* �豸���ƶ����ӶϿ�(�汾��ǰ����) */
#define HILINK_M2M_CLOUD_OFFLINE        0
/* �豸�����ƶ˳ɹ���������������̬(�汾��ǰ����) */
#define HILINK_M2M_CLOUD_ONLINE         1
/* �豸���ƶ����ӳ�ʱ��Ͽ�(�汾��ǰ����) */
#define HILINK_M2M_LONG_OFFLINE         2
/* �豸���ƶ����ӳ�ʱ��Ͽ����������(�汾��ǰ����) */
#define HILINK_M2M_LONG_OFFLINE_REBOOT  3
/* HiLink�߳�δ���� */
#define HILINK_UNINITIALIZED            4
/* �豸��������ģʽ */
#define HILINK_LINK_UNDER_AUTO_CONFIG   5
/* �豸����10���ӳ�ʱ״̬ */
#define HILINK_LINK_CONFIG_TIMEOUT      6
/* �豸��������·���� */
#define HILINK_LINK_CONNECTTING_WIFI    7
/* �豸�Ѿ�����·���� */
#define HILINK_LINK_CONNECTED_WIFI      8
/* �豸���������ƶ� */
#define HILINK_M2M_CONNECTTING_CLOUD    9
/* �豸��·���������ӶϿ� */
#define HILINK_M2M_CLOUD_DISCONNECT     10
/* �豸��ע�� */
#define HILINK_DEVICE_REGISTERED        11
/* �豸����� */
#define HILINK_DEVICE_UNREGISTER        12
/* �豸��λ�����λ */
#define HILINK_REVOKE_FLAG_SET          13
/* �豸Э��ע����Ϣʧ�� */
#define HILINK_NEGO_REG_INFO_FAIL       14

/*
 * ֪ͨ�豸��״̬
 * status��ʾ�豸��ǰ��״̬
 * ע�⣬�˺������豸���̸��ݲ�Ʒҵ��ѡ����ʵ��
 */
void hilink_notify_devstatus(int status);

/*
 * ʵ��ģ������ǰ���豸����
 * flagΪ0��ʾHiLink SDK �߳̿��Ź�����ģ������; Ϊ1��ʾAPPɾ���豸����ģ������
 * ����0��ʾ����ɹ�, ϵͳ����������ʹ��Ӳ����; ����1��ʾ����ɹ�, ϵͳ����������ʹ��������;
 * ���ظ�ֵ��ʾ����ʧ��, ϵͳ��������
 * ע�⣬�˺������豸����ʵ�֣���APPɾ���豸����ģ������ʱ���豸��������ط���0������ᵼ��ɾ���豸�쳣
 */
int hilink_process_before_restart(int flag);

/*
 * ��ȡ�豸�����룬��֪ͨAPP
 * status��ʾ�Ƿ��͹��ϣ�0��ʾ�����ͣ�1��ʾ���ͣ�code��ʾ������
 * ����0��ʾ�ɹ������ط�0ʧ��
 */
int get_faultDetection_state(int *status, int *code);

#ifdef __cplusplus
}
#endif
#endif
