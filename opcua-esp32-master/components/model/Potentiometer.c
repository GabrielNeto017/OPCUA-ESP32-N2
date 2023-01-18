

#include <stdio.h>
#include "driver/adc.h"
#include "Potentiometer.h"
#include "model.h"

int concentracao = 0.;
float media = 0.;
int val = 0;

void configureAnalRead()
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11);
}

float ReadVoltage()
{

    media = 0;
    for (int x = 0; x < 10; x++)
    {
        val = adc1_get_raw(ADC1_CHANNEL_6);
        
        media = media + val;
    }

    concentracao=media/10;

    return concentracao;
}