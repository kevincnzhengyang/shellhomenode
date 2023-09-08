/***
 * @Author      : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @Date        : 2023-09-08 17:10:57
 * @LastEditors : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @LastEditTime: 2023-09-08 17:11:00
 * @FilePath    : /shellhomenode/components/shnode/include/blufi_prov.h
 * @Description :
 * @Copyright (c) 2023 by Zheng, Yang, All Rights Reserved.
 */
#ifndef SHELLHOME_NODE_BLUFI_PROV_H
#define SHELLHOME_NODE_BLUFI_PROV_H

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

#include <wifi_provisioning/manager.h>
#include <wifi_provisioning/scheme_ble.h>

#include "qrcode.h"

#include "events_def.h"


/***
 * @description : enable gpio as the button to reset network
 * @return       {*}
 */
esp_err_t en_btn_network_reset(void);

/***
 * @description : initialize wifi
 * @return       {*}
 */
esp_err_t init_wifi(void);

/***
 * @description : start network provisioning
 *
 * @param        {bool} forced_prov - forced flag
 * @return       {*} ESP_OK when success
 */
esp_err_t network_provision(bool forced_prov);

#ifdef __cplusplus
}
#endif

#endif /* SHELLHOME_NODE_BLUFI_PROV_H */
