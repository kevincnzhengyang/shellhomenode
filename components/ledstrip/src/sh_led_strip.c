/*
 * @Author      : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @Date        : 2023-09-24 23:05:20
 * @LastEditors : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @LastEditTime: 2023-10-11 21:41:15
 * @FilePath    : /shellhomenode/components/ledstrip/src/sh_led_strip.c
 * @Description :
 * Copyright (c) 2023 by Zheng, Yang, All Rights Reserved.
 */

#include "led_strip.h"

#include "sh_led_strip.h"
#include "sh_led_strip_inc.h"

#ifdef CONFIG_NODE_USING_LED_STRIP
/**
 * @brief sin() function from 0 to 2π, in total 255 values rounded up，maximum 255，minimum 0
 *
 */
uint8_t  const SinValue[256]={	128,   131,   134,   137,   140,   143,   147,   150,   153,   156,
                                159,   162,   165,   168,   171,   174,   177,   180,   182,   185,
                                188,   191,   194,   196,   199,   201,   204,   206,   209,   211,
                                214,   216,   218,   220,   223,   225,   227,   229,   230,   232,
                                234,   236,   237,   239,   240,   242,   243,   245,   246,   247,
                                248,   249,   250,   251,   252,   252,   253,   253,   254,   254,
                                255,   255,   255,   255,   255,   255,   255,   255,   255,   254,
                                254,   253,   253,   252,   251,   250,   249,   249,   247,   246,
                                245,   244,   243,   241,   240,   238,   237,   235,   233,   231,
                                229,   228,   226,   224,   221,   219,   217,   215,   212,   210,
                                208,   205,   203,   200,   198,   195,   192,   189,   187,   184,
                                181,   178,   175,   172,   169,   166,   163,   160,   157,   154,
                                151,   148,   145,   142,   139,   136,   132,   129,   126,   123,
                                120,   117,   114,   111,   107,   104,   101,    98,    95,    92,
                                89,    86,    83,    80,    77,    74,    72,    69,    66,    63,
                                61,    58,    55,    53,    50,    48,    45,    43,    41,    38,
                                36,    34,    32,    30,    28,    26,    24,    22,    21,    19,
                                17,    16,    14,    13,    12,    10,     9,     8,     7,     6,
                                5,     4,     4,     3,     2,     2,     1,     1,     1,     0,
                                0,     0,     0,     0,     1,     1,     1,     2,     2,     3,
                                3,     4,     5,     6,     6,     7,     9,    10,    11,    12,
                                14,    15,    17,    18,    20,    21,    23,    25,    27,    29,
                                31,    33,    35,    37,    40,    42,    44,    47,    49,    52,
                                54,    57,    59,    62,    65,    67,    70,    73,    76,    79,
                                82,    85,    88,    91,    94,    97,   100,   103,   106,   109,
                                112,   115,   118,   121,   125,   128
};

static const char *LS_TAG = "led_strip";

#define LS_MAX_STRIP_NUM        4

static LED_Strip_Stru g_led_strips[LS_MAX_STRIP_NUM];


/**
 * @brief Set RGB for all specific pixel
 *
 * @param red: red part of color
 * @param green: green part of color
 * @param blue: blue part of color
 *
 * @return
 *      - ESP_OK: Set RGB for a specific pixel successfully
 *      - ESP_ERR_INVALID_ARG: Set RGB for a specific pixel failed because of invalid parameters
 *      - ESP_FAIL: Set RGB for a specific pixel failed because other error occurred
 */
esp_err_t all_set_pixel(LED_Strip_Stru *strip, uint32_t red, uint32_t green, uint32_t blue)
{
    if (NULL == strip || !strip->configed) return ESP_FAIL;
    for (int i = 0; i < strip->led_num; i++) {
        ESP_ERROR_CHECK(led_strip_set_pixel(strip->led_strip, i, red, green, blue));
    }
    /* Refresh the strip to send data */
    ESP_ERROR_CHECK(led_strip_refresh(strip->led_strip));
    ESP_LOGI(LS_TAG, "LED strip set RGB %ld,%ld,%ld", red, green, blue);
    return ESP_OK;
}

