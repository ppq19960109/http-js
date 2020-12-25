#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "logFunc.h"
#include "frameCb.h"
#include "cJSON.h"
#include "base64.h"

#include "scene.h"
#include "hylinkSend.h"
#include "hylinkRecv.h"
#include "cloudLinkReport.h"
#include "cloudLinkCtrl.h"
#include "cloudLink.h"
#include "cloudLinkListFunc.h"

#include "linkkit_subdev.h"

enum week
{
    Mon,
    Tue,
    Wed,
    Thu,
    Fri,
    Sat,
    Sun
};

static char *weekDay[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

int sceneReport(void *req, unsigned int len)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "SceneId", req);

    char *json = cJSON_PrintUnformatted(root);
    logInfo("cJSON_free(json); send json:%s\n", json);
    cJSON_Delete(root);

    CloudLinkDev *cloudLinkDev = cloudLinkListGetById(STR_GATEWAY_DEVID);
    if (cloudLinkDev == NULL)
    {
        logError("gw cloudLinkDev is NULL");
        goto fail;
    }
    user_post_event(cloudLinkDev->id, "ReportScene", json);
    cJSON_free(json);
fail:
    cJSON_free(json);
    return -1;
}

static int sceneHyDispatch(cJSON *DataArray)
{
    cJSON *dispatch = cJSON_CreateObject();
    cJSON_AddStringToObject(dispatch, STR_COMMAND, STR_DISPATCH);
    cJSON_AddStringToObject(dispatch, STR_FRAMENUMBER, "00");
    cJSON_AddStringToObject(dispatch, STR_TYPE, "LocalScene");

    cJSON_AddItemToObject(dispatch, STR_DATA, DataArray);

    char *json = cJSON_PrintUnformatted(dispatch);
    logInfo("scene send json:%s\n", json);

    hylinkDispatch(json);
    cJSON_free(json);
    cJSON_Delete(dispatch);
    return 0;
}

int getSceneDevStatus(cJSON *params, cJSON *arrayItem)
{
    cJSON *deviceName, *propertyName, *propertyValue;
    deviceName = cJSON_GetObjectItem(params, "deviceName");
    propertyName = cJSON_GetObjectItem(params, "propertyName");
    propertyValue = cJSON_GetObjectItem(params, "propertyValue");
    CloudLinkDev *cloudLinkDev = cloudLinkListGetById(deviceName->valuestring);
    if (cloudLinkDev == NULL)
        return -1;
    int j;
    for (j = 0; j < cloudLinkDev->attrLen; ++j)
    {
        if (strcmp(propertyName->valuestring, cloudLinkDev->attr[j].cloudKey) == 0)
        {
            break;
        }
    }
    if (j == cloudLinkDev->attrLen)
        return -1;
    //-------------------------------
    cJSON_AddStringToObject(arrayItem, "KeyCoding", "Original");

    char strBuf[8];
    cJSON_AddStringToObject(arrayItem, "ActionId", "0");
    cJSON_AddStringToObject(arrayItem, "DevId", deviceName->valuestring);
    cJSON_AddStringToObject(arrayItem, "Key", cloudLinkDev->attr[j].hyKey);
    getValueForJson(propertyValue, strBuf);
    cJSON_AddStringToObject(arrayItem, "Value", strBuf);
    return 0;
}
int addtriggers(const char *localSceneId, cJSON *triggersItems)
{
    int triggersItemsSize = cJSON_GetArraySize(triggersItems);
    if (triggersItemsSize == 0)
        return 0;
    cJSON *triggersArray = cJSON_CreateArray();
    cJSON *triggersItemsSub;
    for (int i = 0; i < triggersItemsSize; ++i)
    {
        triggersItemsSub = cJSON_GetArrayItem(triggersItems, i);
        if (triggersItemsSub == NULL)
            continue;

        cJSON *arrayItem = cJSON_CreateObject();
        cJSON_AddItemToArray(triggersArray, arrayItem);
        cJSON_AddStringToObject(arrayItem, "Op", "AddCond");
        cJSON_AddStringToObject(arrayItem, "Id", localSceneId);

        cJSON_AddStringToObject(arrayItem, "Logic", "Or");

        cJSON *params = cJSON_GetObjectItem(triggersItemsSub, "params");
        if (cJSON_HasObjectItem(params, "cron"))
        {
            cJSON_AddStringToObject(arrayItem, "CondType", "Time");

            cJSON *cron = cJSON_GetObjectItem(params, "cron");

            char *ptimeStr = strdup(cron->valuestring);
            logWarn("ptimeStr addr %d", ptimeStr);
            char *token, *timeStr = ptimeStr;
            const char delim[] = " ,";
            token = strsep(&timeStr, delim);

            cJSON_AddStringToObject(arrayItem, "StartMinu", token);
            token = strsep(&timeStr, delim);
            cJSON_AddStringToObject(arrayItem, "StartHour", token);
            token = strsep(&timeStr, delim);
            token = strsep(&timeStr, delim);

            if (strcmp("*", token) == 0)
                cJSON_AddStringToObject(arrayItem, "Repeat", "1");
            else
                cJSON_AddStringToObject(arrayItem, "Repeat", "0");

            char weekBuf[33] = {0};
            while ((token = strsep(&timeStr, delim)) != NULL)
            {
                int day = atoi(token);
                if (strlen(weekBuf) != 0)
                    strcat(weekBuf, ",");
                strcat(weekBuf, weekDay[day]);
            }
            cJSON_AddStringToObject(arrayItem, "Week", weekBuf);
            logWarn("ptimeStr addr %d", timeStr);
            free(ptimeStr);
        }
        else if (cJSON_HasObjectItem(params, "deviceName"))
        {
            cJSON_AddStringToObject(arrayItem, "CondType", "Event");
            cJSON_AddStringToObject(arrayItem, "Active", "Equ");
            getSceneDevStatus(params, arrayItem);
        }
        else
        {
            /* code */
        }
    }
    return sceneHyDispatch(triggersArray);
}

