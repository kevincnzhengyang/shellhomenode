/*
 * @Author      : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @Date        : 2023-09-08 18:40:25
 * @LastEditors : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @LastEditTime: 2023-09-11 22:14:16
 * @FilePath    : /shellhomenode/components/shnode/src/shn_network.c
 * @Description :
 * Copyright (c) 2023 by Zheng, Yang, All Rights Reserved.
 */

#include "esp_wifi.h"
#include "esp_netif.h"
#include "mdns.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"

#include "shn_network.h"

static const char *NET_TAG = "node_net";

static const char *STR_CMD_HELLO = "hello";
static const char *STR_CMD_FORGET = "forget";
static const char *STR_CMD_REQUEST = "request";

#ifdef CONFIG_NODE_USING_IPV6
static bool g_using_ipv6 = CONFIG_NODE_USING_IPV6;
#else
static bool g_using_ipv6 = false;
#endif

#define NODE_NAME_LEN       12
#define ADDR_STR_ELN        128

typedef enum
{
    CMD_HELLO,
    CMD_FORGET,
    CMD_REQUEST,
    CMD_BUTT
} cmd_type_enum;

typedef esp_err_t (* cmd_handle)(int sock, int index,
                        const cJSON *json_cmd_body, cJSON *json_rsp);

typedef enum
{
    CODE_OK,
    CODE_UNKNOWN_CMD,
    CODE_INVALID_INDEX,
    CODE_INVALID_TERM,
    CODE_BUTT
} rsp_status_enum;

typedef struct
{
    char             *term_name;    // terminal name
    int              remote_seq;    // last msg seq from remote terminal
    int               local_seq;    // last msg seq from local
    char addr_str[ADDR_STR_ELN];    // remote addr in string
} node_term_info;

typedef struct
{
    char                   node_name[NODE_NAME_LEN];    // node physical name
    node_term_info      terms[CONFIG_NODE_TERM_NUM];    // terminal table
    int                                 terms_index;    // last terminal index
    struct sockaddr_in6                 server_addr;    // UDP server address
    struct sockaddr_storage             source_addr;    // UDP client address
    shn_proto_config                        *config;    // handler table
    void                                *handle_arg;    // user defined argument
} shn_proto_cb;

static shn_proto_cb g_node_proto;

static void get_node_name(char *service_name, size_t max)
{
    uint8_t eth_mac[6];
    const char *ssid_prefix = "SHN_";
    esp_wifi_get_mac(WIFI_IF_STA, eth_mac);
    snprintf(service_name, max, "%s%02X%02X%02X",
             ssid_prefix, eth_mac[3], eth_mac[4], eth_mac[5]);
}

static cmd_type_enum get_cmd_type(char *cmd_str)
{
    if (NULL == cmd_str) {
        return CMD_BUTT;
    }
    if (0 == strncmp(cmd_str, STR_CMD_HELLO, strlen(STR_CMD_HELLO)))
    {
        return CMD_HELLO;
    }
    if (0 == strncmp(cmd_str, STR_CMD_FORGET, strlen(STR_CMD_FORGET)))
    {
        return CMD_FORGET;
    }
    if (0 == strncmp(cmd_str, STR_CMD_REQUEST, strlen(STR_CMD_REQUEST)))
    {
        return CMD_REQUEST;
    }
    return CMD_BUTT;
}

static esp_err_t response_request(int sock, cJSON *json_rsp, int status)
{
    if (-1 == sock || NULL == json_rsp) {
        return ESP_FAIL;
    }

    if (NULL == cJSON_AddNumberToObject(json_rsp, "status", status)) {
        ESP_LOGE(NET_TAG, "Failed to encode [status]\n");
        return ESP_FAIL;
    }

    char *str_rsp = cJSON_PrintUnformatted(json_rsp);
    if (NULL == str_rsp) {
        ESP_LOGE(NET_TAG, "failed to encode JSON response");
        return ESP_FAIL;
    }

    int err = sendto(sock, str_rsp, strlen(str_rsp), 0,
                    (struct sockaddr *)&g_node_proto.source_addr,
                    sizeof(g_node_proto.source_addr));
    if (err < 0) {
        ESP_LOGE(NET_TAG, "Error occurred during sending: errno %d", errno);
        return ESP_FAIL;
    }

    return ESP_OK;
}

static esp_err_t start_mdns(void)
{
    // init mDNS
    esp_err_t err = mdns_init();
    if (err) {
        return err;
    }

    // set hostname
    mdns_hostname_set(g_node_proto.node_name);
    // set default instance
    mdns_instance_name_set("ShellHome");
    // add service
    mdns_service_add(NULL, "_shnode", "_udp",
            CONFIG_NODE_SERVICE_PORT, NULL, 0);

    return ESP_OK;
}