/**
 * @description : Clear LED Strip
 * @return       {LED_Strip_Stru  *}strip - pointer to LED_Strip_Stru
 */
esp_err_t all_clear(LED_Strip_Stru *strip)
{
    if (NULL == strip || !strip->configed) return ESP_FAIL;
    return led_strip_clear(strip->led_strip);
}

/**
 * @description : marquee mode
 * @param        {void} *args
 * @return       {*}
 */
void marquee_cb(void *args)
{
    LED_Strip_Stru *strip = (LED_Strip_Stru *)args;
	uint8_t ir, ib;
    ir = (uint8_t)(strip->count + 85);
    ib = (uint8_t)(strip->count + 170);
    for (int j = 0; j < strip->led_num; j ++) {
        // set RGB
        ESP_ERROR_CHECK(led_strip_set_pixel(strip->led_strip, j,
                            (uint32_t)SinValue[(ir + 10 * j) % 256],      // red
                            (uint32_t)SinValue[(strip->count + 10 * j) % 256],  // green
                            (uint32_t)SinValue[(ib + 10 * j) % 256]));    // blue
    }
    strip->count++;

    // refresh
    ESP_ERROR_CHECK(led_strip_refresh(strip->led_strip));
}

/**
 * @description : breath mod
 * @param        {void} *args
 * @return       {*}
 */
void breath_cb(void *args)
{
    LED_Strip_Stru *strip = (LED_Strip_Stru *)args;
	uint8_t ir, ib;
    ir = (uint8_t)(strip->count + 85);
    ib = (uint8_t)(strip->count + 170);
    for (int j = 0; j < strip->led_num; j ++) {
        // set RGB
        ESP_ERROR_CHECK(led_strip_set_pixel(strip->led_strip, j,
                            (uint32_t)SinValue[ir],         // red
                            (uint32_t)SinValue[strip->count], // green
                            (uint32_t)SinValue[ib]));       // blue
    }
    strip->count++;

    // refresh
    ESP_ERROR_CHECK(led_strip_refresh(strip->led_strip));
}

/**
 * @description : stop running
 * @return       {LED_Strip_Stru *} strip - pointer to LED_Strip_Stru
 */
esp_err_t stop_running(LED_Strip_Stru *strip)
{
    if (NULL == strip || ! strip->configed) return ESP_FAIL;
    if (true == strip->running) {
        esp_timer_stop(strip->timer_handle);
        esp_timer_delete(strip->timer_handle);
        strip->running = false;
    }
    all_clear(strip);
    ESP_LOGI(LS_TAG, "LED strip running stop");
    return ESP_OK;
}

/**
 * @description : start running
 * @return       {LED_Strip_Stru *} strip - pointer to LED_Strip_Stru
 * @param        {esp_timer_cb_t} cb - mod callback
 * @param        {uint64_t} periodic - period in ms
 * @return       {*}
 */
esp_err_t start_running(LED_Strip_Stru *strip, esp_timer_cb_t cb, uint64_t periodic)
{
    if (NULL == strip || ! strip->configed) return ESP_FAIL;
    esp_timer_create_args_t strip_timer = {
        .arg = (void *)strip,
        .callback = cb,
        .dispatch_method = ESP_TIMER_TASK,
        .name = strip->name
    };

    if (true == strip->running) {
        stop_running(strip);
    }

    esp_timer_create(&strip_timer, &strip->timer_handle);
    esp_timer_start_periodic(strip->timer_handle, periodic * 1000U);
    strip->running = true;
    ESP_LOGI(LS_TAG, "LED strip running start");
    return ESP_OK;
}

/***
 * @description : get pointer of LED_Strip_Stru by index
 * @param        {int} index
 * @return       {LED_Strip_Stru*}
 */
LED_Strip_Stru *get_led_strip_by_index(int index)
{
    if (0 > index || LS_MAX_STRIP_NUM <= index) {
        ESP_LOGE(LS_TAG, "invalid index [%d]", index);
        return NULL;
    }
    if (!g_led_strips[index].configed) {
        ESP_LOGE(LS_TAG, "not configed index [%d]", index);
        return NULL;
    }
    return &g_led_strips[index];
}

