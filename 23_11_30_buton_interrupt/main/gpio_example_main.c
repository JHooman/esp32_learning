/* GPIO Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include "driver/gpio.h"
#include "esp_log.h"
#define GPIO_PIN_DIODE 5
#define GPIO_BUTTON 4
void button_interrupt()
{
    gpio_set_level(GPIO_PIN_DIODE, 0);
    ESP_LOGI("Info","button click");
}
void gpio_configure()
{
    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_BUTTON,button_interrupt,NULL);
    gpio_set_direction(GPIO_PIN_DIODE, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_BUTTON, GPIO_MODE_INPUT_OUTPUT);
    gpio_set_level(GPIO_PIN_DIODE, 0);
    gpio_set_level(GPIO_BUTTON, 1);
    gpio_set_intr_type(GPIO_BUTTON,GPIO_INTR_LOW_LEVEL);
}


void app_main(void)
{
    gpio_configure();
    while (1) {
        gpio_set_level(GPIO_PIN_DIODE, 1);
    }
}
