/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: HiLink SDK�������ͷ�ļ�
 */
#ifndef HILINK_PROFILE_BRIDGE_H
#define HILINK_PROFILE_BRIDGE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HILINK_M2M_MAX_SVC_NUM
#define HILINK_M2M_MAX_SVC_NUM 20
#endif

#define ST_STR_MAX_LEN  32
#define SVCID_STR_MAX_LEN    64
#ifndef BRG_MAX_SUB_DEV_NUM
#define BRG_MAX_SUB_DEV_NUM  64
#endif

/* Hilink Device SDK ���⿪�Žӿ� */
typedef struct {
    char sn[40];     /* �豸Ψһ��ʶ������sn�ţ����ȷ�Χ��0,40] */
    char prodId[5];  /* �豸HiLink��֤�ţ����ȷ�Χ��0,5] */
    char model[32];  /* �豸�ͺţ����ȷ�Χ��0,32] */
    char devType[4]; /* �豸���ͣ����ȷ�Χ��0,4] */
    char manu[4];    /* �豸�����̣����ȷ�Χ��0,4] */
    char mac[32];    /* �豸MAC��ַ���̶�32�ֽ� */
    char hiv[32];    /* �豸HilinkЭ��汾�����ȷ�Χ��0,32] */
    char fwv[64];    /* �豸�̼��汾�����ȷ�Χ[0,64] */
    char hwv[64];    /* �豸Ӳ���汾�����ȷ�Χ[0,64] */
    char swv[64];    /* �豸����汾�����ȷ�Χ[0,64] */
    int protType;    /* �豸Э�����ͣ�ȡֵ��Χ[1,3] */
} BrgDevInfo;

typedef struct {
    char st[HILINK_M2M_MAX_SVC_NUM][ST_STR_MAX_LEN];
    char svcId[HILINK_M2M_MAX_SVC_NUM][SVCID_STR_MAX_LEN];
} BrgDevSvcInfo;

typedef enum {
    DEV_OFFLINE = 0,    /* �豸���� */
    DEV_ONLINE  = 1,    /* �豸���� */
    DEV_RESTORE = 2,    /* �豸�ָ�������ɾ���ƶ���Ϣ */
    DEV_ADD     = 3     /* �豸�ָ�����֮������ע�� */
} DevOnlineStatus;

typedef struct {
    const char *svcId;    /* ����ȡ�ķ���ID */
    const char *json;     /* Json��ʽ�������ֶΣ�ΪNULLʱ��ȡsvcId�����µ��������� */
    unsigned int jsonLen; /* json��ΪNULLʱ��Json��ʽ�ַ������� */
} GetBrgDevCharState;

/*
 * Bridge�ϱ��¹��豸״̬
 * �ú������豸�����߻��̵���
 * sn: Bridge���¹��豸Ψһ��ʶ
 * status: �¹��豸״̬
 * ע�⣺DEV_RESTORE��DEV_ADDֻ�������Ŵ���ֱ����ģʽ��ʹ��
 */
int HilinkSyncBrgDevStatus(const char *sn, DevOnlineStatus status);

/*
 * ��ȡ�豸���豸��Ϣ
 * �ú������豸�����߻���ʵ��
 * sn: Bridge�¹��豸Ψһ��ʶ
 * devInfo: sdk���ٿռ䣬�ɳ�������豸��Ϣ
 * ����0: �豸��Ϣ��ȡ�ɹ���devInfoָ����豸��Ϣ��ȷ��Ч
 * ���ط�0: �豸��Ϣ��ȡʧ�ܣ�devInfoָ����豸��Ϣ��Ч
 */
int HilinkGetBrgDevInfo(const char *sn, BrgDevInfo *devInfo);

/*
 * ��ȡ�豸�ķ�����Ϣ��
 * �ú������豸�����߻���ʵ��
 * sn: Bridge�¹��豸Ψһ��ʶ��
 * svcInfo: sdk���ٿռ䣬�ɳ������
 * svcNum: �ɳ��̸���ʵ�����ķ����������ֵ
 * ����0: ������Ϣ��ȡ�ɹ���svcInfoָ��ķ�����Ϣ��ȷ��Ч
 * ���ط�0: ������Ϣ��ȡʧ�ܣ�svcInfoָ��ķ�����Ϣ��Ч
 */
int HilinkGetBrgSvcInfo(const char *sn, BrgDevSvcInfo *svcInfo, unsigned int *svcNum);