static int dimmable_handle(const cJSON *cmd_json, const cJSON *rsp_json, void *arg)
{
    if (NULL == cmd_json || NULL == rsp_json) return LS_ENTRY_INVALID_PARAM;

    // get on
    cJSON const *json_on = cJSON_GetObjectItemCaseSensitive(cmd_json, "on");
    if (!cJSON_IsBool(json_on)) {
        ESP_LOGE(LS_TAG, "JSON failed to get boolean element [on]");
        return LS_ENTRY_MISSING_ON;
    }
    // prepare result
    cJSON *json_body = cJSON_AddObjectToObject((cJSON *)rsp_json, "body");
    if (NULL == cJSON_AddStringToObject(json_body, "entry", "dimmable_light")) {
        ESP_LOGE(LS_TAG, "Failed to encode body [entry]");
        return LS_ENTRY_RESULT_ERROR;
    }
    cJSON *json_result = cJSON_AddObjectToObject(json_body, "result");
    if (cJSON_AddBoolToObject(json_result, "on", cJSON_IsTrue(json_on)) == NULL) {
        ESP_LOGE(LS_TAG, "Failed to encode on");
        return LS_ENTRY_RESULT_ERROR;
    }

    // get and check index
    cJSON *json_index = cJSON_GetObjectItemCaseSensitive(cmd_json, "index");
    if (!cJSON_IsNumber(json_index)) {
        ESP_LOGE(LS_TAG, "JSON failed to get integer element [index]");
        return LS_ENTRY_MISSING_INDEX;
    }
    if (cJSON_AddNumberToObject(json_result, "index", json_index->valueint) == NULL) {
        ESP_LOGE(LS_TAG, "Failed to encode index [%d]", json_index->valueint);
        return LS_ENTRY_RESULT_ERROR;
    }
    if (0 > json_index->valueint || LS_MAX_STRIP_NUM <= json_index->valueint) {
        ESP_LOGE(LS_TAG, "invalid index [%d]", json_index->valueint);
        return LS_ENTRY_INVALID_INDEX;
    }
    if (!g_led_strips[json_index->valueint].configed) {
        ESP_LOGE(LS_TAG, "not configed index [%d]", json_index->valueint);
        return LS_ENTRY_NOT_CONFIGED;
    }

    if (cJSON_IsFalse(json_on)) {
        // LED off
        return stop_running(&g_led_strips[json_index->valueint]);
    }

    // get RGB value and set
    const cJSON *json_r = cJSON_GetObjectItemCaseSensitive(cmd_json, "r");
    if (!cJSON_IsNumber(json_r)) {
        ESP_LOGE(LS_TAG, "JSON failed to get element [r]");
        return LS_ENTRY_MISSING_R;
    }
    if (0 > json_r->valueint || 255 < json_r->valueint) {
        ESP_LOGE(LS_TAG, "invalid R 0~255 [%d]", json_r->valueint);
        return LS_ENTRY_INVALID_R;
    }
    const cJSON *json_g = cJSON_GetObjectItemCaseSensitive(cmd_json, "g");
    if (!cJSON_IsNumber(json_g)) {
        ESP_LOGE(LS_TAG, "JSON failed to get element [r]");
        return LS_ENTRY_MISSING_G;
    }
    if (0 > json_g->valueint || 255 < json_g->valueint) {
        ESP_LOGE(LS_TAG, "invalid G 0~255 [%d]", json_g->valueint);
        return LS_ENTRY_INVALID_G;
    }
    const cJSON *json_b = cJSON_GetObjectItemCaseSensitive(cmd_json, "b");
    if (!cJSON_IsNumber(json_b)) {
        ESP_LOGE(LS_TAG, "JSON failed to get element [r]");
        return LS_ENTRY_MISSING_B;
    }
    if (0 > json_b->valueint || 255 < json_b->valueint) {
        ESP_LOGE(LS_TAG, "invalid B 0~255 [%d]", json_b->valueint);
        return LS_ENTRY_INVALID_B;
    }

    return all_set_pixel(&g_led_strips[json_index->valueint],
                         (uint32_t)json_r->valueint,
                         (uint32_t)json_g->valueint,
                         (uint32_t)json_b->valueint);
}

