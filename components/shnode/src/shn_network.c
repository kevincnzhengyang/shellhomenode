/*
 * @Author      : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @Date        : 2023-09-08 18:40:25
 * @LastEditors : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @LastEditTime: 2023-09-10 16:23:44
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

#ifdef CONFIG_NODE_USING_IPV6
static bool g_using_ipv6 = CONFIG_NODE_USING_IPV6;
#else
static bool g_using_ipv6 = false;
#endif

#define NODE_NAME_LEN       12
#define ADDR_STR_ELN        128

typedef struct
{
    char             *term_name;    // terminal name
    int              remote_seq;    // last msg seq from remote terminal
    int               local_seq;    // last msg seq from local
    char addr_str[ADDR_STR_ELN];    // remote addr in string
} node_term_info;

typedef struct
{
    char                   node_name[NODE_NAME_LEN];
    node_term_info      terms[CONFIG_NODE_TERM_NUM];
    int                                 terms_index;
    struct sockaddr_in6                   dest_addr;
    shn_proto_config                        *config;
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

static esp_err_t handle_hello(void *stack, void *data)
{
    return ESP_OK;
}

static esp_err_t handle_forget(void *stack, void *data)
{
    return ESP_OK;
}

static esp_err_t handle_request(void *stack, void *data)
{
    return ESP_OK;
}

static esp_err_t process_udp_msg(char *buff, int size)
{
    return ESP_OK;
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
        // struct timeval timeout;
        // timeout.tv_sec = 10;
        // timeout.tv_usec = 0;
        // setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);

        int err = bind(sock, (struct sockaddr *)&g_node_proto.dest_addr,
                            sizeof(g_node_proto.dest_addr));
        if (err < 0) {
            ESP_LOGE(NET_TAG, "Socket unable to bind: errno %d", errno);
        }
        ESP_LOGI(NET_TAG, "Socket bound, port %d", CONFIG_NODE_SERVICE_PORT);

        struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
        socklen_t socklen = sizeof(source_addr);

        while (1) {
            ESP_LOGI(NET_TAG, "Waiting for data");
            int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0,
                                    (struct sockaddr *)&source_addr, &socklen);

            if (len < 0) {
                ESP_LOGE(NET_TAG, "recvfrom failed: errno %d", errno);
                break;
            } else {
                if (source_addr.ss_family == PF_INET) {
                    inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr,
                                    addr_str, sizeof(addr_str) - 1);
                } else if (source_addr.ss_family == PF_INET6) {
                    inet6_ntoa_r(((struct sockaddr_in6 *)&source_addr)->sin6_addr,
                                    addr_str, sizeof(addr_str) - 1);
                }
                rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string...
                ESP_LOGI(NET_TAG, "Received %d bytes from %s:", len, addr_str);
                ESP_LOGI(NET_TAG, "%s", rx_buffer);

                // todo
                if (ESP_OK != process_udp_msg(rx_buffer, len)) {
                    ESP_LOGE(NET_TAG, "failed to handle message %s", rx_buffer);
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
 * @return       {*}
 */
esp_err_t init_shn_proto(void)
{
    memset(&g_node_proto, 0, sizeof(g_node_proto));

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
                (struct sockaddr_in *)&g_node_proto.dest_addr;
        dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr_ip4->sin_family = AF_INET;
        dest_addr_ip4->sin_port = htons(CONFIG_NODE_SERVICE_PORT);
    } else {
        // bzero(&g_node_proto.dest_addr.sin6_addr.un, sizeof(g_node_proto.dest_addr.sin6_addr.un));
        g_node_proto.dest_addr.sin6_family = AF_INET6;
        g_node_proto.dest_addr.sin6_port = htons(CONFIG_NODE_SERVICE_PORT);
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