static esp_err_t handle_hello(int sock, int index,
                    const cJSON *json_cmd_body, cJSON *json_rsp)
{
    if (ESP_OK != response_request(sock, json_rsp, CODE_OK)) {
        ESP_LOGE(NET_TAG, "Failed to response request\n");
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t handle_forget(int sock, int index,
                    const cJSON *json_cmd_body, cJSON *json_rsp)
{
    if (ESP_OK != response_request(sock, json_rsp, CODE_OK)) {
        ESP_LOGE(NET_TAG, "Failed to response request\n");
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t handle_request(int sock, int index,
                    const cJSON *json_cmd_body, cJSON *json_rsp)
{
    if (ESP_OK != response_request(sock, json_rsp, CODE_OK)) {
        ESP_LOGE(NET_TAG, "Failed to response request\n");
        return ESP_FAIL;
    }
    return ESP_OK;
}

static cmd_handle g_cmd_tab[CMD_BUTT] =
{
    handle_hello,
    handle_forget,
    handle_request
};

static esp_err_t process_udp_msg(int sock, char *buff, int size)
{
    esp_err_t res = ESP_FAIL;
    const cJSON *json_term = NULL, *json_token = NULL, *json_index = NULL,
            *json_seq = NULL, *json_cmd = NULL, *json_body = NULL;
    cJSON *json_rsp = NULL, *root = NULL;

    json_rsp = cJSON_CreateObject();
    if (NULL == json_rsp) {
        ESP_LOGE(NET_TAG, "Failed to prepare JSON response\n");
        goto end;
    }

    root = cJSON_ParseWithLength(buff, size);
    if (NULL == root) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (NULL != error_ptr)
        {
            ESP_LOGE(NET_TAG, "Parse JSON error before: %s\n", error_ptr);
        }

        goto end;
    }

    // get term, token, index, seq, cmd and body
    json_term = cJSON_GetObjectItemCaseSensitive(root, "term");
    if (!cJSON_IsString(json_term) || (NULL == json_term->valuestring)) {
        ESP_LOGE(NET_TAG, "JSON failed to get string element [term]\n");
        goto end;
    }
    json_token = cJSON_GetObjectItemCaseSensitive(root, "token");
    if (!cJSON_IsString(json_token) || (NULL == json_token->valuestring)) {
        ESP_LOGE(NET_TAG, "JSON failed to get string element [token]\n");
        goto end;
    }
    json_index = cJSON_GetObjectItemCaseSensitive(root, "index");
    if (!cJSON_IsNumber(json_index)) {
        ESP_LOGE(NET_TAG, "JSON failed to get number element [index]\n");
        goto end;
    }
    json_seq = cJSON_GetObjectItemCaseSensitive(root, "seq");
    if (!cJSON_IsNumber(json_seq)) {
        ESP_LOGE(NET_TAG, "JSON failed to get number element [seq]\n");
        goto end;
    }
    json_cmd = cJSON_GetObjectItemCaseSensitive(root, "cmd");
    if (!cJSON_IsString(json_cmd) || (NULL == json_cmd->valuestring)) {
        ESP_LOGE(NET_TAG, "JSON failed to get string element [cmd]\n");
        goto end;
    }
    json_body = cJSON_GetObjectItemCaseSensitive(root, "body");
    if (!cJSON_IsObject(json_body)) {
        ESP_LOGE(NET_TAG, "JSON failed to get object element [body]\n");
        goto end;
    }

    // init response
    if (NULL == cJSON_AddStringToObject(json_rsp, "term", json_term->valuestring)) {
        ESP_LOGE(NET_TAG, "Failed to encode [term]\n");
        goto end;
    }
    if (NULL == cJSON_AddNumberToObject(json_rsp, "seq", json_seq->valueint)) {
        ESP_LOGE(NET_TAG, "Failed to encode [term]\n");
        goto end;
    }
    if (NULL == cJSON_AddStringToObject(json_rsp, "cmd", json_cmd->valuestring)) {
        ESP_LOGE(NET_TAG, "Failed to encode [cmd]\n");
        goto end;
    }

    // todo check token
    ESP_LOGD(NET_TAG, "msg token = %s\n", json_token->valuestring);

    cmd_type_enum cmd_type = get_cmd_type(json_cmd->valuestring);
    if (CMD_BUTT == cmd_type) {
        ESP_LOGE(NET_TAG, "unknown cmd [%s]\n", json_cmd->valuestring);
        response_request(sock, json_rsp, CODE_UNKNOWN_CMD);
        goto end;
    }

    // check term
    if (-1 != json_index->valueint) {
        if (CONFIG_NODE_TERM_NUM <= json_index->valueint) {
            // invalid index
            ESP_LOGE(NET_TAG, "invalid index [%d]\n", json_index->valueint);
            response_request(sock, json_rsp, CODE_INVALID_INDEX);
            goto end;
        }

        if (0 != strncmp(json_term->valuestring,
                    g_node_proto.terms[json_index->valueint].term_name,
                    NODE_NAME_LEN)) {
            // wrong term info
            ESP_LOGE(NET_TAG, "wrong term [%s]@[%d]\n",
                json_term->valuestring,
                json_index->valueint);
            response_request(sock, json_rsp, CODE_INVALID_TERM);
            goto end;
        }
    }

    res = g_cmd_tab[cmd_type](sock, json_index->valueint, json_body, json_rsp);
end:
    cJSON_Delete(json_rsp);
    cJSON_Delete(root);
    return res;
}

static void udp_server_task(void *arg)
{
    g_node_proto.config = (shn_proto_config *)arg;

    char rx_buffer[CONFIG_NODE_PROTO_BUFF_SIZE];
    memset(rx_buffer, 0, CONFIG_NODE_PROTO_BUFF_SIZE);

    char addr_str[128];     // remote addr in string

    while (1) {
        int sock = -1;
        if (!g_using_ipv6) {
            sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
        } else {
            sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_IPV6);
        }

        if (0 > sock) {
            ESP_LOGE(NET_TAG, "Unable to create socket: errno %d", errno);
            break;
        }

        ESP_LOGI(NET_TAG, "Socket created");
        if (g_using_ipv6) {
            int opt = 1;
            setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
            setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(opt));
        }

        // Set timeout
        struct timeval timeout;
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;
        setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);

        int err = bind(sock, (struct sockaddr *)&g_node_proto.server_addr,
                            sizeof(g_node_proto.server_addr));
        if (err < 0) {
            ESP_LOGE(NET_TAG, "Socket unable to bind: errno %d", errno);
        }
        ESP_LOGI(NET_TAG, "Socket bound, port %d", CONFIG_NODE_SERVICE_PORT);

        socklen_t socklen = sizeof(g_node_proto.source_addr);

        while (1) {
            ESP_LOGI(NET_TAG, "Waiting for data");
            int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0,
                                    (struct sockaddr *)&g_node_proto.source_addr,
                                    &socklen);

            if (len < 0) {
                ESP_LOGE(NET_TAG, "recvfrom failed: errno %d", errno);
                // TODO count and power save
                // if (EGAIN == errno) {
                //     // count
                // }
                break;
            } else {
                if (g_node_proto.source_addr.ss_family == PF_INET) {
                    inet_ntoa_r(((struct sockaddr_in *)&g_node_proto.source_addr)->sin_addr,
                                    addr_str, sizeof(addr_str) - 1);
                } else if (g_node_proto.source_addr.ss_family == PF_INET6) {
                    inet6_ntoa_r(((struct sockaddr_in6 *)&g_node_proto.source_addr)->sin6_addr,
                                    addr_str, sizeof(addr_str) - 1);
                }
                rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string...
                ESP_LOGI(NET_TAG, "Received %d bytes from %s:", len, addr_str);
                ESP_LOGD(NET_TAG, "%s", rx_buffer);

                // todo
                if (ESP_OK != process_udp_msg(sock, rx_buffer, len)) {
                    ESP_LOGE(NET_TAG, "failed to process udp [%s]", rx_buffer);
                    break;
                }
            }
        }

        if (sock != -1) {
            ESP_LOGE(NET_TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }
    vTaskDelete(NULL);
}

/***
 * @description : initialize protocol of ShellHome Node
 * @param        {void} *arg pointer to user defined argument
 * @return       {*}
 */
esp_err_t init_shn_proto(void *arg)
{
    memset(&g_node_proto, 0, sizeof(g_node_proto));
    g_node_proto.handle_arg = arg;

    // esp_netif_ip_info_t ip_info;
    // esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"), &ip_info);
    // char ip_addr[16];
    // inet_ntoa_r(ip_info.ip.addr, ip_addr, 16);
    // ESP_LOGI(NET_TAG, "Proto IP: %s", ip_addr);

    // get proto node name
    get_node_name(g_node_proto.node_name, NODE_NAME_LEN);

    esp_err_t err = start_mdns();
    if (ESP_OK != err) {
        ESP_LOGE(NET_TAG, "MDNS Init failed: %d\n", err);
        return ESP_FAIL;
    } else {
        ESP_LOGI(NET_TAG, "MDNS Init OK\n");
    }

    if (!g_using_ipv6) {
        struct sockaddr_in *dest_addr_ip4 =
                (struct sockaddr_in *)&g_node_proto.server_addr;
        dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr_ip4->sin_family = AF_INET;
        dest_addr_ip4->sin_port = htons(CONFIG_NODE_SERVICE_PORT);
    } else {
        // bzero(&g_node_proto.server_addr.sin6_addr.un, sizeof(g_node_proto.server_addr.sin6_addr.un));
        g_node_proto.server_addr.sin6_family = AF_INET6;
        g_node_proto.server_addr.sin6_port = htons(CONFIG_NODE_SERVICE_PORT);
    }

    return ESP_OK;
}

/***
 * @description : launch ShellHome Node protocol
 * @param        {config} *shn_proto_config pointer to config
 * @return       {*}
 */
esp_err_t launch_shn_proto(shn_proto_config *config)
{
    if (NULL == config) {
        ESP_LOGE(NET_TAG, "launch proto without request config\n");
        return ESP_FAIL;
    }

    xTaskCreate(udp_server_task, "proto_server",
        CONFIG_NODE_PROTO_STACK, (void *)config, 5, NULL);
    return ESP_OK;
}