static int marquee_handle(const cJSON *cmd_json, const cJSON *rsp_json, void *arg)
{
    if (NULL == cmd_json || NULL == rsp_json) return LS_ENTRY_INVALID_PARAM;

    // get on
    cJSON const *json_on = cJSON_GetObjectItemCaseSensitive(cmd_json, "on");
    if (NULL == json_on) {
        ESP_LOGE(LS_TAG, "JSON failed to get element [on]");
        return LS_ENTRY_MISSING_ON;
    }

    // prepare result
    cJSON *json_body = cJSON_AddObjectToObject((cJSON *)rsp_json, "body");
    if (NULL == cJSON_AddStringToObject(json_body, "entry", "switch")) {
        ESP_LOGE(LS_TAG, "Failed to encode body [entry]");
        return LS_ENTRY_RESULT_ERROR;
    }
    cJSON *json_result = cJSON_AddObjectToObject(json_body, "result");
    if (cJSON_AddBoolToObject(json_result, "on", cJSON_IsTrue(json_on)) == NULL) {
        ESP_LOGE(LS_TAG, "Failed to encode on");
        return LS_ENTRY_RESULT_ERROR;
    }

    // get and check index
    cJSON *json_index = cJSON_GetObjectItemCaseSensitive(cmd_json, "index");
    if (!cJSON_IsNumber(json_index)) {
        ESP_LOGE(LS_TAG, "JSON failed to get integer element [index]");
        return LS_ENTRY_MISSING_INDEX;
    }
    if (cJSON_AddNumberToObject(json_result, "index", json_index->valueint) == NULL) {
        ESP_LOGE(LS_TAG, "Failed to encode index [%d]", json_index->valueint);
        return LS_ENTRY_RESULT_ERROR;
    }
    if (0 > json_index->valueint || LS_MAX_STRIP_NUM <= json_index->valueint) {
        ESP_LOGE(LS_TAG, "invalid index [%d]", json_index->valueint);
        return LS_ENTRY_INVALID_INDEX;
    }
    if (!g_led_strips[json_index->valueint].configed) {
        ESP_LOGE(LS_TAG, "not configed index [%d]", json_index->valueint);
        return LS_ENTRY_NOT_CONFIGED;
    }

    if (cJSON_IsFalse(json_on)) {
        // LED off
        return stop_running(&g_led_strips[json_index->valueint]);
    }

    // get RGB value and set
    const cJSON *json_period = cJSON_GetObjectItemCaseSensitive(cmd_json, "period");
    if (!cJSON_IsNumber(json_period)) {
        ESP_LOGE(LS_TAG, "JSON failed to get element [period]");
        return LS_ENTRY_MISSING_PERIOD;
    }

    // start marquee
    ESP_LOGI(LS_TAG, "LED strip marquee start");
    return start_running(&g_led_strips[json_index->valueint], marquee_cb,
                         (uint64_t)json_period->valueint);
}

