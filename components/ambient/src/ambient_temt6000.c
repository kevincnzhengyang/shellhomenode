/*
 * @Author      : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @Date        : 2023-10-02 16:12:58
 * @LastEditors : kevin.z.y <kevin.cn.zhengyang@gmail.com>
 * @LastEditTime: 2023-10-03 20:57:36
 * @FilePath    : /shellhomenode/components/ambient/src/ambient_temt6000.c
 * @Description : ambient light sensor TEMT6000
 * Copyright (c) 2023 by Zheng, Yang, All Rights Reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"

#include "ambient_temt6000.h"

#ifdef CONFIG_AMBIENT_TEMT_6000
#define NO_OF_SAMPLES (1<<4)
#define DEFAULT_VREF    1100        /**< Use adc2_vref_to_gpio() to obtain a better estimate */

static const char *S_TAG = "temt6000";

adc_oneshot_unit_handle_t g_adc1_handle;
adc_cali_handle_t g_adc1_cali_handle = NULL;
#endif /* CONFIG_AMBIENT_TEMT_6000 */

Sensor_Ambient_Lisght *g_sensor = NULL;

static esp_err_t sensor_init(void * arg)
{
#ifdef CONFIG_AMBIENT_TEMT_6000
    // init ADC unit
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &g_adc1_handle));

    // config channel
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_11,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(g_adc1_handle,
                                    CONFIG_NODE_AMBIENT_LIGHT_CHN, &config));

    // config calibration
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    if (!calibrated) {
        ESP_LOGI(S_TAG, "calibration scheme version is %s", "Curve Fitting");
        adc_cali_curve_fitting_config_t cali_config = {
            .unit_id = ADC_UNIT_1,
            .atten = ADC_ATTEN_DB_11,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_curve_fitting(&cali_config, &g_adc1_cali_handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    if (!calibrated) {
        ESP_LOGI(S_TAG, "calibration scheme version is %s", "Line Fitting");
        adc_cali_line_fitting_config_t cali_config = {
            .unit_id = ADC_UNIT_1,
            .atten = ADC_ATTEN_DB_11,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_line_fitting(&cali_config, &g_adc1_cali_handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

    if (ret == ESP_OK) {
        ESP_LOGI(S_TAG, "Calibration Success");
    } else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {
        ESP_LOGW(S_TAG, "eFuse not burnt, skip software calibration");
    } else {
        ESP_LOGE(S_TAG, "Invalid arg or no memory");
    }
#endif /* CONFIG_AMBIENT_TEMT_6000 */
    return ESP_OK;
}

// get adc voltage in mV
static void adc_get_voltage(int *out_voltage)
{
#ifdef CONFIG_AMBIENT_TEMT_6000
    static uint32_t sample_index = 0;
    static int filter_buf[NO_OF_SAMPLES] = {0};
    int raw;

    ESP_ERROR_CHECK(adc_oneshot_read(g_adc1_handle,
                                CONFIG_NODE_AMBIENT_LIGHT_CHN,
                                &raw));
    ESP_ERROR_CHECK(adc_oneshot_read(g_adc1_handle,
                                CONFIG_NODE_AMBIENT_LIGHT_CHN,
                                &filter_buf[(sample_index)&(NO_OF_SAMPLES-1)]));
    ESP_LOGI(S_TAG, "ADC Channel[%d] Raw Data: %d @ %"PRIu32", %d",
                CONFIG_NODE_AMBIENT_LIGHT_CHN,
                filter_buf[sample_index&(NO_OF_SAMPLES-1)],
                sample_index&(NO_OF_SAMPLES-1), raw);
    sample_index++;

    int sum = 0;
    for (int i = 0; i < NO_OF_SAMPLES; i++) {
        sum += filter_buf[i];
    }

    sum /= NO_OF_SAMPLES;
    ESP_LOGI(S_TAG, "sum [%d]", sum);

    /**< Convert adc_reading to voltage in mV */
    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(g_adc1_cali_handle, (int)sum, (int *)out_voltage));
#endif /* CONFIG_AMBIENT_TEMT_6000 */
}

// function to read lux data
static esp_err_t sensor_read(float *o_lux)
{
#ifdef CONFIG_AMBIENT_TEMT_6000
    if (NULL == o_lux) {
        ESP_LOGE(S_TAG, "Invalid parameter for lux output");
        return ESP_FAIL;
    }

    // get voltage
    int mv = 0;
    adc_get_voltage(&mv);
    // convert to lux
    *o_lux = (float)mv * 0.9765625;  // 1000/1024
    ESP_LOGI(S_TAG, "voltage %d, lux %0.4f", mv, *o_lux);
#endif /* CONFIG_AMBIENT_TEMT_6000 */
    return ESP_OK;
}


/***
 * @description : get instance of the sensor temt6000
 * @return       {*}
 */
Sensor_Ambient_Lisght *temt6000_instance(void)
{
    if (NULL != g_sensor) return g_sensor;

    g_sensor = (Sensor_Ambient_Lisght *)malloc(sizeof(Sensor_Ambient_Lisght));
    memset(g_sensor, 0, sizeof(Sensor_Ambient_Lisght));
    g_sensor->init_handle = sensor_init;
    g_sensor->read_handle = sensor_read;

    return g_sensor;
}
