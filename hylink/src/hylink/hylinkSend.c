#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "cJSON.h"
#include "logFunc.h"
#include "commonFunc.h"
#include "frameCb.h"
#include "hylink.h"
#include "hylinkRecv.h"

#include "hylinkSend.h"

int hylinkDispatch(const char *str)
{
    char *dispatchBuf = (char *)getHyDispatchBuf();
    int jsonLen = strlen(str);
    dispatchBuf[0] = 0x02;
    strncpy(&dispatchBuf[1], str, jsonLen);
    dispatchBuf[jsonLen + 1] = 0x03;

    return runTransferCb(dispatchBuf, jsonLen + 2, TRANSFER_CLIENT_WRITE);
}
int hylinkSend(void *ptr)
{
    if (NULL == ptr)
    {
        return -1;
    }
    HylinkDevSendData *hylinkDevSendData = (HylinkDevSendData *)ptr;

    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, STR_COMMAND, STR_DISPATCH);

    cJSON_AddStringToObject(root, STR_FRAMENUMBER, "0");

    cJSON_AddStringToObject(root, STR_TYPE, hylinkDevSendData->Type);

    cJSON *DataArray = cJSON_CreateArray();
    cJSON_AddItemToObject(root, STR_DATA, DataArray);

    cJSON *arrayItem = cJSON_CreateObject();
    if (strlen(hylinkDevSendData->Data.DeviceId))
        cJSON_AddStringToObject(arrayItem, STR_DEVICEID, hylinkDevSendData->Data.DeviceId);
    if (strlen(hylinkDevSendData->Data.ModelId))
        cJSON_AddStringToObject(arrayItem, STR_MODELID, hylinkDevSendData->Data.ModelId);
    if (strlen(hylinkDevSendData->Data.Key))
        cJSON_AddStringToObject(arrayItem, STR_KEY, hylinkDevSendData->Data.Key);
    if (strlen(hylinkDevSendData->Data.Value))
        cJSON_AddStringToObject(arrayItem, STR_VALUE, hylinkDevSendData->Data.Value);
    cJSON_AddItemToArray(DataArray, arrayItem);

    char *json = cJSON_PrintUnformatted(root);
    logInfo("send json:%s\n", json);

    int ret = hylinkDispatch(json);

    free(json);
    cJSON_Delete(root);
    return ret;
}

int hylinkHeart(void)
{
    return runTransferCb(HY_HEART, strlen(HY_HEART), TRANSFER_CLIENT_WRITE);
}

int hylinkSendDevInfo(void)
{
    HylinkDevSendData hylinkDevSendData = {0};
    hylinkDevSendData.FrameNumber = 0;
    strcpy(hylinkDevSendData.Type, STR_DEVSINFO);
    strcpy(hylinkDevSendData.Data.DeviceId, STR_GATEWAY_DEVID);
    strcpy(hylinkDevSendData.Data.Key, STR_DEVSINFO);
    return hylinkSend(&hylinkDevSendData);
}

int hylinkSendDevAttr(void *devId, unsigned int len)
{
    HylinkDevSendData hylinkDevSendData = {0};
    hylinkDevSendData.FrameNumber = 0;
    strcpy(hylinkDevSendData.Type, STR_DEVATTRI);
    strcpy(hylinkDevSendData.Data.DeviceId, devId);
    strcpy(hylinkDevSendData.Data.Key, "All");
    return hylinkSend(&hylinkDevSendData);
}

int hylinkSendReset(void)
{
    HylinkDevSendData hylinkDevSendData = {0};
    hylinkDevSendData.FrameNumber = 0;
    strcpy(hylinkDevSendData.Type, STR_REFACTORY);
    return hylinkSend(&hylinkDevSendData);
}

int hylinkDelDev(const char *sn)
{
    if (sn == NULL)
        return -1;
    HylinkDevSendData out = {0};
    out.FrameNumber = 0;
    strcpy(out.Type, STR_DELETE);
    strcpy(out.Data.DeviceId, sn);
    int ret = hylinkSend(&out);
    if (ret >= 0)
    {
        hylinkListDelById(sn);
    }

    return ret;
}