static int breath_handle(const cJSON *cmd_json, const cJSON *rsp_json, void *arg)
{
    if (NULL == cmd_json || NULL == rsp_json) return LS_ENTRY_INVALID_PARAM;

    // get on
    cJSON const *json_on = cJSON_GetObjectItemCaseSensitive(cmd_json, "on");
    if (NULL == json_on) {
        ESP_LOGE(LS_TAG, "JSON failed to get element [on]");
        return LS_ENTRY_MISSING_ON;
    }

    // prepare result
    cJSON *json_body = cJSON_AddObjectToObject((cJSON *)rsp_json, "body");
    if (NULL == cJSON_AddStringToObject(json_body, "entry", "switch")) {
        ESP_LOGE(LS_TAG, "Failed to encode body [entry]");
        return LS_ENTRY_RESULT_ERROR;
    }
    cJSON *json_result = cJSON_AddObjectToObject(json_body, "result");
    if (cJSON_AddBoolToObject(json_result, "on", cJSON_IsTrue(json_on)) == NULL) {
        ESP_LOGE(LS_TAG, "Failed to encode on");
        return LS_ENTRY_RESULT_ERROR;
    }

    // get and check index
    cJSON *json_index = cJSON_GetObjectItemCaseSensitive(cmd_json, "index");
    if (!cJSON_IsNumber(json_index)) {
        ESP_LOGE(LS_TAG, "JSON failed to get integer element [index]");
        return LS_ENTRY_MISSING_INDEX;
    }
    if (cJSON_AddNumberToObject(json_result, "index", json_index->valueint) == NULL) {
        ESP_LOGE(LS_TAG, "Failed to encode index [%d]", json_index->valueint);
        return LS_ENTRY_RESULT_ERROR;
    }
    if (0 > json_index->valueint || LS_MAX_STRIP_NUM <= json_index->valueint) {
        ESP_LOGE(LS_TAG, "invalid index [%d]", json_index->valueint);
        return LS_ENTRY_INVALID_INDEX;
    }
    if (!g_led_strips[json_index->valueint].configed) {
        ESP_LOGE(LS_TAG, "not configed index [%d]", json_index->valueint);
        return LS_ENTRY_NOT_CONFIGED;
    }

    if (cJSON_IsFalse(json_on)) {
        // LED off
        return stop_running(&g_led_strips[json_index->valueint]);
    }

    // get RGB value and set
    const cJSON *json_period = cJSON_GetObjectItemCaseSensitive(cmd_json, "period");
    if (!cJSON_IsNumber(json_period)) {
        ESP_LOGE(LS_TAG, "JSON failed to get element [period]");
        return LS_ENTRY_MISSING_PERIOD;
    }

    // start breath
    ESP_LOGI(LS_TAG, "LED strip breath start");
    return start_running(&g_led_strips[json_index->valueint],
                         breath_cb, (uint64_t)json_period->valueint);
}
#endif /* CONFIG_NODE_USING_LED_STRIP */

/***
 * @description : init LED Strip
 * @return       {*}
 */
esp_err_t register_led_strip(void)
{
#ifdef CONFIG_NODE_USING_LED_STRIP
    // init CB
    memset(g_led_strips, 0, sizeof(g_led_strips));
#ifdef CONFIG_USING_STRIP_1
    g_led_strips[0].configed = true;
    g_led_strips[0].running = false;
    g_led_strips[0].name = strdup("strip 1");
     g_led_strips[0].led_num = CONFIG_STRIP1_LED_NUM;
    // LED strip general initialization
    led_strip_config_t strip_config1 = {
        .strip_gpio_num = CONFIG_STRIP1_GPIO_NUM,       // The GPIO connected to the LED strip's data line
        .max_leds = CONFIG_STRIP1_LED_NUM,              // The number of LEDs in the strip,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB,       // Pixel format of your LED strip
        .led_model = LED_MODEL_WS2812,                  // LED strip model
        .flags.invert_out = false,                      // whether to invert the output signal
    };

    // LED strip backend configuration: RMT
    led_strip_rmt_config_t rmt_config1 = {
        .clk_src = RMT_CLK_SRC_DEFAULT,                 // different clock source can lead to different power consumption
        .resolution_hz = CONFIG_LED_STRIP_RESOLUTION_HZ, // RMT counter clock frequency
        .flags.with_dma = false,               // DMA feature is available on ESP target like ESP32-S3
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config1, &rmt_config1,
                                             &(g_led_strips[0].led_strip)));
    ESP_LOGI(LS_TAG, "Created LED strip 1 object with RMT backend");