int addconditions(const char *localSceneId, cJSON *conditionsItems)
{
    int conditionsItemsSize = cJSON_GetArraySize(conditionsItems);
    if (conditionsItemsSize == 0)
        return 0;
    cJSON *conditionsArray = cJSON_CreateArray();

    cJSON *conditionsItemsSub;
    for (int i = 0; i < conditionsItemsSize; ++i)
    {
        conditionsItemsSub = cJSON_GetArrayItem(conditionsItems, i);
        if (conditionsItemsSub == NULL)
            continue;
        cJSON *arrayItem = cJSON_CreateObject();
        cJSON_AddItemToArray(conditionsArray, arrayItem);
        cJSON_AddStringToObject(arrayItem, "Op", "AddCond");
        cJSON_AddStringToObject(arrayItem, "Id", localSceneId);

        cJSON_AddStringToObject(arrayItem, "Logic", "And");

        cJSON *params = cJSON_GetObjectItem(conditionsItemsSub, "params");
        if (cJSON_HasObjectItem(params, "startTime"))
        {
            cJSON_AddStringToObject(arrayItem, "CondType", "Time");

            cJSON *startTime = cJSON_GetObjectItem(params, "startTime");
            cJSON *endTime = cJSON_GetObjectItem(params, "endTime");

            const char delim[] = ":";

            char *ptimeStr = strdup(startTime->valuestring);
            char *token, *timeStr = ptimeStr;
            token = strsep(&timeStr, delim);
            cJSON_AddStringToObject(arrayItem, "StartHour", token);
            token = strsep(&timeStr, delim);
            cJSON_AddStringToObject(arrayItem, "StartMinu", token);
            free(ptimeStr);

            ptimeStr = strdup(endTime->valuestring);
            timeStr = ptimeStr;
            token = strsep(&timeStr, delim);
            cJSON_AddStringToObject(arrayItem, "EndHour", token);
            token = strsep(&timeStr, delim);
            cJSON_AddStringToObject(arrayItem, "EndMinu", token);
            free(ptimeStr);
        }
        else if (cJSON_HasObjectItem(params, "deviceName"))
        {
            cJSON_AddStringToObject(arrayItem, "CondType", "Event");
            cJSON_AddStringToObject(arrayItem, "Active", "Equ");
            getSceneDevStatus(params, arrayItem);
        }
        else
        {
            /* code */
        }
    }
    return sceneHyDispatch(conditionsArray);
}
int addActions(const char *localSceneId, cJSON *actions)
{
    int actionsSize = cJSON_GetArraySize(actions);
    if (actionsSize == 0)
        return 0;
    int dispatchNum = 0, i = 0;
    do
    {
        if (actionsSize > dispatchNum + 10)
        {
            dispatchNum += 10;
        }
        else
        {
            dispatchNum = actionsSize;
        }

        cJSON *actionsArray = cJSON_CreateArray();

        cJSON *actionsSub;
        for (; i < dispatchNum; ++i)
        {
            actionsSub = cJSON_GetArrayItem(actions, i);
            if (actionsSub == NULL)
                continue;
            cJSON *arrayItem = cJSON_CreateObject();
            cJSON_AddItemToArray(actionsArray, arrayItem);
            cJSON_AddStringToObject(arrayItem, "Op", "AddAction");
            cJSON_AddStringToObject(arrayItem, "Id", localSceneId);
            //--------------------------
            cJSON *params = cJSON_GetObjectItem(actionsSub, "params");
            if (cJSON_HasObjectItem(params, "sceneId"))
            {
                cJSON *sceneId = cJSON_GetObjectItem(params, "sceneId");
                //-------------------------------
                cJSON_AddStringToObject(arrayItem, "KeyCoding", "Original");

                cJSON_AddStringToObject(arrayItem, "ActionId", sceneId->valuestring);
                cJSON_AddStringToObject(arrayItem, "DevId", STR_GATEWAY_DEVID);
                cJSON_AddStringToObject(arrayItem, "Key", "SceneId");
                cJSON_AddStringToObject(arrayItem, "Value", sceneId->valuestring);
            }
            else
            {
                getSceneDevStatus(params, arrayItem);
            }
        }

        sceneHyDispatch(actionsArray);
    } while (dispatchNum < actionsSize);

    return 0;
fail:
    return -1;
}

