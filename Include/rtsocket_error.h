/*++
Copyright (c) Realtek Corporation. All rights reserved.

Module Name:
	rtsocket_error.h
+*/

#ifndef RTKBASEERR
#define RTKBASEERR 10000

/// RTK_SOCKET ERRORS
//#define RTK_SOCKET_ERROR            (-1)
//#define RTK_INVALID_SOCKET          0


///defined by xrh, used for socket function error

#define RTKEOUTBUFFERERROR       100L///<call DeviceIoControl success, but return buffer size is wrong
#define RTKEIOCTLRETURNFAIL     	101L///<call DeviceIoControl error
#define RTKECREATEEVENTFAIL     	102L///<call CreateEvent() function failed

#define RTKNOTINITIALISED  		103L///<not call RtkBtStartup() to initialize socket 
#define RTKEAFNOSUPPORT     		104L///<address family not supported
#define RTKESOCKTNOSUPPORT     	105L///not supported socket type
#define RTKEPROTOTYPE    		106L///proto type not supported
#define RTKEMFILE     				107L///socket has reached maximum number
#define RTKENOBUFS    			108L///memory insufficient
#define RTKENOTSOCK     			109L///it is not a socket
#define RTKEINVAL     				110L /// invalid parameter

#define RTKEISCONN  				111L///
#define RTKENOTCONN     			112L///
#define RTKESHUTDOWN     		113L///socket has been shut down
#define RTKVERNOTSUPPORTED     	114L /// version not supported
#define RTKEPROCLIM     			115L
#define RTKEFAULT     			116L
#define RTKSYSNOTREADY     		117L///system hasnot ready



#define RTKEREMOTEDISCONN     	118L/// remote socket has been disconnected
#define RTKETIMEOUT                      119L///Socket operation timeout, used by recv function

#endif // defined(RTKBASEERR)

