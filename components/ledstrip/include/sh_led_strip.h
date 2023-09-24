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

/***
 * @description : init LED Strip
 * @return       {*}
 */
esp_err_t register_led_strip(void);

#ifdef __cplusplus
}
#endif

#endif /* SHELLHOME_LED_STRIP */
