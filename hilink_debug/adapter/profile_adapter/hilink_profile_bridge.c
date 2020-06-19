/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: ���ļ��ṩ�˼������Ź������迪����ʵ�ֵĽӿڵ�˵����HiLink SDK������ʱ���ö�Ӧ�ӿ�;
 *              �迪���ߵ��õĽӿ����hilink_profile_bridge.h;
 *              �뿪������ϸ�Ķ��ļ��е�ע��˵�����ο����޸�ʵ�֡�
 * Create: 2019-03-27
 */
#include "hilink_profile_bridge.h"

#ifndef NULL
#define NULL 0
#endif

/*
 * ��ȡ�豸���豸��Ϣ
 * �ú������豸�����߻���ʵ��
 * sn: Bridge�¹��豸Ψһ��ʶ
 * devInfo: sdk���ٿռ䣬�ɳ�������豸��Ϣ
 * ����0: �豸��Ϣ��ȡ�ɹ���devInfoָ����豸��Ϣ��ȷ��Ч
 * ���ط�0: �豸��Ϣ��ȡʧ�ܣ�devInfoָ����豸��Ϣ��Ч
 */
int HilinkGetBrgDevInfo(const char* sn, BrgDevInfo* devInfo)
{
    /* ����ʵ�ִ˽ӿ� */
    if ((sn == NULL) || (devInfo == NULL)) {
        return -1;
    }
    return 0;
}

/*
 * ��ȡ�豸�ķ�����Ϣ��
 * �ú������豸�����߻���ʵ��
 * sn: Bridge�¹��豸Ψһ��ʶ��
 * svcInfo: sdk���ٿռ䣬�ɳ������
 * svcNum: �ɳ��̸���ʵ�����ķ����������ֵ
 * ����0: ������Ϣ��ȡ�ɹ���svcInfoָ��ķ�����Ϣ��ȷ��Ч
 * ���ط�0: ������Ϣ��ȡʧ�ܣ�svcInfoָ��ķ�����Ϣ��Ч
 */
int HilinkGetBrgSvcInfo(const char* sn, BrgDevSvcInfo* svcInfo, unsigned int* svcNum)
{
    /* ����ʵ�ִ˽ӿ� */
    if ((sn == NULL) || (svcInfo == NULL) || (svcNum == NULL)) {
        return -1;
    }
    return 0;
}

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
int HilinkPutBrgDevCharState(const char* sn, const char* svcId, const char* payload, unsigned int len)
{
    /* ����ʵ�ִ˽ӿ� */
    if ((sn == NULL) || (svcId == NULL) || (payload == NULL)) {
        return -1;
    }
    return 0;
}

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
int HilinkGetBrgDevCharState(const char* sn, GetBrgDevCharState* in, char** out, unsigned int* outLen)
{
    /* ����ʵ�ִ˽ӿ� */
    if ((sn == NULL) || (in == NULL) || (out == NULL) || (outLen == NULL)) {
        return -1;
    }
    return 0;
}

/*
 * ɾ��bridge�¹��豸
 * �ú������豸�����߻���ʵ��
 * sn: Bridge���¹��豸Ψһ��ʶ
 */
int HilinkDelBrgDev(const char* sn)
{
    /* ����ʵ�ִ˽ӿ� */
    if (sn == NULL) {
        return -1;
    }
    return 0;
}