/*
 * �ϱ������豸����report���Ե�״̬
 * �ú������豸�����߻��̵���
 * ע��: ֻ����report�����������ֶ���ʹ�ô˽ӿ��ϱ��¼���ֻ������hilink_m2m_process��һ�������е���
 * sn: ���¹��豸SN
 * svcId: ���¹��豸����ID
 * payload: Json��ʽ���ֶ�����ֵ
 * len: payload�ĳ���
 * tid: �ӿڵ�������taskid ��ͬm2m taskid, ���ڱ�֤ͬһ�����е���
 * ����0: ����״̬�ϱ��ɹ�
 * ���ط�0: ����״̬�ϱ�ʧ��
 */
int HilinkReportBrgDevCharState(const char *sn, const char *svcId,
    char *payload, unsigned int len, unsigned long tid);

/*
 * �ϱ�Bridge���¹��豸����״̬
 * �ú������豸�����߻��̵���
 * ע��: �˽ӿڷ����ϱ��¼�֪ͨ�������ֶ�״̬��Hilink Device SDKͳһ��ѯ���ϱ��������ʱ200ms
 * sn: Bridge���¹��豸Ψһ��ʶ
 * svcid: Bridge���¹��豸����ID
 * ����0: ����״̬�ϱ��ɹ�
 * ���ط�0: ����״̬�ϱ�ʧ��
 */
int HilinkUploadBrgDevCharState(const char *sn, const char *svcId);

/*
 * ���÷���״̬
 * �ú������豸�����߻���ʵ��
 * sn: Bridge���¹��豸Ψһ��ʶ
 * svcId: ����ID
 * payload: ��Ҫ���õ�Json��ʽ���ֶ�����ֵ
 * len: ���յ���payload�ĳ��ȣ���ΧΪ[0��512)
 * ����M2M_SEARCH_GW_INVALID_PACKET(-101): ��ñ��Ĳ�����Ҫ��
 * ����M2M_SVC_STUTAS_VALUE_MODIFYING(-111): ����״ֵ̬�����޸��У��޸ĳɹ���ײ��豸���������ϱ�
 * ����M2M_NO_ERROR(0): ����״ֵ̬�޸ĳɹ�������Ҫ�ײ��豸�����ϱ�����Hilink Device SDK�ϱ�
 */
int HilinkPutBrgDevCharState(const char *sn, const char *svcId, const char *payload, unsigned int len);

/*
 * ��ȡ����״̬
 * �ú������豸�����߻���ʵ��
 * sn: Bridge���¹��豸Ψһ��ʶ
 * in: ��ȡ��Щ�������Ե�ֵ
 * out: ���ر�������ֶ�ֵ���ݵ�ָ��,�ڴ��ɳ��̿��٣�ʹ����ɺ���Hilink Device SDK�ͷ�
 * outLen: ��ȡ����payload�ĳ��ȣ���ΧΪ[0��512)
 * ����0: ��ȡ����״̬�ɹ�
 * ���ط�0: ��ȡ����״̬ʧ��
 * ע��: out��Ҫ��̬���룬Hilink Device SDKʹ����ɺ�����hilink_free�ӿ��ͷ�
 */
int HilinkGetBrgDevCharState(const char *sn, GetBrgDevCharState *in, char **out, unsigned int *outLen);

/*
 * ɾ��bridge�¹��豸
 * �ú������豸�����߻���ʵ��
 * sn: Bridge���¹��豸Ψһ��ʶ
 */
int HilinkDelBrgDev(const char *sn);

/*
 * bride�¹��豸�ָ�����
 * �ú������豸�����߻��̵���
 * sn: Bridge���¹��豸Ψһ��ʶ
 */
int HilinkResetBrgDev(const char *sn);

/*
 * bride�¹��豸�������֪ͨSDK
 * �ú������豸�����߻��̵���
 * sn��ʾ���¹��豸Ψһ��ʶ��code��ʾ�������
 */
void HiLinkBrgDevOtaFinish(const char *sn, int code);

/*
 * ���豸devinfo���º�ɵ��ô˽ӿ�ͬ��
 * sn��ʾ���¹��豸Ψһ��ʶ��taskId��ʾ�ӿڵ�����������id
 */
int HiLinkSyncBrgDevInfo(const char *sn, unsigned long taskId);

/*
 * �豸�����߻��߳��̵��ô˽ӿ�ʵ�������¹����豸����/���Ե������ϱ�
 * payloadΪjson��ʽ�����ݣ�ע����������ʽ
    [{
      "devId": "2389294",
      "sn": "00E0FC018009",
      "services": [{
          "sid": "light1",
          "data": {
            "characteristicName1": "value1",
            "characteristicName2": "value2"
        }
      }]
    }]
 * ����0��ʾ�ϱ��ɹ���������ʾʧ��
 * ע��:payload������'\0'Ϊ��������������ܵ����쳣
 */
int HILINK_ReportBrgAllSubDevState(const char *payload);

#ifdef __cplusplus
}
#endif

#endif  /* HILINK_PROFILE_BRIDGE_H */
