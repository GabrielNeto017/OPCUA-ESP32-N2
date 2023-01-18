/**
  @file: Defaults.h
  @class: Defaults

  @version: v0r1
  @author: gabriels
  @date: 05/10/2015

  @brief: Define defaults app parameters


  @since	VER		DATE			COMMENTS
  @since	v0r1	05/10/2015 		Class creation
 */

#ifndef DEFAULTS_H_
#define DEFAULTS_H_


/*********************************************************
    Public type.
*********************************************************/

/*********************************************************
    Public Definitions.
*********************************************************/
#define DEFAULTS_FIRST_INIT_CONST				0x9966
#ifdef COMMON_USE_DEFAULT_MAC_SERIAL
#define DEFAULTS_MAC_0							0x12
#define DEFAULTS_MAC_1							0x34
#define DEFAULTS_MAC_2							0x56
#define DEFAULTS_MAC_3							0x78
#define DEFAULTS_MAC_4							0x90
#define DEFAULTS_MAC_5							0x12
#define DEFAULTS_SERIAL_NUMBER_0				0x00
#define DEFAULTS_SERIAL_NUMBER_1				0x01
#define DEFAULTS_SERIAL_NUMBER_2				0x02
#define DEFAULTS_SERIAL_NUMBER_3				0x03
#define DEFAULTS_SERIAL_NUMBER_4				0x04
#define DEFAULTS_SERIAL_NUMBER_5				0x05
#define DEFAULTS_SERIAL_NUMBER_6				0x06
#define DEFAULTS_SERIAL_NUMBER_7				0x07
#define DEFAULTS_SERIAL_NUMBER_8				0x08
#define DEFAULTS_SERIAL_NUMBER_9				0x09
#define DEFAULTS_SERIAL_NUMBER_10				0x0a
#define DEFAULTS_SERIAL_NUMBER_11				0x0b
#define DEFAULTS_SERIAL_NUMBER_12				0x0c
#define DEFAULTS_SERIAL_NUMBER_13				0x0d
#define DEFAULTS_SERIAL_NUMBER_14				0x0e
#define DEFAULTS_SERIAL_NUMBER_15				0x0f
#endif
#define DEFAULTS_IP_0							192
#define DEFAULTS_IP_1							168
#define DEFAULTS_IP_2							1
#define DEFAULTS_IP_3							150
#define DEFAULTS_GATEWAY_0						192
#define DEFAULTS_GATEWAY_1						168
#define DEFAULTS_GATEWAY_2						1
#define DEFAULTS_GATEWAY_3						1
#define DEFAULTS_MASK_0							255
#define DEFAULTS_MASK_1							255
#define DEFAULTS_MASK_2							255
#define DEFAULTS_MASK_3							0
#define DEFAULTS_PORT							1234

#define DEFAULTS_ESE							0
#define DEFAULTS_SRE							0

#define DEFAULTS_FREQ							60
#define DEFAULTS_MEASURE_MODE					0	/* 0 - star */
#define DEFAULTS_VOLTAGE_FACTOR_1				677
#define DEFAULTS_VOLTAGE_FACTOR_2				50000
#define DEFAULTS_CURRENT_FACTOR_1				5937
#define DEFAULTS_CURRENT_FACTOR_2				1000000

#define DEFAULTS_OFFSET_VOLTAGE					0
#define DEFAULTS_GAIN_VOLTAGE					10000
#define DEFAULTS_OFFSET_CURRENT					0
#define DEFAULTS_GAIN_CURRENT					10000


/*********************************************************
    Public functions
*********************************************************/



#endif /* DEFAULTS_H_ */
