#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/reboot.h>

#include "logFunc.h"
#include "frameCb.h"
#include "hylink.h"
#include "hylinkRecv.h"
#include "cloudLinkReport.h"
#include "cloudLinkCtrl.h"
#include "cloudLink.h"
#include "cloudLinkListFunc.h"
#include "scene.h"

#include "linkkit_app_gateway.h"
#include "linkkit_subdev.h"

static CloudLinkControl g_CloudLinkControl;

void cloudLinkInit(void)
{
    pthread_mutex_init(&g_CloudLinkControl.mutex, NULL);
    cloudLinkListInit();
}

void cloudLinkDestory(void)
{
    cloudLinkListEmpty();
    cloudLinkListDestroy();
    pthread_mutex_destroy(&g_CloudLinkControl.mutex);
}

//-----------------------------------------------------
int reportGateWayInfo(const char *deviceName, const char *productKey, const int result)
{
    int res;
    cJSON *root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "Command", "Report");
    cJSON_AddStringToObject(root, "FrameNumber", "00");
    cJSON_AddStringToObject(root, "Type", "Register");
    cJSON_AddStringToObject(root, "DeviceNum", "0");
    cJSON *Data = cJSON_AddArrayToObject(root, "Data");
    cJSON *DataList = cJSON_CreateObject();
    cJSON_AddItemToArray(Data, DataList);

    cJSON_AddStringToObject(DataList, "DeviceName", deviceName);
    cJSON_AddStringToObject(DataList, "ProductKey", productKey);
    char buf[4];
    snprintf(buf, sizeof(buf), "%d", result);
    cJSON_AddStringToObject(DataList, "Result", buf);
    char *json = cJSON_PrintUnformatted(root);

    cJSON_Delete(root);

    cJSON *ReportInfo = cJSON_CreateObject();
    cJSON_AddStringToObject(ReportInfo, "ReportGateWayInfo", json);
    char *info = cJSON_PrintUnformatted(ReportInfo);
    // EXAMPLE_TRACE("info:%s", info);
    CloudLinkDev *cloudLinkDev = cloudLinkListGetById(STR_GATEWAY_DEVID);
    if (cloudLinkDev == NULL)
        goto fail;
    user_post_event(cloudLinkDev->id, "GateWayInfo", info);

    cJSON_Delete(ReportInfo);
    free(info);
    free(json);

    return res;
fail:
    cJSON_Delete(ReportInfo);
    free(info);
    free(json);

    return -1;
}
static int cloudSubDevStatus(CloudLinkDev *cloudLinkDev, int status)
{

    int res = -1;
    if (cloudLinkDev == NULL)
        return res;

    HyLinkDev *hylinkDev = hylinkListGetById(cloudLinkDev->alinkInfo.device_name);
    logWarn("cloudSubDevStatus:id:%s,%d,%d\n", cloudLinkDev->alinkInfo.device_name, hylinkDev->online, status);
    if (hylinkDev->online == SUBDEV_OFFLINE && status != SUBDEV_RESTORE)
        return res;

    res = linkkit_subdev_status(&cloudLinkDev->alinkInfo, &cloudLinkDev->id, status);
    if (res < 0)
    {
        logError("linkkit_subdev_status id:%s error:%d\n", cloudLinkDev->alinkInfo.device_name, res);
        return res;
    }

    if (status == SUBDEV_RESTORE)
    {
        cloudLinkListDelDev(cloudLinkDev);
    }
    else if (status == SUBDEV_ONLINE)
    {
        cloudAttrReport(cloudLinkDev, ATTR_REPORT_ALL);
        reportGateWayInfo(cloudLinkDev->alinkInfo.device_name, cloudLinkDev->alinkInfo.product_key, res);
    }
    else
    {
        /* code */
    }
    return 0;
}

static int cloudSubDevLink(char *sn, int status)
{
    pthread_mutex_lock(&g_CloudLinkControl.mutex);
    int res = 0;
    CloudLinkDev *cloudLinkDev;
    if (sn == NULL)
    {

        cloudLink_kh_foreach_value(cloudLinkDev)
        {
            logWarn("cloudLink_kh_foreach_value");
            cloudSubDevStatus(cloudLinkDev, status);
        }
    }
    else
    {
        cloudLinkDev = cloudLinkListGetById(sn);
        if (cloudLinkDev == NULL)
        {
            logError("cloudSubDevLink sn:%s not exist", sn);
            goto fail;
        }

        res = cloudSubDevStatus(cloudLinkDev, status);
    }
    pthread_mutex_unlock(&g_CloudLinkControl.mutex);
    return res;
fail:
    pthread_mutex_unlock(&g_CloudLinkControl.mutex);
    return -1;
}

void cloudLinkClose(void)
{
    // runTransferCb(NULL, SUBDEV_OFFLINE, TRANSFER_SUBDEV_LINE);
    runSystemCb(HYLINK_CLOSE);

    cloudLinkDestory();
    // sleep(2);
}

void cloudLinkReset(void)
{
    runTransferCb(NULL, SUBDEV_RESTORE, TRANSFER_SUBDEV_LINE);
    runSystemCb(HYLINK_RESET);

    system("sh /userdata/app/restore.sh alink &");
}

void cloudLinkMain(void)
{
    registerSystemCb(cloudLinkReset, SYSTEM_RESET);

    registerTransferCb(cloudSubDevLink, TRANSFER_SUBDEV_LINE);
    registerTransferCb(cloudUpdate, TRANSFER_CLOUD_REPORT);
    registerTransferCb(sceneReport, TRANSFER_SCENE_REPORT);
    cloudLinkInit();
}
