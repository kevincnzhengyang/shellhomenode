/*
 * @Author      : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @Date        : 2023-09-08 16:16:12
 * @LastEditors : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @LastEditTime: 2023-09-19 22:28:10
 * @FilePath    : /shellhomenode/main/main.c
 * @Description : dummy node for shell home
 * Copyright (c) 2023 by Zheng, Yang, All Rights Reserved.
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <freertos/event_groups.h>

#include "cJSON.h"

#include <nvs_flash.h>
#include <esp_event.h>

#include "blufi_prov.h"
#include "shn_network.h"

/* declaration of event group for the node */
EventGroupHandle_t node_event_group;

static const char *ENT_TAG = "switch";

static int simple_switch_handle(const cJSON *cmd_json, const cJSON *rsp_json, void *arg)
{
    if (NULL == cmd_json || NULL == rsp_json) return 1;

    const cJSON *json_value = cJSON_GetObjectItemCaseSensitive(cmd_json, "value");
    if (!cJSON_IsBool(json_value)) {
        ESP_LOGE(ENT_TAG, "JSON failed to get boolean element [value]\n");
        return 2;
    }

    cJSON *json_body = cJSON_AddObjectToObject(rsp_json, "body");
    if (NULL == cJSON_AddStringToObject(json_body, "entry", "switch")) {
        ESP_LOGE(ENT_TAG, "Failed to encode body [entry]\n");
        return 3;
    }
    cJSON *json_result = cJSON_AddObjectToObject(json_body, "result");

    if (cJSON_IsTrue(json_value)) {
        ESP_LOGI(ENT_TAG, "Simple Switch On\n");
        if (NULL == cJSON_AddBoolToObject(json_result, "value", true)) {
            ESP_LOGE(ENT_TAG, "Failed to encode body [node]\n");
            return 3;
        }
    } else {
        ESP_LOGI(ENT_TAG, "Simple Switch Off\n");
        if (NULL == cJSON_AddBoolToObject(json_result, "value", false)) {
            ESP_LOGE(ENT_TAG, "Failed to encode body [node]\n");
            return 3;
        }
    }
    return 0;
}

static shn_sap_table sap_table[] = {{"switch", simple_switch_handle}};
static shn_proto_config proto_config = {sap_table, 1};

void app_main(void)
{
    /* Initialize NVS partition */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        /* NVS partition was truncated
         * and needs to be erased */
        ESP_ERROR_CHECK(nvs_flash_erase());

        /* Retry nvs_flash_init */
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    /* Initialize the event loop */
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* Initialize the wifi */
    ESP_ERROR_CHECK(init_wifi());

    /* Initialize the node event group */
    node_event_group = xEventGroupCreate();

    /* Enable reset button */
    ESP_ERROR_CHECK(en_btn_network_reset());

    /* network provisioning if in need*/
    ESP_ERROR_CHECK(network_provision(false));

    // ESP_ERROR_CHECK(en_btn_noise_detect());

    /* Initialize protocol */
    ESP_ERROR_CHECK(init_shn_proto(NULL));

    /* Launch protocol */
    ESP_ERROR_CHECK(launch_shn_proto(&proto_config));

    int i = 0;
    while (1) {
        printf("[%d] Hello world!\n", i);
        i++;
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