#endif /* CONFIG_USING_STRIP_1 */
#ifdef CONFIG_USING_STRIP_2
    g_led_strips[1].configed = true;
    g_led_strips[1].running = false;
    g_led_strips[1].name = strdup("strip 2");
     g_led_strips[1].led_num = CONFIG_STRIP2_LED_NUM;
    // LED strip general initialization
    led_strip_config_t strip_config2 = {
        .strip_gpio_num = CONFIG_STRIP2_GPIO_NUM,       // The GPIO connected to the LED strip's data line
        .max_leds = CONFIG_STRIP2_LED_NUM,              // The number of LEDs in the strip,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB,       // Pixel format of your LED strip
        .led_model = LED_MODEL_WS2812,                  // LED strip model
        .flags.invert_out = false,                      // whether to invert the output signal
    };

    // LED strip backend configuration: RMT
    led_strip_rmt_config_t rmt_config2 = {
        .clk_src = RMT_CLK_SRC_DEFAULT,                 // different clock source can lead to different power consumption
        .resolution_hz = CONFIG_LED_STRIP_RESOLUTION_HZ, // RMT counter clock frequency
        .flags.with_dma = false,               // DMA feature is available on ESP target like ESP32-S3
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config2, &rmt_config2,
                                             &(g_led_strips[1].led_strip)));
    ESP_LOGI(LS_TAG, "Created LED strip 2 object with RMT backend");
#endif /* CONFIG_USING_STRIP_2 */
#ifdef CONFIG_USING_STRIP_3
    g_led_strips[2].configed = true;
    g_led_strips[2].running = false;
    g_led_strips[2].name = strdup("strip 3");
    g_led_strips[2].led_num = CONFIG_STRIP3_LED_NUM;
    // LED strip general initialization
    led_strip_config_t strip_config3 = {
        .strip_gpio_num = CONFIG_STRIP3_GPIO_NUM,       // The GPIO connected to the LED strip's data line
        .max_leds = CONFIG_STRIP3_LED_NUM,              // The number of LEDs in the strip,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB,       // Pixel format of your LED strip
        .led_model = LED_MODEL_WS2812,                  // LED strip model
        .flags.invert_out = false,                      // whether to invert the output signal
    };

    // LED strip backend configuration: RMT
    led_strip_rmt_config_t rmt_config3 = {
        .clk_src = RMT_CLK_SRC_DEFAULT,                 // different clock source can lead to different power consumption
        .resolution_hz = CONFIG_LED_STRIP_RESOLUTION_HZ, // RMT counter clock frequency
        .flags.with_dma = false,               // DMA feature is available on ESP target like ESP32-S3
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config3, &rmt_config3,
                                             &(g_led_strips[2].led_strip)));
    ESP_LOGI(LS_TAG, "Created LED strip 3 object with RMT backend");
#endif /* CONFIG_USING_STRIP_2 */
#ifdef CONFIG_USING_STRIP_4
    g_led_strips[3].configed = true;
    g_led_strips[3].running = false;
    g_led_strips[3].name = strdup("strip 4");
    g_led_strips[3].led_num = CONFIG_STRIP3_LED_NUM;
    // LED strip general initialization
    led_strip_config_t strip_config4 = {
        .strip_gpio_num = CONFIG_STRIP4_GPIO_NUM,       // The GPIO connected to the LED strip's data line
        .max_leds = CONFIG_STRIP4_LED_NUM,              // The number of LEDs in the strip,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB,       // Pixel format of your LED strip
        .led_model = LED_MODEL_WS2812,                  // LED strip model
        .flags.invert_out = false,                      // whether to invert the output signal
    };

    // LED strip backend configuration: RMT
    led_strip_rmt_config_t rmt_config4 = {
        .clk_src = RMT_CLK_SRC_DEFAULT,                 // different clock source can lead to different power consumption
        .resolution_hz = CONFIG_LED_STRIP_RESOLUTION_HZ, // RMT counter clock frequency
        .flags.with_dma = false,               // DMA feature is available on ESP target like ESP32-S3
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config4, &rmt_config4,
                                             &(g_led_strips[3].led_strip)));
    ESP_LOGI(LS_TAG, "Created LED strip 4 object with RMT backend");
#endif /* CONFIG_USING_STRIP_3 */

    // LED Strip object handle
    ESP_ERROR_CHECK(register_entry("dimmable_light", dimmable_handle, NULL, g_led_strips));
    ESP_ERROR_CHECK(register_entry("marquee", marquee_handle, NULL, g_led_strips));
    ESP_ERROR_CHECK(register_entry("breathing_light", breath_handle, NULL, g_led_strips));
#endif /* CONFIG_NODE_USING_LED_STRIP */
    return ESP_OK;
}