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

typedef enum {
    SS_ENTRY_OK,
    SS_ENTRY_INVALID_PARAM,
    SS_ENTRY_MISSING_SLOTS,
    SS_ENTRY_MISSING_INDEX,
    SS_ENTRY_MISSING_VALUE,
    SS_ENTRY_INVALID_INDEX,
    SS_ENTRY_NOT_CONFIGED,
    SS_ENTRY_RESULT_ERROR,
} SimpleSwitchErrEnum;

/***
 * @description : init simple switch
 * @return       {*}
 */
esp_err_t register_simple_switch(void);

#ifdef __cplusplus
}
#endif

#endif /* SHELLHOME_SIMPLE_SWITCH */
