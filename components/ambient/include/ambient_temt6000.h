/***
 * @Author      : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @Date        : 2023-10-02 16:12:40
 * @LastEditors : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @LastEditTime: 2023-10-02 16:12:43
 * @FilePath    : /shellhomenode/components/shnode/include/ambient_temt6000.h
 * @Description : ambient light sensor TEMT6000
 * @Copyright (c) 2023 by Zheng, Yang, All Rights Reserved.
 */
#ifndef SHELLHOME_AMBIENT_TEMT6000
#define SHELLHOME_AMBIENT_TEMT6000

#ifdef __cplusplus
extern "C" {
#endif

#include "sensor_ambient.h"

/***
 * @description : get instance of the sensor temt6000
 * @return       {*}
 */
Sensor_Ambient_Lisght *temt6000_instance(void);

#ifdef __cplusplus
}
#endif

#endif /* SHELLHOME_AMBIENT_TEMT6000 */
