//Author Gabriel Luiz B. Neto

#include <stdio.h>
#include "esp_event.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "ReadInput.h"


#define DI0 34
#define DI1 14
#define DI2 15
#define DI3 35

#define MCP_CS 5
#define MCP_SCK 18
#define MCP_MISO 19
#define MCP_MOSI 23

#define MCLR 13

#define SW1 2
#define SW2 32

int level1, level2, level3, level4 = 2;
int levelled, levelatual=0;
int sw1, sw2 = 2;
int flagatual =0;
int flag = 1;
int flag2 = 0;
int flag3 = 0; 
int count1, count2 = 0;
int denominador = 1;
//char ESTADO_M = 'none';

void ReadInput_init(){
    gpio_pad_select_gpio(DI0);                    // Defines GPIO functionality for the pin
    gpio_set_direction(DI0, GPIO_MODE_DEF_INPUT); // Pin input/output definition

    gpio_pad_select_gpio(DI1);                    // Defines GPIO functionality for the pin
    gpio_set_direction(DI1, GPIO_MODE_DEF_INPUT); // Pin input/output definition

    gpio_pad_select_gpio(DI2);                    // Defines GPIO functionality for the pin
    gpio_set_direction(DI2, GPIO_MODE_DEF_INPUT); // Pin input/output definition

    gpio_pad_select_gpio(DI3);                    // Defines GPIO functionality for the pin
    gpio_set_direction(DI3, GPIO_MODE_DEF_INPUT); // Pin input/output definition


    gpio_pad_select_gpio(SW1);                    // Defines GPIO functionality for the pin
    gpio_set_direction(SW1, GPIO_MODE_DEF_INPUT); // Pin input/output definition

    gpio_pad_select_gpio(SW2);                    // Defines GPIO functionality for the pin
    gpio_set_direction(SW2, GPIO_MODE_DEF_INPUT); // Pin input/output definition
}

void ReadInput_Main()
{   
    level1 = gpio_get_level(DI0);  //BOTAO EMERGENCIA
    level2 = gpio_get_level(DI1);  //LED AZUL (RESET)
    level3 = gpio_get_level(DI2);  //LED VERDE-ON(MODO DESLIGADO)
    level4 = gpio_get_level(DI3);  //CHAVE SELETORA
    sw1 = gpio_get_level(SW1); 
    sw2 = gpio_get_level(SW2); 

    //_________________________________________________MÁQUINA DE ESTADOS__________________________________________________________

   if((level1==1) && (flag2==0))

    {   
        //ESTADO_M  = "Emergencia";
        levelled = 1;
        flag = 1;
        levelatual = levelled;
        flagatual = flag;
        
    }

    else if((level1==0)  && (level2==1) && (level3==1) && (level4==1)  && (flag2==0) && (flag==1))
    {
        //ESTADO_M  = 'AGUARDANDO RESET';
        levelled = 2;
        flag = 2;
        levelatual = levelled;
        flagatual = flag-1;
    }

    else if((level1==0) && (level2==0) && (level3==1) && (level4==1) && (flag==2) && (flag2==0))
    {
       //ESTADO_M = 'DESLIGADO';
        levelled = 3;
        flag = 3;
        levelatual = levelled;
        flagatual = flag-1;

    }







    else if((level1==0) && (level2==1) && (level3==0) && (level4==1) && (flag==3) && (flag2==0))
    {
       // ESTADO_M = 'AGUARDANDO OPERACAO';
        levelled = 4;
        flag = 4;
        levelatual = levelled;
        flagatual = flag-1;
    }


    else if(((level1==0) && (level2==1) && (level3==1) && (level4==1) && (flag==4) && (flag2==0)))
    {
        levelled = 3;
        flag=3;
        levelatual = levelled;
        flagatual = flag+1;
    }





    else if((level1==0) && (level2==1) && (level3==0) && (level4==0) && (flag==4) && (flag2==0))
    {
       // ESTADO_M = 'AGUARDANDO OPERACAO';
        levelled = 5;
        flag = 3;
        levelatual = levelled;
        //flagatual = flag+1;

    }

    else{
        levelled = levelatual;
    }

    //_________________________________________________ MODO CARNAVAL ____________________________________________________________
   
   
    if(sw1==0 && levelled!=6) 
    {   
        for(int i = 0; i<20; i++)
        {
            vTaskDelay(100/portTICK_RATE_MS);
            count1++;
            if(count1>=20)
            {
            levelled = 6; 
            levelatual = levelled;
            flag2 = 1;
           
            //ESTADO_M = 'CARNAVAL';
            }
        }
        
    }
    else if(sw1==1)
    {
        count1=0;
    }
    /*

    if(levelled==6 && sw1==1)
    {
        flag3 = 1;
    }

    if(sw1==0 && levelled==6 && flag3 ==1){
        denominador = denominador + 1;
        vTaskDelay(800/portTICK_RATE_MS);
    }   */

    
    if(sw2==0) 
    {   
       
        levelled = 0; 
        levelatual = levelled;
        flag = flagatual;
        flag2 = 0;
        flag3 = 0 ;
        count1 = 0;
        count2 = 0;
        
        vTaskDelay(100/portTICK_RATE_MS);
         
    }
  
}
