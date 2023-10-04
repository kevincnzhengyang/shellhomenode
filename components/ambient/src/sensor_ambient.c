/*
 * @Author      : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @Date        : 2023-10-02 16:17:17
 * @LastEditors : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @LastEditTime: 2023-10-04 22:30:36
 * @FilePath    : /shellhomenode/components/ambient/src/sensor_ambient.c
 * @Description : ambient light sensor
 * Copyright (c) 2023 by Zheng, Yang, All Rights Reserved.
 */
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "esp_timer.h"
#include <esp_log.h>

#include "sensor_ambient.h"
#include "shn_network.h"

#ifdef CONFIG_NODE_AMBIENT_LIGHT
static const char *SAL_TAG = "ambient";

#define FILTER_WINDOW       (1<<2)

#ifdef CONFIG_AMBIENT_TEMT_6000
#include "ambient_temt6000.h"
#elif CONFIG_AMBIENT_VEML_7700
#include "ambient_veml7700.h"
#else
#error "unknown amblient light sensor type"
#endif

typedef struct {
    Sensor_Ambient_Lisght    *sensor;       // sensor
    esp_timer_handle_t  timer_handle;       // timer handler
    float        buff[FILTER_WINDOW];       // buffer from reading
    uint32_t                   index;       // last index for buffer
} Sensor_CB;

static Sensor_CB g_sensor_cb;

static void read_data(void *arg)
{
    if (NULL == g_sensor_cb.sensor) return;
    // read from sensor and save
    if (ESP_OK != g_sensor_cb.sensor->read_handle(
            &g_sensor_cb.buff[(g_sensor_cb.index++)&(FILTER_WINDOW-1)])) return;
}

static float get_lux(void)
{
    float lux = 0.0;
    for (int i = 0; i < FILTER_WINDOW; i++) {
        lux += g_sensor_cb.buff[i];
    }
    // return average
    return lux/FILTER_WINDOW;
}

static int ambient_get_handle(const cJSON *cmd_json, const cJSON *rsp_json, void *arg)
{
    if (NULL == cmd_json || NULL == rsp_json) return SAL_ENTRY_INVALID_PARAM;

    // prepare result
    cJSON *json_body = cJSON_AddObjectToObject((cJSON *)rsp_json, "body");
    if (NULL == cJSON_AddStringToObject(json_body, "entry", "sal_get")) {
        ESP_LOGE(SAL_TAG, "Failed to encode body [entry]");
        return SAL_ENTRY_RESULT_ERROR;
    }
    cJSON *json_result = cJSON_AddObjectToObject(json_body, "result");
    if (cJSON_AddNumberToObject(json_result, "lux", (double)get_lux()) == NULL) {
        ESP_LOGE(SAL_TAG, "Failed to encode lux");
        return SAL_ENTRY_RESULT_ERROR;
    }

    return SAL_ENTRY_OK;
}

static int ambient_sub_handle(const cJSON *cmd_json, const cJSON *rsp_json, void *arg)
{
    if (NULL == cmd_json || NULL == rsp_json) return SAL_ENTRY_INVALID_PARAM;
    return SAL_ENTRY_NOT_IMPLEMENTED;
}

#endif /* CONFIG_NODE_AMBIENT_LIGHT */

/***
 * @description : regitster entry for sensor
 * @return       {*}
 */
esp_err_t register_sensor_ambient(void)
{
#ifdef CONFIG_NODE_AMBIENT_LIGHT
    memset(&g_sensor_cb, 0, sizeof(Sensor_CB));
#ifdef CONFIG_AMBIENT_TEMT_6000
    g_sensor_cb.sensor = temt6000_instance();
#elif CONFIG_AMBIENT_VEML_7700
    g_sensor_cb.sensor = veml7700_instance();
#else
    #error "unknown amblient light sensor type"
#endif

    if (NULL == g_sensor_cb.sensor) {
        ESP_LOGE(SAL_TAG, "failed to create sensor instance");
        return ESP_FAIL;
    }

    ESP_ERROR_CHECK(g_sensor_cb.sensor->init_handle(NULL));
    ESP_ERROR_CHECK(register_entry("sal_get", ambient_get_handle));
    ESP_ERROR_CHECK(register_entry("sal_subscribe", ambient_sub_handle));


    esp_timer_create_args_t sal_timer = {
        .arg = NULL,
        .callback = read_data,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "sal_timer"
    };

    esp_timer_create(&sal_timer, &g_sensor_cb.timer_handle);
    esp_timer_start_periodic(g_sensor_cb.timer_handle,
                    CONFIG_NODE_AMBIENT_LIGHT_READ_TIMER * 1000000U);

#endif /* CONFIG_NODE_AMBIENT_LIGHT */
    return ESP_OK;
}