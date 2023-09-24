/*
 * @Author      : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @Date        : 2023-09-24 23:05:20
 * @LastEditors : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @LastEditTime: 2023-09-24 23:54:07
 * @FilePath    : /shellhomenode/components/ledstrip/src/sh_led_strip.c
 * @Description :
 * Copyright (c) 2023 by Zheng, Yang, All Rights Reserved.
 */

#include "led_strip.h"
#include "sh_led_strip.h"

static const char *LS_TAG = "led_strip";

static led_strip_handle_t g_led_strip;


/**
 * @brief Set RGB for all specific pixel
 *
 * @param red: red part of color
 * @param green: green part of color
 * @param blue: blue part of color
 *
 * @return
 *      - ESP_OK: Set RGB for a specific pixel successfully
 *      - ESP_ERR_INVALID_ARG: Set RGB for a specific pixel failed because of invalid parameters
 *      - ESP_FAIL: Set RGB for a specific pixel failed because other error occurred
 */
static esp_err_t all_set_pixel(uint32_t red, uint32_t green, uint32_t blue)
{
#ifdef CONFIG_NODE_USING_LED_STRIP
    for (int i = 0; i < CONFIG_LED_STRIP_GPIO_NUM; i++) {
        ESP_ERROR_CHECK(led_strip_set_pixel(g_led_strip, i, red, green, blue));
    }
    /* Refresh the strip to send data */
    ESP_ERROR_CHECK(led_strip_refresh(g_led_strip));
#endif /* CONFIG_NODE_USING_LED_STRIP */
    return ESP_OK;
}

static esp_err_t all_clear(void)
{
    return led_strip_clear(g_led_strip);
}

static int led_strip_handle(const cJSON *cmd_json, const cJSON *rsp_json, void *arg)
{
    return 0;
}

/***
 * @description : init LED Strip
 * @return       {*}
 */
esp_err_t register_led_strip(void)
{
#ifdef CONFIG_NODE_USING_LED_STRIP
    // LED strip general initialization
    led_strip_config_t strip_config = {
        .strip_gpio_num = CONFIG_LED_STRIP_GPIO_NUM,    // The GPIO connected to the LED strip's data line
        .max_leds = CONFIG_LED_NUM,                     // The number of LEDs in the strip,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB,       // Pixel format of your LED strip
        .led_model = LED_MODEL_WS2812,                  // LED strip model
        .flags.invert_out = false,                      // whether to invert the output signal
    };

    // LED strip backend configuration: RMT
    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,                 // different clock source can lead to different power consumption
        .resolution_hz = CONFIG_LED_STRIP_RESOLUTION_HZ, // RMT counter clock frequency
        .flags.with_dma = false,               // DMA feature is available on ESP target like ESP32-S3
    };

    // LED Strip object handle
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &g_led_strip));
    ESP_LOGI(LS_TAG, "Created LED strip object with RMT backend");

    all_set_pixel(100, 100, 50);
    return register_entry("ledstrip", led_strip_handle);
#endif /* CONFIG_NODE_USING_LED_STRIP */
    return ESP_OK;
}