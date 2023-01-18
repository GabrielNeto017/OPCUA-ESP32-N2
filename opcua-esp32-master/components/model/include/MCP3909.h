/**
  @file: MCP3909.h
  @class: MCP3909

  @version: v0r1
  @author: gabriels
  @date: 14/02/2016

  @brief: This class implement drive with MCP3909

  @since	VER		DATE		COMMENTS
  @since	v0r1	14/02/2016	Class creation

 */

#ifndef MEASURE_MCP3909_H_
#define MEASURE_MCP3909_H_

#include "driver/spi_master.h"

struct MCP3909_reading {
    int status;
    int tensao;
    int corrente;
};




struct MCP3909_reading MCP3909_read();



//void MCP3909_init();
void mcpSetGain(unsigned char ganho);
void mcpInitSPI();
void EXECUTE_SPI();
void ARMAZENA_BITS();

void Analisador();


#endif /* MEASURE_MCP3909_H_ */
