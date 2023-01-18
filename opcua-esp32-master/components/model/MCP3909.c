// Author: Gabriel Luiz B. Neto
// Exsto Tecnologia
#include "soc/rtc_wdt.h"

#include "MCP3909.h"
#include <driver/gpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "Leds.h"

#include "hal/timer_types.h"
#include "esp_task_wdt.h"

/*
#define MCP_CS 5
#define MCP_SCK 18
#define MCP_MISO 19
#define MCP_MOSI 23

#define MCLR 13
#define G0_gain 26
#define G1_gain 27

#define PRE_HPF1    0xA4
#define POST_HPF1   0xAC
*/

float Vglobal = 0;
float Iglobal = 0;
float Pglobal = 0;
float Sglobal = 0;
float Rglobal = 0;


void Analisador(double tensao1, double corrente1, double potencia1, double aparente1, double reativa1)
{
    Vglobal = tensao1;
    Iglobal = corrente1;
    Pglobal = potencia1;
    Sglobal = aparente1;
    Rglobal = reativa1;
}

//INITIALIZE SPI COMMUNICATION
void mcpInitSPI()
{     /* 
    
        
         ganho = 0: CH0 Gain = 01 | Tensão máxima = 470mV;
         G1 = 0     G0 = 0

        ganho = 1: CH0 Gain = 02 | Tensão máxima = 235mV;
        G1 = 1      G0 = 0

        ganho = 2: CH0 Gain = 08 | Tensão máxima = 60mV;
        G1 = 0      G0 = 1

        ganho = 3: CH0 Gain = 16 | Tensão máxima = 30mV.    
        G1 = 1      G0 = 1
    */

 
}

void EXECUTE_SPI()
{    
        
    
    
}

