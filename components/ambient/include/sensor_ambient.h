/***
 * @Author      : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @Date        : 2023-10-02 16:16:09
 * @LastEditors : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @LastEditTime: 2023-10-02 16:16:13
 * @FilePath    : /shellhomenode/components/shnode/include/sensor_ambient.h
 * @Description : ambient light sensor
 * @Copyright (c) 2023 by Zheng, Yang, All Rights Reserved.
 */
#ifndef SHELLHOME_AMBIENT_SENSOR
#define SHELLHOME_AMBIENT_SENSOR

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "esp_timer.h"
#include "esp_err.h"
#include "esp_log.h"

typedef enum {
    SAL_ENTRY_OK,
    SAL_ENTRY_INVALID_PARAM,
    SAL_ENTRY_NOT_IMPLEMENTED,
    SAL_ENTRY_RESULT_ERROR
} Sensor_Ambient_Light_Err_Enum;


// function to init sensor
typedef esp_err_t (*sal_init)(void * arg);

// function to read lux data
typedef esp_err_t (*sal_read)(float *o_lux);

typedef struct
{
    sal_init        init_handle;
    sal_read        read_handle;
} Sensor_Ambient_Lisght;

#define FILTER_WINDOW       (1<<2)

typedef struct {
    Sensor_Ambient_Lisght    *sensor;       // sensor
    esp_timer_handle_t  timer_handle;       // timer handler
    float        buff[FILTER_WINDOW];       // buffer from reading
    uint32_t                   index;       // last index for buffer
} Sensor_CB;

/***
 * @description : regitster entry for sensor
 * @return       {*}
 */
esp_err_t register_sensor_ambient(void);

#ifdef __cplusplus
}
#endif

#endif /* SHELLHOME_AMBIENT_SENSOR */


