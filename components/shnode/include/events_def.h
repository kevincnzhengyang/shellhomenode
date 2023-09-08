/***
 * @Author      : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @Date        : 2023-08-22 15:41:04
 * @LastEditors : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @LastEditTime: 2023-08-22 15:41:07
 * @FilePath    : /shellhomenode/components/shnode/include/events_def.h
 * @Description : application events definitions
 * @Copyright (c) 2023 by Zheng, Yang, All Rights Reserved.
 */
#ifndef SHELLHOME_NODE_EVENTS_DEF_H
#define SHELLHOME_NODE_EVENTS_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>

#include <freertos/event_groups.h>
#include <esp_event.h>

#define WIFI_CONNECTED_EVENT BIT0

#define NODE_HELLO_EVENT     BIT1
#define NODE_FORGET_EVENT    BIT2
#define NODE_REQUEST_EVENT   BIT3


extern EventGroupHandle_t node_event_group;

#ifdef __cplusplus
}
#endif

#endif /* SHELLHOME_NODE_EVENTS_DEF_H */
