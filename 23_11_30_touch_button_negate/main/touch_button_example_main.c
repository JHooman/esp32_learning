/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "iot_button.h"
#include "esp_log.h"
#include <stdio.h>
#include <stdlib.h>
#include "driver/gpio.h"
#include "esp_log.h"


#define GPIO_PIN_DIODE 5
#define GPIO_BUTTON 4
bool diode_state = 1;
void button_hold()
{
    diode_state = !diode_state;
    gpio_set_level(GPIO_PIN_DIODE, diode_state);
    ESP_LOGI("Info","button click");
}

void gpio_configure()
{
    gpio_set_direction(GPIO_PIN_DIODE, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_PIN_DIODE, 1);
    button_config_t gpio_btn_cfg = {
        .type = BUTTON_TYPE_GPIO,
        .gpio_button_config = {
            .gpio_num = GPIO_BUTTON,
            .active_level = 0,
        },
    };
    
    button_handle_t gpio_btn = iot_button_create(&gpio_btn_cfg);
    iot_button_register_cb(gpio_btn, BUTTON_PRESS_DOWN, button_hold, NULL);

}


void app_main(void)
{
    gpio_configure();
    while (1) {
        vTaskDelay(1);
    }
}
