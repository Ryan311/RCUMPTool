#pragma once

#include "rtk_socket.h"
#include "rtsocket_error.h"

typedef   unsigned char				uint8_t;
typedef   unsigned short int		uint16_t;
typedef   unsigned int				uint32_t;

typedef   char				int8_t;
typedef   short int			int16_t;
typedef   int				int32_t;

//typedef	RTKSOCKET		SOCKET;
typedef		RTK_SOCKADDR_BT_USER	RTK_SOCKADDR_BTH;
//typedef	RTK_SOCKADDR		SOCKADDR;

bool RtkInitWSALib();
bool RtkCleanWSALib();

bool RtkCreateSocket(RTKSOCKET* pSocketName);
void _RtkCloseSocket(RTKSOCKET SocketName);

bool RtkSocketConnect(RTKSOCKET SocketName,RTK_SOCKADDR_BTH* pBthAddr);
bool RtkSocketBindAndGet(RTKSOCKET SocketName,RTK_SOCKADDR_BTH* pBthAddr);
bool RtkSocketListen(RTKSOCKET SocketName);
bool RtkSocketGetOpt(RTKSOCKET SocketName);
int32_t RtkSocketSend(RTKSOCKET SocketName, uint8_t* buffer, uint32_t len);
int32_t RtkSockRecv(RTKSOCKET SocketName, uint8_t* buffer, uint32_t len);

bool RtkSocketSetTime(RTKSOCKET SocketName,int32_t timeout);
RTKSOCKET RtkSocketAccept(RTKSOCKET SocketName,RTK_SOCKADDR *from);//???

bool RtkSocketSetMode(RTKSOCKET SocketName,	RT_U16 iProfileType, bool bToAMP, RT_U64 iFileLength);



