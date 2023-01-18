// Author: Gabriel Neto


#include "driver/ledc.h"
#include <driver/gpio.h>
//#include <driver/include/driver/ledc.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>


#include "PWM.h"
#include "model.h"
#include "ReadInput.c"

#define LED_RED 4   //RED
#define LED_BLUE 17  //BLUE
#define LED_GREEN 16  //GREEN


int config = 0;
int32_t freq = 10;
int32_t duty = 100;



ledc_timer_config_t ledc_timer =
{  
    .speed_mode = LEDC_HIGH_SPEED_MODE,     // LEDC speed speed_mode, high-speed mode or low-speed mode
    .timer_num = LEDC_TIMER_0,              // The timer source of channel (0 - 3)
    .duty_resolution = LEDC_TIMER_8_BIT,    // range 0..255
    .freq_hz = 50                        // changes/second
};

ledc_timer_config_t ledc_timer2 =
{  
    .speed_mode = LEDC_HIGH_SPEED_MODE,     // LEDC speed speed_mode, high-speed mode or low-speed mode
    .timer_num = LEDC_TIMER_1,              // The timer source of channel (0 - 3)
    .duty_resolution = LEDC_TIMER_8_BIT,    // range 0..255
    .freq_hz = 50                        // changes/second
};

ledc_timer_config_t ledc_timer3 =
{  
    .speed_mode = LEDC_HIGH_SPEED_MODE,     // LEDC speed speed_mode, high-speed mode or low-speed mode
    .timer_num = LEDC_TIMER_2,              // The timer source of channel (0 - 3)
    .duty_resolution = LEDC_TIMER_8_BIT,    // range 0..255
    .freq_hz = 50                        // changes/second
};

ledc_channel_config_t ledc_channel =
{ 
    .timer_sel = LEDC_TIMER_0, .speed_mode = LEDC_HIGH_SPEED_MODE, .channel = LEDC_CHANNEL_0, 
    .gpio_num = LED_RED, .duty = 0 
};

ledc_channel_config_t ledc_channel2 =
{ 
    .timer_sel = LEDC_TIMER_1, .speed_mode = LEDC_HIGH_SPEED_MODE, .channel = LEDC_CHANNEL_1, 
    .gpio_num = LED_GREEN, .duty = 0 
};

ledc_channel_config_t ledc_channel3 =
{ 
    .timer_sel = LEDC_TIMER_2, .speed_mode = LEDC_HIGH_SPEED_MODE, .channel = LEDC_CHANNEL_2, 
    .gpio_num = LED_BLUE, .duty = 0 
};


int ESTADO = 100;


void PWM_init(){

    gpio_pad_select_gpio(LED_RED);
    gpio_set_direction (LED_RED,GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(LED_GREEN);
    gpio_set_direction (LED_GREEN,GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(LED_BLUE);
    gpio_set_direction (LED_BLUE,GPIO_MODE_OUTPUT);

}


// 0 - undefined
// 1 - EMERGENCIA
// 2 - RESET
// 3 - DESLIGADO
// 4 - AGUARDANDO
// 5 - OCUPADO
// 6 - MODO CARNAVAL

void configled(){

    
    //R
    ledc_set_pin(LED_RED, LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
    ledc_channel_config(&ledc_channel);
    ledc_timer_config(&ledc_timer);

    
    //G
    ledc_set_pin(LED_GREEN, LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);
    ledc_channel_config(&ledc_channel2);
    ledc_timer_config(&ledc_timer2);

    //B
    ledc_set_pin(LED_BLUE, LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2);
    ledc_channel_config(&ledc_channel3);
    ledc_timer_config(&ledc_timer3);
    
}


void ledred(){
    ledc_set_freq(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0, freq);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
}

void ledgreen(){

    ledc_set_freq(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_1, freq);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, duty);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);
}

void ledblue(){
    ledc_set_freq(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_2, freq);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, duty);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2);
}


void statusled(){

    freq = 255; //255 changes per second
    //DUTY = 0-255 (RGB COLORS)
    configled();
    ESTADO = levelled;

    
    if(ESTADO == 0)
    {
        gpio_pad_select_gpio(LED_RED);
        gpio_set_direction (LED_RED,GPIO_MODE_OUTPUT);
        gpio_set_level(LED_RED, 0);

        gpio_pad_select_gpio(LED_GREEN);
        gpio_set_direction (LED_GREEN,GPIO_MODE_OUTPUT);
        gpio_set_level(LED_GREEN, 0);

        gpio_pad_select_gpio(LED_BLUE);
        gpio_set_direction (LED_BLUE,GPIO_MODE_OUTPUT);
        gpio_set_level(LED_BLUE, 0);

        vTaskDelay(300/portTICK_RATE_MS);

        gpio_set_level(LED_RED, 1);
        gpio_set_level(LED_GREEN, 1);
        gpio_set_level(LED_BLUE, 1);

        vTaskDelay(300/portTICK_RATE_MS);

        
    }

    else if(ESTADO == 1){

        freq = 255; //255 changes per second
        duty = 255;
        ledred();
        duty = 0;
        ledgreen(); 
        duty = 0;
        ledblue();

    }

    else if(ESTADO == 2){

        freq = 255; //255 changes per second
        duty = 0;
        ledred();
        duty = 0;
        ledgreen(); 
        duty = 255;
        ledblue();
    }

    else if(ESTADO == 3){

        freq = 255; //255 changes per second
        duty = 255;
        ledred();
        duty = 100;
        ledgreen(); 
        duty = 0;
        ledblue();

        //gpio_set_level(LED_GREEN,1);

    }
    else if(ESTADO == 4){

        freq = 255; //255 changes per second
        duty = 0;
        ledred();
        duty = 255;
        ledgreen(); 
        duty = 0;
        ledblue();

    }
    else if(ESTADO == 5){

        gpio_pad_select_gpio(LED_GREEN);
        gpio_set_direction (LED_GREEN,GPIO_MODE_OUTPUT);
        gpio_set_level(LED_GREEN, 0);
        vTaskDelay(700/portTICK_RATE_MS);
        gpio_set_level(LED_GREEN, 1);
        //vTaskDelay(100/portTICK_RATE_MS);
    }
    else if(ESTADO == 6 ){

        freq = 30;
        
        
        duty = rand() % 255;
        ledred();
        duty = rand() % 255;
        ledgreen(); 
        duty = rand() % 255;
        ledblue();


    }

    else{

    
    }

}





