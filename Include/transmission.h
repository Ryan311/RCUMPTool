#ifndef __TRANSMISSION_LINE_H
#define __TRANSMISSION_LINE_H

/**

Copyright (c) Realtek Corporation. All rights reserved.

\file    transmission.h

\brief   Virtual transmission line functions.

*/

//#include <stdio.h>

//#include <winsock2.h>
//#include <ws2tcpip.h>

//#include "att_api.h"
//#include "RTKSocket.h"
//#pragma comment (lib, "Ws2_32.lib")
//#pragma comment (lib, "Mswsock.lib")
//#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN    512
#define DEFAULT_PORT      "27016"



//#ifdef LE_CORE_ROLE_REMOTE
//#define IP_ADDR_TARGET   "192.168.1.1"
//#else
//#define IP_ADDR_TARGET   "192.168.1.2"
//#endif




// Access function
int
IpSendData(
        void * pSockHandle,
	unsigned char *pData,
	unsigned long DataSize
	);

int
IpReceiveData(
        void * pSockHandle,
	unsigned char *pData,
	unsigned long *pDataSize
	);





#endif