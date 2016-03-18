#ifndef __LE_BASIC_H
#define __LE_BASIC_H

/**

Copyright (c) Realtek Corporation. All rights reserved.

\file    le_basic.h

\brief   LE basic functions.

*/


#include <assert.h>
#include "os_abstractor.h"

#define TRUE  1
#define FALSE 0

// Definition

// Mask and shift
#define LE_BYTE_MASK    0xff
#define LE_BYTE_SHIFT   8

// Constant
#define LE_BIT_NUM_PER_BYTE   8


// 0-bit data
#define LE_0_BYTE_LEN   0

// 8-bit unsigned integer
#define LE_1_BYTE_LEN   1

// 16-bit unsigned integer
#define LE_2_BYTE_LEN   2

// 24-bit unsigned integer
#define LE_3_BYTE_LEN   3
typedef struct
{
	unsigned char Value[LE_3_BYTE_LEN];
}
LE_3_BYTE;

// 48-bit unsigned integer
#define LE_6_BYTE_LEN   6
typedef struct
{
	unsigned char Value[LE_6_BYTE_LEN];
}
LE_6_BYTE;

// 56-bit unsigned integer
#define LE_7_BYTE_LEN   7
typedef struct
{
	unsigned char Value[LE_7_BYTE_LEN];
}
LE_7_BYTE;

// 64-bit unsigned integer
#define LE_8_BYTE_LEN   8
typedef struct
{
	unsigned char Value[LE_8_BYTE_LEN];
}
LE_8_BYTE;

// 96-bit unsigned integer
#define LE_12_BYTE_LEN   12
typedef struct
{
	unsigned char Value[LE_12_BYTE_LEN];
}
LE_12_BYTE;

// 128-bit unsigned integer
#define LE_16_BYTE_LEN   16
typedef struct
{
	unsigned char Value[LE_16_BYTE_LEN];
}
LE_16_BYTE;


// UUID
typedef enum
{
	LE_UUID_BIT_16,
	LE_UUID_BIT_128,
}
LE_UUID_TYPE;

typedef struct
{
	LE_UUID_TYPE Type;

	union
	{
		unsigned short Bit16;
		LE_16_BYTE Bit128;
	}
	Value;
}
LE_UUID;


// LE executing status
typedef enum
{
	LE_SUCCESS,
	LE_ERROR_L2CAP,
	LE_ERROR_L2CAP_TIMEOUT,
	LE_ERROR_SM,
	LE_ERROR_ATT,
	LE_ERROR_GATT,
	LE_ERROR_OS,
	LE_ERROR_NO_DATA_AVAIABLE
}LE_STATUS;


// LE executing status
typedef enum
{
	LE_FALSE = 0,
	LE_TRUE  = 1,
}
LE_BOOL;

#ifdef __cplusplus
extern "C" {
#endif

// Buffer data setting functions
void
__cdecl
LeBufferSetUchar(
	unsigned char *pBuffer,
	unsigned char Value
	);

void
__cdecl
LeBufferSetUshort(
	unsigned char *pBuffer,
	unsigned short Value
	);

void
 __cdecl
LeBufferSetUlong(
	unsigned char *pBuffer,
	unsigned long Value
	);

void
__cdecl
LeBufferSetUuidBit16(
	unsigned char *pBuffer,
	LE_UUID Uuid
	);

void
__cdecl
LeBufferSetUuidBit128(
	unsigned char *pBuffer,
	LE_UUID Uuid
	);

void
__cdecl
LeBufferSetUuid(
	unsigned char *pBuffer,
	LE_UUID Uuid
	);

void
__cdecl
LeBufferSetBytes(
	unsigned char *pBuffer,
	unsigned char *pBytes,
	unsigned long ByteNum
	);

unsigned long
__cdecl    
LeBufferSetString(
	unsigned char *pBuffer,
	const char *pString
	);


// Buffer data getting functions
void
__cdecl
LeBufferGetUchar(
	unsigned char *pBuffer,
	unsigned char *pValue
	);

void
__cdecl
LeBufferGetUshort(
	unsigned char *pBuffer,
	unsigned short *pValue
	);

void
__cdecl
LeBufferGetUlong(
	unsigned char *pBuffer,
	unsigned long *pValue
	);

void
__cdecl
LeBufferGetUuidBit16(
	unsigned char *pBuffer,
	LE_UUID *pUuid
	);

void
__cdecl
LeBufferGetUuidBit128(
	unsigned char *pBuffer,
	LE_UUID *pUuid
	);

void
__cdecl
LeBufferGetUuid(
	unsigned char *pBuffer,
	LE_UUID *pUuid,
	unsigned long UuidLen
	);

void
__cdecl
LeBufferGetBytes(
	unsigned char *pBuffer,
	unsigned char *pBytes,
	unsigned long ByteNum
	);


// Variable-length data tools
int
__cdecl
LeVariableLengthDataIsEqualTo(
	unsigned char *pBytesA,
	unsigned char *pBytesB,
	unsigned long ByteNum
	);


// UUID tools
void
__cdecl
LeUuidSetBit16(
	LE_UUID *pUuid,
	unsigned short Value
	);

void
__cdecl
LeUuidSetBit128(
	LE_UUID *pUuid,
	LE_16_BYTE Value
	);

void
LeUuidGetLen(
	LE_UUID Uuid,
	unsigned long *pUuidLen
	);

void
__cdecl
LeUuidConvertToBit128(
	LE_UUID Uuid,
	LE_UUID *pUuidBit128
	);

void
__cdecl
LeUuidConvertToBit16(
	LE_UUID Uuid,
	LE_UUID *pUuidBit16
	);

int
__cdecl
LeUuidIsEqualTo(
	LE_UUID UuidA,
	LE_UUID UuidB
	);

int
__cdecl
LeUuidIsEqualToBit16(
	LE_UUID UuidA,
	unsigned short Value
	);

int
__cdecl
LeUuidIsEqualToBit128(
	LE_UUID UuidA,
	LE_16_BYTE Value
	);



#ifdef __cplusplus
}
#endif



#endif
