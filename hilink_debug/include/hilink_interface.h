/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: HiLink Interfaceͷ�ļ�
 */
#ifndef HILINK_INTERFACE_H
#define HILINK_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *(*malloc)(unsigned int);
    void (*free)(void *);
} HILINK_MemHooks;

/*
 * ע���ڴ�����ӿڵ�HiLink SDK
 * �������Ϊ�û�ע����ڴ���������ṹ��ָ��
 * ����0�ɹ������ط�0ʧ��
 * ע�⣺malloc��free�ӿڱ���ɶ�ע��
 */
int HILINK_RegisterMemHooks(const HILINK_MemHooks *memHooks);

#ifdef __cplusplus
}
#endif

#endif /* HILINK_INTERFACE_H */