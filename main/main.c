/*
 * @Author      : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @Date        : 2023-09-08 16:16:12
 * @LastEditors : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @LastEditTime: 2023-09-27 22:11:14
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
#include "simple_switch.h"
#include "sh_led_strip.h"
#include "node_status.h"


/* declaration of event group for the node */
EventGroupHandle_t node_event_group;

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

    /* Initialize status */
    shn_init_status();

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

    /* Register Simple Switch */
    ESP_ERROR_CHECK(register_simple_switch());

    /* Register Simple Switch */
    ESP_ERROR_CHECK(register_led_strip());

    /* Launch protocol */
    ESP_ERROR_CHECK(launch_shn_proto());

    while (1) {
        shn_flush_status();
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