int addScene(cJSON *rules, const int isUpdate)
{
    int i, delayTime = 0;

    cJSON *ruleId = cJSON_GetObjectItem(rules, "ruleId");
    cJSON *enable = cJSON_GetObjectItem(rules, "enable");

    //---------------------------------
    cJSON *additionsSub, *params;
    cJSON *additions = cJSON_GetObjectItem(rules, "additions");
    int additionsSize = cJSON_GetArraySize(additions);
    if (additionsSize)
    {
        additionsSub = cJSON_GetArrayItem(additions, 0);
        params = cJSON_GetObjectItem(additionsSub, "params");
        if (params != NULL)
        {
            delayTime = cJSON_GetObjectItem(params, "delayTime")->valueint;
        }
    }
    //-----------------------------------
    if (isUpdate == 0)
    {
        cJSON *updateArray = cJSON_CreateArray();
        cJSON *arrayItem = cJSON_CreateObject();
        cJSON_AddItemToArray(updateArray, arrayItem);

        cJSON_AddStringToObject(arrayItem, "Op", "AddScene");
        char buf[8] = {0};
        cJSON_AddStringToObject(arrayItem, "Id", ruleId->valuestring);
        sprintf(buf, "%d", enable->valueint);
        cJSON_AddStringToObject(arrayItem, "Enable", buf);
        sprintf(buf, "%d", delayTime);
        cJSON_AddStringToObject(arrayItem, "ExecDelayed", buf);
        sceneHyDispatch(updateArray);
    }
    //---------------------------------
    cJSON *triggers = cJSON_GetObjectItem(rules, "triggers");
    cJSON *triggersItems = cJSON_GetObjectItem(triggers, "items");
    addtriggers(ruleId->valuestring, triggersItems);

    //---------------------------------
    cJSON *conditions = cJSON_GetObjectItem(rules, "conditions");
    cJSON *conditionsItems = cJSON_GetObjectItem(conditions, "items");
    addconditions(ruleId->valuestring, conditionsItems);

    //---------------------------------
    cJSON *actions = cJSON_GetObjectItem(rules, "actions");
    addActions(ruleId->valuestring, actions);

    return 0;
fail:
    return -1;
}

int delScene(cJSON *ruleIds)
{
    int ruleIdsSize = cJSON_GetArraySize(ruleIds);
    if (ruleIdsSize == 0)
        return 0;
    cJSON *delArray = cJSON_CreateArray();

    cJSON *ruleIdsSub;
    for (int i = 0; i < ruleIdsSize; ++i)
    {
        ruleIdsSub = cJSON_GetArrayItem(ruleIds, i);
        if (ruleIdsSub == NULL)
            continue;
        cJSON *arrayItem = cJSON_CreateObject();
        cJSON_AddItemToArray(delArray, arrayItem);
        cJSON_AddStringToObject(arrayItem, "Op", "DelScene");
        cJSON_AddStringToObject(arrayItem, "Id", ruleIdsSub->valuestring);
    }
    return sceneHyDispatch(delArray);
}

