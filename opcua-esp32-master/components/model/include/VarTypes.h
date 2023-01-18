/**
  @file: VarTypes.h
  @class: VarTypes

  @version: v0r1
  @author: gabriels
  @date: 03/10/2015

  @brief: Sets the standard variables used in the application and conversion variables

  @since	VER		DATE			COMMENTS
  @since	v0r1	03/10/2015 		Class creation
 */

#ifndef VARTYPES_H_
#define VARTYPES_H_



/***********************************************************************
 * Global typedefs
 **********************************************************************/

/* Var definitions */
typedef char Int8;
typedef unsigned char uInt8;
typedef	unsigned short int uInt16;
typedef	signed short int Int16;
typedef	unsigned long int uInt32;
typedef	signed long int Int32;
typedef Int8 Bool8;
#define true	(Bool8)1
#define false	(Bool8)0

/* Pointer definitions */
typedef void*	Handle;
typedef char* pInt8;
typedef unsigned char* puInt8;
typedef	unsigned short int* puInt16;
typedef	signed short int* pInt16;
typedef	unsigned long int* puInt32;
typedef	signed long int* pInt32;
typedef pInt8 pBool8;





#endif /* VARTYPES_H_ */
