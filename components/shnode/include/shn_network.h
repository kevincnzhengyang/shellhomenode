/***
 * @Author      : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @Date        : 2023-09-08 18:39:51
 * @LastEditors : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @LastEditTime: 2023-09-08 18:39:54
 * @FilePath    : /shellhomenode/components/shnode/include/shn_network.h
 * @Description :
 * @Copyright (c) 2023 by Zheng, Yang, All Rights Reserved.
 */
#ifndef SHELLHOME_NODE_NETWORK
#define SHELLHOME_NODE_NETWORK

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>

#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_event.h>

#include "events_def.h"


// function for handle request command
typedef void (* shn_cmd_handle)(void *stack, void *cmd);

typedef struct {
    char                *req_cmd;       // request command
    shn_cmd_handle       handler;       // handler for command
} shn_cmd_table;

typedef struct {
    shn_cmd_table     *cmd_table;
    int               table_size;
} shn_proto_config;

/***
 * @description : initialize protocol of ShellHome Node
 * @return       {*}
 */
esp_err_t init_shn_proto(void);

/***
 * @description : launch ShellHome Node protocol
 * @param        {shn_proto_cnf} *shn_proto_config pointer to config
 * @return       {*}
 */
esp_err_t launch_shn_proto(shn_proto_config *config);
#ifdef __cplusplus
}
#endif

#endif /* SHELLHOME_NODE_NETWORK */