int sceneDispatch(const char *str, char **response, int *response_len)
{
    int i, res;
    /* Parse Root */
    cJSON *root = cJSON_Parse(str);
    if (root == NULL || !cJSON_IsObject(root))
    {
        logError("JSON Parse Error");
        return -1;
    }
    // char *rootjson = cJSON_PrintUnformatted(root);
    // logInfo("rootjson:%s\n", rootjson);
    // cJSON_free(rootjson);

    cJSON *operationType = cJSON_GetObjectItem(root, "operationType");
    if (operationType == NULL)
    {
        logError("operationType is NULL\n");
        goto fail;
    }

    cJSON *sceneId = cJSON_GetObjectItem(root, "sceneId");
    if (sceneId == NULL)
    {
        logError("sceneId is NULL\n");
        goto fail;
    }

    //-----------------------------------
    cJSON *rules = cJSON_GetObjectItem(root, "rules");
    if (rules != NULL)
    {
        int rulesSize = cJSON_GetArraySize(rules);
        cJSON *rulesSub;
        for (int i = 0; i < rulesSize; ++i)
        {
            rulesSub = cJSON_GetArrayItem(rules, i);
            if (rulesSub == NULL)
                continue;
            if (strcmp("createScene", operationType->valuestring) == 0)
            {
                res = addScene(rulesSub, 0);
            }
            else if (strcmp("updateScene", operationType->valuestring) == 0)
            {
                res = addScene(rulesSub, 1);
            }
            else
            {
                goto fail;
            }
        }
    }
    cJSON *ruleIds = cJSON_GetObjectItem(root, "ruleIds");
    if (ruleIds != NULL)
    {
        if (strcmp("deleteScene", operationType->valuestring) == 0)
        {
            res = delScene(ruleIds);
        }
    }
    //-----------------------------------
    cJSON *rsp = cJSON_CreateObject();
    cJSON_AddNumberToObject(rsp, "code", 200);
    cJSON_AddStringToObject(rsp, "data", sceneId->valuestring);
    cJSON_AddStringToObject(rsp, "message", "successed");
    char *rspJson = cJSON_PrintUnformatted(rsp);
    cJSON_Delete(rsp);
    //-----------------------------------
    cJSON_Delete(root);
    //-----------------------------------
    /* Send Service Response To Cloud */
    *response_len = strlen(rspJson) + 1;
    *response = (char *)malloc(*response_len);
    if (*response == NULL)
    {
        logInfo("Memory Not Enough");
        return -1;
    }
    memset(*response, 0, *response_len);
    strncpy(*response, rspJson, *response_len);
    logInfo("scene response:%s\n", *response);

    cJSON_free(rspJson);
    //-----------------------------------
    return 0;

fail:
    cJSON_Delete(root);
    return -1;
}
int localScene(const int devid, const char *serviceid, const int serviceid_len, const char *request, char **response, int *response_len)
{
    int i;
    /* Parse Root */
    cJSON *root = cJSON_Parse(request);
    if (root == NULL || !cJSON_IsObject(root))
    {
        logError("JSON Parse Error");
        return -1;
    }
    //Type字段
    cJSON *Type = cJSON_GetObjectItem(root, STR_TYPE);
    if (Type == NULL)
    {
        logError("Type is NULL\n");
        goto fail;
    }
    cJSON *sceneRequest;
    char buf[24] = {0};
    for (i = 1; i <= 9; ++i)
    {
        sprintf(buf, "Request_%d", i);
        sceneRequest = cJSON_GetObjectItem(root, buf);
        if (sceneRequest == NULL)
        {
            logError("sceneRequest is NULL\n");
            continue;
        }
        int encLen = strlen(sceneRequest->valuestring);
        if (encLen != 0)
        {
            void *decodeOut = malloc(BASE64_DECODE_OUT_SIZE(encLen) + 1);
            memset(decodeOut, 0, BASE64_DECODE_OUT_SIZE(encLen) + 1);
            int decodeOutlen = base64_decode(sceneRequest->valuestring, encLen, decodeOut);
            logDebug("sceneRequest:%s", decodeOut);
            sceneDispatch(decodeOut, response, response_len);
            free(decodeOut);
        }
    }
    cJSON_Delete(root);

    return 0;

fail:
    cJSON_Delete(root);
    return -1;
}