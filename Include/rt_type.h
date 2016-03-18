/*++

Copyright (c) Realtek Corporation. All rights reserved.

Module Name:
	rttype.h

Abstract:
	Mediate data structures referenced by stack project. 

Major Change History:
	  When             Who	       What
	----------	------	----------------------------------------------
	2010-06-04	W.Bi  	Created, Only basic types added.

Notes:
	Add more extended types here.
	32 bit int is assumed, so take care with integer overflow
--*/

/** 
\file     rttype.h
\brief   Define basic type alias. We would better use alias since we could only change this file if basic type changes
*/

#ifndef _INC_RTTYPE_H
#define _INC_RTTYPE_H

#ifndef CONST
#define CONST const
#endif

#ifndef EXTERN
#define EXTERN extern 
#endif

#ifndef INLINE
#define INLINE  __inline
#endif

#ifndef STATIC
#define STATIC static
#endif

#ifndef NULL
#define NULL    ((void*)0)
#endif
//two indication for function parameters direction
#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif


//two value for RT_BOOL types, if TRUE is not defined use this flag
#ifndef _TRUE
#define _TRUE 1
#endif

#ifndef _FALSE
#define _FALSE 0
#endif


typedef signed char                     RT_BOOL;                        //define own bool type instead platform provided       

typedef void                            RT_VOID, *PRT_VOID;

typedef unsigned char                   RT_U8,    *PRT_U8;
typedef unsigned short                  RT_U16,  *PRT_U16;
typedef unsigned long                   RT_U32,  *PRT_U32;      //long is 32 bit,K.C
typedef unsigned long long				RT_U64,  *PRT_U64;


typedef signed char                     RT_S8,    *PRT_S8;
typedef signed short                    RT_S16,  *PRT_S16;
typedef signed long                     RT_S32,  *PRT_S32; 
typedef signed long long                RT_S64,  *PRT_S64;


//bit32 and above is not defined.
#define BIT(_I)   (RT_U32)(1<<(_I))

#define BIT0		0x00000001
#define BIT1		0x00000002
#define BIT2		0x00000004
#define BIT3		0x00000008
#define BIT4		0x00000010
#define BIT5		0x00000020
#define BIT6		0x00000040
#define BIT7		0x00000080
#define BIT8		0x00000100
#define BIT9		0x00000200
#define BIT10		0x00000400
#define BIT11		0x00000800
#define BIT12		0x00001000
#define BIT13		0x00002000
#define BIT14		0x00004000
#define BIT15		0x00008000
#define BIT16		0x00010000
#define BIT17		0x00020000
#define BIT18		0x00040000
#define BIT19		0x00080000
#define BIT20		0x00100000
#define BIT21		0x00200000
#define BIT22		0x00400000
#define BIT23		0x00800000
#define BIT24		0x01000000
#define BIT25		0x02000000
#define BIT26		0x04000000
#define BIT27		0x08000000
#define BIT28		0x10000000
#define BIT29		0x20000000
#define BIT30		0x40000000
#define BIT31		0x80000000

/**
	define RT Status Code.
	Add more status if neccessary
*/    
typedef enum _RT_STATUS{
	RT_STATUS_SUCCESS =0,
	RT_STATUS_FAILURE =1,
	RT_STATUS_PENDING =2,
	RT_STATUS_TIMEOUT =3,								///< Time out error
	RT_STATUS_RESOURCE =4,								///< error with resource limitation
	RT_STATUS_ALREADY_EXIST = 5,						///< Already exist error.
	RT_STATUS_INVALID_PARAMETER =6,						///< Input Parameter error
	RT_STATUS_NOT_IMPLEMENT_YET =7,						///< This function has not implement yet 
	RT_STATUS_NO_SUCH_FILE_DIRECTORY = 8,				///< Operation object not exist
	RT_STATUS_NOT_SUPPORT = 9,							///< Operation not support yet
	RT_STATUS_HOST_UNREACH = 10,						///< No route to host
	RT_STATUS_CONNECT_RESET = 11,						///< connection reset by peer
	RT_STATUS_NOT_CONNECT = 12, 						///< transport endpoint is not connected
	RT_STATUS_CONNECT_REFUSED = 13,						///< Connection refused
	RT_STATUS_PROCEDURE_FAIL = 14,
	RT_STATUS_RESPONSE_ERROR = 15,
	RT_STATUS_MAX_ERROR = 0x7f							///< status code max, not expected to see
}RT_STATUS,*PRT_STATUS;

/**
	Bluetooth address definition.
	Since it may embeded in other structure, so pack it, and thus sizeof() will equal to 6
*/
#define BT_ADDR_LEN     6

#pragma pack(push, 1)
typedef struct {
	RT_U8   Address[BT_ADDR_LEN];
}BT_ADDR;
#pragma pack(pop)

//#define BTADDR_ANY   (&(BT_ADDR) {{0, 0, 0, 0, 0, 0}})
//#define BTADDR_LOCAL (&(BT_ADDR) {{0, 0, 0, 0xff, 0xff, 0xff}})

#define BT_FMT "%02x:%02x:%02x:%02x:%02x:%02x"
#define BT_ARG(_x) ((RT_U8*)(_x))[5],((RT_U8*)(_x))[4],((RT_U8*)(_x))[3],((RT_U8*)(_x))[2],((RT_U8*)(_x))[1],((RT_U8*)(_x))[0]

EXTERN RT_S32 
BaCmp(BT_ADDR *Ba1, BT_ADDR *Ba2);


EXTERN RT_VOID
BaCpy(BT_ADDR *Dest, BT_ADDR *Src);

EXTERN CONST RT_U8* GetStringFromHciErrorCode(RT_U8 Index);
#endif //__INC_RTTYPE_H
