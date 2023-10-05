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

#include "cJSON.h"

#include "events_def.h"


// function for handle request command
// NOTE: return 0 means success, > 0 means error happened
typedef int (* shn_cmd_handle)(const cJSON *cmd_json, const cJSON *rsp_json, void *arg);

/***
 * @description : initialize protocol of ShellHome Node
 * @param        {void} *arg pointer to user defined argument
 * @return       {*}
 */
esp_err_t init_shn_proto(void *arg);

/***
 * @description : register an entry into protocol
 * @param        {char} *entry: name of entry
 * @param        {shn_cmd_handle} handler: pointer to handler
 * @param        {shn_cmd_handle} hook: pointer to hook
 * @param        {void} *arg: pointer to user defined argument
 * @return       {*}
 */
esp_err_t register_entry(const char *entry, shn_cmd_handle handler,
                        shn_cmd_handle hook, void *arg);

/***
 * @description : add a hook for an entry
 * @param        {char} *entry: name of entry
 * @param        {shn_cmd_handle} hook: pointer to hook
 * @return       {*}
 */
esp_err_t add_entry_hook(const char *entry, shn_cmd_handle hook);

/***
 * @description : launch ShellHome Node protocol
 * @return       {*}
 */
esp_err_t launch_shn_proto(void);

#ifdef __cplusplus
}
#endif

#endif /* SHELLHOME_NODE_NETWORK */
