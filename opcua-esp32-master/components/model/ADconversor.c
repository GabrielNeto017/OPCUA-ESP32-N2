//Author: Gabriel Luiz B. Neto

#include <driver/adc.h>
#include <stdio.h>

int val, val2 = 0;



void ADC_init(){
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_DB_11);
    adc1_config_channel_atten(ADC1_CHANNEL_3,ADC_ATTEN_DB_11);
}


void leituraAI0(){
    //resolution 2^12 bits

     val = (4095 - adc1_get_raw(ADC1_CHANNEL_0))*1.1716;
    
   
}

void leituraAI1(){

     val2 = (4095 - 640 - adc1_get_raw(ADC1_CHANNEL_3))*1.28;

     if(val2<0)val2=0;
     if(val2>2950)val2=val2*1.1068;
 
}