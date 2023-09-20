/***
 * @Author      : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @Date        : 2023-09-20 22:14:23
 * @LastEditors : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @LastEditTime: 2023-09-20 22:14:26
 * @FilePath    : /shellhomenode/components/simpleswitch/include/simple_switch.h
 * @Description : simple switch
 * @Copyright (c) 2023 by Zheng, Yang, All Rights Reserved.
 */
#ifndef SHELLHOME_SIMPLE_SWITCH
#define SHELLHOME_SIMPLE_SWITCH

#ifdef __cplusplus
extern "C" {
#endif

#include "shn_network.h"

/***
 * @description : init simple switch
 * @return       {*}
 */
esp_err_t register_simple_switch(void);

#ifdef __cplusplus
}
#endif

#endif /* SHELLHOME_SIMPLE_SWITCH */
