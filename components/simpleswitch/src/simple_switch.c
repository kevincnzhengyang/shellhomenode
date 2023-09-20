/*
 * @Author      : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @Date        : 2023-09-20 22:05:13
 * @LastEditors : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @LastEditTime: 2023-09-20 22:43:20
 * @FilePath    : /shellhomenode/components/simpleswitch/src/simple_switch.c
 * @Description : Simple Switch
 * Copyright (c) 2023 by Zheng, Yang, All Rights Reserved.
 */

#include "simple_switch.h"

static const char *SS_TAG = "Simple Switch";

static int *g_ss_gpio_tab = NULL;

static int g_ss_num = 0;


static int simple_switch_handle(const cJSON *cmd_json, const cJSON *rsp_json, void *arg)
{
    if (NULL == cmd_json || NULL == rsp_json) return 1;

    const cJSON *json_value = cJSON_GetObjectItemCaseSensitive(cmd_json, "value");
    if (!cJSON_IsBool(json_value)) {
        ESP_LOGE(SS_TAG, "JSON failed to get boolean element [value]\n");
        return 2;
    }

    cJSON *json_body = cJSON_AddObjectToObject(rsp_json, "body");
    if (NULL == cJSON_AddStringToObject(json_body, "entry", "switch")) {
        ESP_LOGE(SS_TAG, "Failed to encode body [entry]\n");
        return 3;
    }
    cJSON *json_result = cJSON_AddObjectToObject(json_body, "result");

    if (cJSON_IsTrue(json_value)) {
        ESP_LOGI(SS_TAG, "Simple Switch On\n");
        if (NULL == cJSON_AddBoolToObject(json_result, "value", true)) {
            ESP_LOGE(SS_TAG, "Failed to encode body [node]\n");
            return 3;
        }
    } else {
        ESP_LOGI(SS_TAG, "Simple Switch Off\n");
        if (NULL == cJSON_AddBoolToObject(json_result, "value", false)) {
            ESP_LOGE(SS_TAG, "Failed to encode body [node]\n");
            return 3;
        }
    }
    return 0;
}

/***
 * @description : init simple switch
 * @return       {*}
 */
esp_err_t register_simple_switch(void)
{
    return register_entry("switch", simple_switch_handle);
}