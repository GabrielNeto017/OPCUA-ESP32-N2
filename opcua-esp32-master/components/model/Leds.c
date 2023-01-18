// Author: Gabriel Neto

#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <esp_task_wdt.h>

#include "Leds.h"
#include "model.h"

#define LED1 0
#define LED2 12

TaskHandle_t panic;

void leds_init(){
    gpio_pad_select_gpio(LED1);
    gpio_set_direction (LED1,GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(LED2);
    gpio_set_direction (LED2,GPIO_MODE_OUTPUT);
    gpio_set_level(LED1,1);
    gpio_set_level(LED2,1);
}

void errorled_on(){
    gpio_set_level(LED1,0);
}

void errorled_off(){
    vTaskDelay(100/portTICK_RATE_MS);
    gpio_set_level(LED1, 0);
    vTaskDelay(100/portTICK_RATE_MS);
    gpio_set_level(LED1,1);

 
}

void requisicao(){
    vTaskDelay(100/portTICK_RATE_MS);
    gpio_set_level(LED2,0);
    vTaskDelay(100/portTICK_RATE_MS);
    gpio_set_level(LED2,1);
    

}

