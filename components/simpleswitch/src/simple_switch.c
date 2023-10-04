/*
 * @Author      : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @Date        : 2023-09-20 22:05:13
 * @LastEditors : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @LastEditTime: 2023-10-04 22:34:39
 * @FilePath    : /shellhomenode/components/simpleswitch/src/simple_switch.c
 * @Description : Simple Switch
 * Copyright (c) 2023 by Zheng, Yang, All Rights Reserved.
 */

#include "driver/gpio.h"

#include "simple_switch.h"

#ifdef CONFIG_NODE_USING_RELAY
static const char *SS_TAG = "simple_switch";

#define SS_MAX_RELAY_NUM        4

static int g_ss_gpio_tab[SS_MAX_RELAY_NUM];

static int simple_switch_handle(const cJSON *cmd_json, const cJSON *rsp_json, void *arg)
{
    if (NULL == cmd_json || NULL == rsp_json) return SS_ENTRY_INVALID_PARAM;

    // get slots
    const cJSON *json_slot = NULL;
    const cJSON *json_slots = cJSON_GetObjectItemCaseSensitive(cmd_json, "slots");
    if (NULL == json_slots) {
        ESP_LOGE(SS_TAG, "JSON failed to get array [slots]");
        return SS_ENTRY_MISSING_SLOTS;
    }

    cJSON_ArrayForEach(json_slot, json_slots)
    {
        // get and check index
        cJSON *json_index = cJSON_GetObjectItemCaseSensitive(json_slot, "index");
        if (!cJSON_IsNumber(json_index)) {
            ESP_LOGE(SS_TAG, "JSON failed to get integer element [index]");
            return SS_ENTRY_MISSING_INDEX;
        }
        if (0 > json_index->valueint || SS_MAX_RELAY_NUM <= json_index->valueint) {
            ESP_LOGE(SS_TAG, "invalid index [%d]", json_index->valueint);
            return SS_ENTRY_INVALID_INDEX;
        }
        if (-1 == g_ss_gpio_tab[json_index->valueint]) {
            ESP_LOGE(SS_TAG, "not configed index [%d]", json_index->valueint);
            return SS_ENTRY_NOT_CONFIGED;
        }

        // get value and set gpio level
        const cJSON *json_value = cJSON_GetObjectItemCaseSensitive(json_slot, "value");
        if (!cJSON_IsBool(json_value)) {
            ESP_LOGE(SS_TAG, "JSON failed to get boolean element [value]");
            return SS_ENTRY_MISSING_VALUE;
        }
        if (cJSON_IsTrue(json_value)) {
            gpio_set_level(g_ss_gpio_tab[json_index->valueint], 1);
            ESP_LOGI(SS_TAG, "relay [%d] On", json_index->valueint);
        } else {
            gpio_set_level(g_ss_gpio_tab[json_index->valueint], 0);
            ESP_LOGI(SS_TAG, "relay [%d] Off", json_index->valueint);
        }
    }

    // prepare result
    cJSON *json_body = cJSON_AddObjectToObject((cJSON *)rsp_json, "body");
    if (NULL == cJSON_AddStringToObject(json_body, "entry", "switch")) {
        ESP_LOGE(SS_TAG, "Failed to encode body [entry]");
        return SS_ENTRY_RESULT_ERROR;
    }
    cJSON *json_result = cJSON_AddObjectToObject(json_body, "result");
    cJSON *json_result_slots = cJSON_AddArrayToObject(json_result, "slots");
    if (json_result_slots == NULL)
    {
        ESP_LOGE(SS_TAG, "Failed to encode slots [entry]");
        return SS_ENTRY_RESULT_ERROR;
    }

    // query all gpio and report
    for (int i = 0; i < SS_MAX_RELAY_NUM; i++) {
        if (-1 == g_ss_gpio_tab[i]) continue;
        cJSON *slot = cJSON_CreateObject();
        if (cJSON_AddNumberToObject(slot, "index", i) == NULL) {
            ESP_LOGE(SS_TAG, "Failed to encode slot index [%d]", i);
            return SS_ENTRY_RESULT_ERROR;
        }
        if (cJSON_AddBoolToObject(slot, "value",
                gpio_get_level(g_ss_gpio_tab[i])) == NULL) {
            ESP_LOGE(SS_TAG, "Failed to encode slot value [%d]", i);
            return SS_ENTRY_RESULT_ERROR;
        }
        ESP_LOGI(SS_TAG, "index [%d] level [%d]", i, gpio_get_level(g_ss_gpio_tab[i]));
        cJSON_AddItemToArray(json_result_slots, slot);
    }

    return SS_ENTRY_OK;
}
#endif /* CONFIG_NODE_USING_RELAY */

/***
 * @description : init simple switch
 * @return       {*}
 */
esp_err_t register_simple_switch(void)
{
#ifdef CONFIG_NODE_USING_RELAY
    for (int i = 0; i < SS_MAX_RELAY_NUM; i++) {
        g_ss_gpio_tab[i] = -1;
    }
    // init all gpio for simple switch
    uint64_t pin_mask = 0;
    #ifdef CONFIG_USING_RELAY_1
        pin_mask |= (1ULL<<CONFIG_RELAY1_GPIO_NUM);
        g_ss_gpio_tab[0] = CONFIG_RELAY1_GPIO_NUM;
    #endif /* CONFIG_USING_RELAY_1 */
    #ifdef CONFIG_USING_RELAY_2
        pin_mask |= (1ULL<<CONFIG_RELAY2_GPIO_NUM);
        g_ss_gpio_tab[1] = CONFIG_RELAY2_GPIO_NUM;
    #endif /* CONFIG_USING_RELAY_2 */
    #ifdef CONFIG_USING_RELAY_3
        pin_mask |= (1ULL<<CONFIG_RELAY3_GPIO_NUM);
        g_ss_gpio_tab[2] = CONFIG_RELAY3_GPIO_NUM;
    #endif /* CONFIG_USING_RELAY_3 */
    #ifdef CONFIG_USING_RELAY_4
        pin_mask |= (1ULL<<CONFIG_RELAY4_GPIO_NUM);
        g_ss_gpio_tab[3] = CONFIG_RELAY4_GPIO_NUM;
    #endif /* CONFIG_USING_RELAY_4 */
    if (0 == pin_mask) {
        ESP_LOGE(SS_TAG, "No GPIO config for relay");
        return ESP_FAIL;
    }

    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,     //disable interrupt
        .mode = GPIO_MODE_INPUT_OUTPUT,     //set as input & output mode since we'll report level
        .pin_bit_mask = pin_mask,           //bit mask of the pins
        .pull_down_en = 0,                  //disable pull-down mode
        .pull_up_en = 0                     //disable pull-up mode
    };
    gpio_config(&io_conf);       //configure GPIO

    return register_entry("switch", simple_switch_handle);
#else
    return ESP_OK;
#endif /* CONFIG_NODE_USING_RELAY */

}