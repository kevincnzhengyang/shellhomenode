/***
 * @Author      : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @Date        : 2023-10-03 19:27:59
 * @LastEditors : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @LastEditTime: 2023-10-03 19:27:59
 * @FilePath    : /shellhomenode/components/ambient/include/ambient_veml7700.h
 * @Description : ambient light sensor VEML7700
 * @Copyright (c) 2023 by Zheng, Yang, All Rights Reserved.
 */
#ifndef SHELLHOME_AMBIENT_VEML7700
#define SHELLHOME_AMBIENT_VEML7700

#ifdef __cplusplus
extern "C" {
#endif

#include "sensor_ambient.h"

/***
 * @description : get instance of the sensor veml7700
 * @return       {*}
 */
Sensor_Ambient_Lisght *veml7700_instance(void);

#ifdef __cplusplus
}
#endif

#endif /* SHELLHOME_AMBIENT_VEML7700 */