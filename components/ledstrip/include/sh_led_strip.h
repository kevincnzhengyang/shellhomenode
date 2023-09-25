/***
 * @Author      : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @Date        : 2023-09-24 23:05:28
 * @LastEditors : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @LastEditTime: 2023-09-24 23:05:32
 * @FilePath    : /shellhomenode/components/ledstrip/include/led_strip.h
 * @Description :
 * @Copyright (c) 2023 by Zheng, Yang, All Rights Reserved.
 */
#ifndef SHELLHOME_LED_STRIP
#define SHELLHOME_LED_STRIP

#ifdef __cplusplus
extern "C" {
#endif

#include "shn_network.h"


typedef enum {
    LS_ENTRY_OK,
    LS_ENTRY_INVALID_PARAM,
    LS_ENTRY_MISSING_ON,
    LS_ENTRY_MISSING_R,
    LS_ENTRY_MISSING_G,
    LS_ENTRY_MISSING_B,
    LS_ENTRY_INVALID_R,
    LS_ENTRY_INVALID_G,
    LS_ENTRY_INVALID_B,
    LS_ENTRY_MISSING_PERIOD,
    LS_ENTRY_RESULT_ERROR,
} LEDStripErrEnum;


/***
 * @description : init LED Strip
 * @return       {*}
 */
esp_err_t register_led_strip(void);

#ifdef __cplusplus
}
#endif

#endif /* SHELLHOME_LED_STRIP */
