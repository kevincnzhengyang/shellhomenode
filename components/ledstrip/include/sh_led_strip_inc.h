/***
 * @Author      : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @Date        : 2023-10-05 17:04:16
 * @LastEditors : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @LastEditTime: 2023-10-05 17:04:17
 * @FilePath    : /shellhomenode/components/ledstrip/include/sh_led_strip_inc.h
 * @Description : functions for LED strip
 * @Copyright (c) 2023 by Zheng, Yang, All Rights Reserved.
 */
#ifndef SHELLHOME_LED_STRIP_INC
#define SHELLHOME_LED_STRIP_INC

#ifdef __cplusplus
extern "C" {
#endif

#include "sh_led_strip.h"

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
esp_err_t all_set_pixel(uint32_t red, uint32_t green, uint32_t blue);

/**
 * @description : Clear LED Strip
 * @return       {*}
 */
esp_err_t all_clear(void);

/**
 * @description : marquee mode
 * @param        {void} *args
 * @return       {*}
 */
void marquee_cb(void *args);

/**
 * @description : breath mod
 * @param        {void} *args
 * @return       {*}
 */
void breath_cb(void *args);

/**
 * @description : stop running
 * @return       {*}
 */
esp_err_t stop_running(void);

/**
 * @description : start running
 * @param        {esp_timer_cb_t} cb - mod callback
 * @param        {uint64_t} periodic - period in ms
 * @return       {*}
 */
esp_err_t start_running(esp_timer_cb_t cb, uint64_t periodic);
#ifdef __cplusplus
}
#endif

#endif /* SHELLHOME_LED_STRIP_INC */

