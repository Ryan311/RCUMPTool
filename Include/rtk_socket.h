/*++

Copyright (c) Realtek Corporation. All rights reserved.

Module Name:
     Rtk_Socket.h
     
Abstract:
     socket supported by BT stack
     
Major Change History:
    	     When                    Who                  What
	------------     ------------     ------------------
   	  2010-7-5		    hr&ranhui		created
Notes:
    
--*/
    
/**
	\file	rtk_socket.h
	\brief	 socket interface funcitions
*/


#ifndef _INC_RTK_SOCKET
#define _INC_RTK_SOCKET


#include "rt_type.h"


#define RTK_AF_BTH  32 ///<address family of bluetooth
#define RTK_INVALID_SOCKET  (RTKSOCKET)(~0) ///<SOCKET is a pointer,so NULL is invalid socket
#define RTK_BT_PORT_ANY        ((RT_U32)-1)
#define RTK_SOCKET_ERROR		(-1)
/**************************************************************************************
	structure of socket
***************************************************************************************/

/**
   RTKData definition, which will be used in RtkBtStartup function
    \param wVersion: version 
    \param wHighVersion: version 
    \param Description: a description of the Sockets implementation
    \param SystemStatus: relevant status or configuration information.  
    \param VendorInfo: vendor's information.
*/
#define RTKDESCRIPTION_LEN 64
#define RTKSYS_STATUS_LEN 64
typedef struct _RTKDATA {
  RT_S32  wVersion; 			     
  RT_S32  wHighVersion;		
  RT_S8   Description[RTKDESCRIPTION_LEN+1]; 
  RT_S8  SystemStatus[RTKSYS_STATUS_LEN+1]; 
  RT_S8*  VendorInfo;		                 
}RTKDATA, *PRTKDATA;

///socket protocols
typedef enum _RTK_BT_PROTO{
	RTK_PROTO_HCI,
	RTK_PROTO_L2CAP,
	RTK_PROTO_RFCOMM
}RTK_SOCK_PROTO;

#define RTK_SOL_RFCOMM  RTK_PROTO_RFCOMM
#define RTK_SOL_L2CAP   RTK_PROTO_L2CAP
#define RTK_SOL_HCI   RTK_PROTO_HCI

/// SOCKET OPTIONS
#define RTK_SO_BTH_AUTHENTICATE 0x80000001  //< optlen=sizeof(ULONG), optval = &(ULONG)TRUE/FALSE 
#define RTK_SO_BTH_ENCRYPT      0x00000002  //< optlen=sizeof(ULONG), optval = &(ULONG)TRUE/FALSE
#define RTK_SO_BTH_MTU_IN 0x00000003
#define RTK_SO_BTH_MTU_OUT 0x00000004
#define RTK_SO_BTH_MTU          0x80000007  //< optlen=sizeof(ULONG), optval = &mtu
#define RTK_SO_BTH_MTU_MAX      0x80000008  //< optlen=sizeof(ULONG), optval = &max. mtu
#define RTK_SO_BTH_MTU_MIN      0x8000000a  //< optlen=sizeof(ULONG), optval = &min. mtu


/**
	represents  a  UUID,  used  with  SDP
*/
#pragma pack(push,1)
typedef  struct _RTK_GUID_USER  { 
	RT_U32  Data1 ;
	RT_U16  Data2 ;
	RT_U16  Data3 ;
	RT_U8    Data4 [ 8 ] ;
}RTK_GUID_USER ;

/**
	socket addressing structure used for establishing 
	connections and retrieving connection details
*/
typedef  RT_U64 BT_ADDR_USER;

typedef  struct _RTK_SOCKADDR_BTH_USER  
{
	RT_U16  AddressFamily ;	///<only RTK_AF_BTH is valid
	BT_ADDR_USER  BTAddr ;		///<remote device address or host device address
	RTK_GUID_USER  ServiceClassId ;	///< not used now
	RT_U32  PortNum ;			///< port nubmer 
}RTK_SOCKADDR_BT_USER,*PRTK_SOCKADDR_BT_USER;


typedef struct _RTK_SOCKADDR {
        RT_U16  sa_family;
        RT_S8    sa_data[14];
}RTK_SOCKADDR, *PRTK_SOCKADDR;


#pragma pack(pop)

///type of socket
typedef enum _RTK_SOCK_TYPE {  
	RTK_SOCK_DGRAM = 1,  
	RTK_SOCK_STREAM = 2,  ///<only used in BT
	RTK_SOCK_RAW    = 3,  
	RTK_SOCK_RDM    = 4,  
	RTK_SOCK_SEQPACKET = 5,  
	RTK_SOCK_DCCP   = 6,  
	RTK_SOCK_PACKET = 10 
}RTK_SOCK_TYPE;

///used to shutdown send or recv or both ,used in function RtkShutdown()
typedef enum _RTK_SOCK_DIRECTION{
	RTK_SD_NONE=1,
	RTK_SD_SEND,
	RTK_SD_RECV,
	RTK_SD_BOTH
}RTK_SOCK_DIRECTION;

///mode of socket: blocking or non-blocking
typedef enum _RTK_SOCK_OPT{
	SOCK_BLOCK=1,
	SOCK_NOBLOCK
}RTK_SOCK_OPT;

/**
	IOCTL CMD type
*/
typedef enum _SOCK_IOCTL_OPT{
    IOCTL_CMD_ECHO = 1,
    IOCTL_CMD_LOCAL_BUSY = 2,
	IOCTL_CMD_RESERVED
}SOCK_IOCTL_OPT;

typedef
RT_VOID
(*RTK_SOCK_CALLBACK_FN)(RT_VOID);

typedef enum _RTK_SOCKET_EVENT_CODE{
	RTK_FD_CLOSE= 0x01,
	RTK_FD_READ = 0x02,
	RTK_FD_WRITE = 0x04
}RTK_SOCKET_EVENT_CODE;


typedef  RT_U32 RTKSOCKET;

#define RTK_FD_SETSIZE 64
typedef struct _RTK_FDSET
{
	RT_U32 Fd_count;
	RTKSOCKET Fd_array[RTK_FD_SETSIZE];
}RTK_FDSET;

///remove descriptor fd from set
#define RTK_FD_CLR(fd, set)  do { \
			RT_U32 __i; \
			for(__i = 0; __i < (RTK_FDSET*)set->Fd_count; __i++) { \
				if((RTK_FDSET*)set->Fd_array[__i] == fd) { \
					while(__i < (RTK_FDSET*)set->Fd_count-1) { \
						(RTK_FDSET*)set->Fd_array[__i] = \
						(RTK_FDSET*)set->Fd_array[__i+1]; \
						__i++; \
						} \
						(RTK_FDSET*)set->Fd_count--; \
						break; \
					} \
				} \
}while(0) 
///add descriptor fd to set
#define RTK_FD_SET(fd, set) do { \
	RT_U32 __i; \
	for(__i = 0; __i < (RTK_FDSET*)set->Fd_count; __i++) { \
		if((RTK_FDSET*)set->Fd_array[i] == fd) { \
			break; \
			} \
		} \
		if(__i == (RTK_FDSET*)(set)->Fd_count ) { \
			if((RTK_FDSET*)(set)->Fd_count < RTK_FD_SETSIZE) {\
				(RTK_FDSET*)(set)->Fd_array[__i] =  (fd); \
				(RTK_FDSET*)(set)->Fd_count++; \
			} \
		}\
}while(0)

///Initialize set to be a null set
#define RTK_FD_ZERO(set)  (((RTK_FDSET*)(set))->Fd_count = 0)

///nonzero if s is a member of set
#define RTK_FD_ISSET(s, set) 

///timeval structure for select
typedef struct _RTK_TIMEVAL{
	RT_U32 tv_sec;
	RT_U32 tv_usec;
}RTK_TIMEVAL;

/// timeout setting for RtkRecv function, upper layer can set this value by invoke "RtkSetSockOpt"
#define USER_MODE_OPT  1
#define SET_RECV_TIMEOUT_OPT  1

/************************************************************************/
/* RTK SOCKET functions                                                                     */
/************************************************************************/

/**
	allocate an unused socket.
	\param [IN]	AddressFamily	<RT_S32>	:The address family specification,only RTK_AF_BTH is valid 
	\param [IN]	SocketTypeThe	<RT_S32>	:type specification for the new socket,only SOCK_STREAM is valid
	\param [IN]	Protocol		<RT_S32>	:protocol to be used,l2cap,hci,rfcomm,etc.
	\return  success returns new socket, fail return RTK_INVALID_SOCKET(NULL)
*/
#if defined(__cplusplus)
extern "C"
{
#endif

RTKSOCKET
__cdecl
RtkSocket(
	IN	RT_S32 AddressFamily,
	IN	RT_S32 SocketType,
	IN	RT_S32 Protocol
);

/**
	controls the I/O mode of a socket
	\param [IN]	SockHandle	<RTKSOCKET>:	A descriptor identifying a socket
	\param [IN]	Cmd		<RT_S32>:	A command to perform on the socket sock
	\param [OUT] Argp	<PRT_U32>:	A pointer to a parameter for cmd
	\return  success returns 0, fail return RTK_SOCKET_ERROR(-1)
*/
RT_S32
__cdecl
RtkIoctlSocket(
	IN	RTKSOCKET SockHandle,
	IN  RT_S32	Cmd,
	IN OUT	PRT_U32	Argp
);

/**
	establish  an  outgoing  connection with  the  specified  device
	\param[IN]	SockHandle<RTKSOCKET>:	identify an unconnected socket
	\param[IN]	SockAddr<PRTK_SOCKADDR_BT_USER>:	a pointer to the sockaddr structure to which the connection should be established
	\param[IN]	AddrLen<RT_S32>:	The length, in bytes, of the sockaddr structure pointed to by the name parameter
	\return  success returns 0, fail return RTK_SOCKET_ERROR(-1)
*/
RT_S32
__cdecl
RtkConnect(
	IN	RTKSOCKET SockHandle,
	IN	CONST PRTK_SOCKADDR SockAddr,
	IN	RT_S32 AddrLen 
);

/**
	associates a local address with a socket,the port number can be RTK_BT_PORT_ANY to use a dynamically assigned port
	\param[IN]	SockHandle		<RTKSOCKET>:			Descriptor identifying an unbound socket
	\param[IN]	SockAddr	<PRTK_SOCKADDR_BTH>:	Address to assign to the socket from the sockaddr_bth structure
	\param[IN]	AddrLen		<RT_S32>:				Length of the value in the sockaddr parameter, in bytes
	\return  success returns 0, fail return RTK_SOCKET_ERROR(-1)
*/
RT_S32
__cdecl
RtkBind(
	IN	RTKSOCKET SockHandle,
	IN	CONST PRTK_SOCKADDR SockAddr,
	IN	RT_S32 AddrLen
);

/**
	switches the socket into listening mode, and can only be invoked after a call to bind
	\param[IN]	SockHandle	<RTKSOCKET>:	A descriptor identifying a bound, unconnected socket
	\param[IN]	BackLog	<RT_S32>:		The maximum length of the queue of pending connections
	\return  success returns 0, fail return RTK_SOCKET_ERROR(-1)
*/
RT_S32
__cdecl
RtkListen(
	IN	RTKSOCKET SockHandle, 
	IN	RT_S32 BackLog 
);

/**
	waits for an incoming connection, and can only be invoked after a call to listen
	\param[IN]	SockHandle		<RTKSOCKET>:	A descriptor that identifies a socket that has been placed in a listening state with the listen function
	\param[IN]	SockAddr	<PRTK_SOCKADDR_BTH>:	An optional pointer to a buffer that receives the address of the 
							connecting entity, as known to the communications layer
	\param[IN OUT]	AddrLen		<PRT_S32>:	An optional pointer to an integer that contains the length of structure pointed to by the addr parameter
	\return  success returns new socket for connection, fail return RTK_INVALID_SOCKET(NULL)
*/
RTKSOCKET
__cdecl
RtkAccept(
	IN	RTKSOCKET SockHandle,
	OUT	PRTK_SOCKADDR SockAddr,
	IN OUT	PRT_S32 AddrLen
);

/**
	sends data on a connected socket.
	\param[IN]	SockHandle	<RTKSOCKET>:	A descriptor identifying a connected socket
	\param[IN]	Buffer	<PRT_VOID>:		A pointer to a buffer containing the data to be transmitted
	\param[IN]	BufLen	<RT_S32>:		The length, in bytes, of the data in buffer pointed to by the buffer parameter
	\param[IN]	flags	<RT_S32>:		A set of flags that specify the way in which the call is made
	\return  success returns bytes sended ,otherwisel return RTK_SOCKET_ERROR(-1)
*/
RT_S32
__cdecl
RtkSend(
	IN	RTKSOCKET SockHandle,
	IN	CONST PRT_VOID Buffer,
	IN	RT_S32 BufLen,
	IN	RT_S32 flags
);

/**
	receives data from a connected socket
	\param[IN]	SockHandle	<RTKSOCKET>:	A descriptor identifying a bound socket
	\param[OUT] Buffer	<PRT_VOID>:		A buffer for the incoming data
	\param[IN]	BufLen	<RT_S32>:		The length, in bytes, of the buffer pointed to by the buffer parameter
	\param[IN]	flags	<RT_S32>:		A set of flags that influences the behavior of this function
	\return  success returns 0, fail return RTK_SOCKET_ERROR(-1)
*/
RT_S32
__cdecl
RtkRecv(
	IN	RTKSOCKET SockHandle,
	OUT	PRT_VOID Buffer,
	IN	RT_S32 BufLen,
	IN	RT_S32 Flags
);

/**
	closes an existing socket.
	\param[IN]	SockHandle	<RTKSOCKET>:	A descriptor identifying the socket to close
	\return  success returns 0, fail return RTK_SOCKET_ERROR(-1)
*/
RT_S32
__cdecl
RtkCloseSocket(
	IN	RTKSOCKET SockHandle
);

/**
	disables sends or receives on a socket.
	\param[IN]	SockHandle	<RTKSOCKET>:	A descriptor identifying a socket.
	\param[In]	How		<RT_S32>:		A flag that describes what types of operation will no longer be allowed. 
	\return  success returns 0, fail return RTK_SOCKET_ERROR(-1)										Possible values for this flag are listed in the Winsock2.h header file.
*/
RT_S32
__cdecl
RtkShutDown(
  	IN    RTKSOCKET SockHandle,
  	IN    RT_S32  How
);

/**
	retrieves the standard host name for the local computer.
	\param[IN]	SockHandle	<RTKSOCKET>:	The length, in bytes, of the buffer pointed to by the hostname parameter
	\param[OUT]	HostName	<PRT_S8>:	A pointer to a buffer that receives the local host name
	\param[IN]	NameLen		<PRT_U8>:	The length, in bytes, of the buffer pointed to by the hostname parameter
	\return  success returns 0, fail return RTK_SOCKET_ERROR(-1)
*/
RT_S32
__cdecl
RtkGetHostName(
	IN    RTKSOCKET SockHandle,
  	OUT RT_U8 HostName[248],
  	OUT   PRT_U8  NameLen
);

/**
	retrieves the address of the peer to which a socket is connected
	\param[IN]		SockHandle	<RTKSOCKET>:				A descriptor identifying a connected socket.
	\param[OUT]		PeerName	<PRTK_SOCKADDR_BTH>:	The 248 byte that receives the name of the peer
	\param[IN OUT]	NameLen	<PRT_S8>:					A pointer to the size, in bytes, of the peername parameter
	\return  success returns 0, fail return RTK_SOCKET_ERROR(-1)
*/
RT_S32
__cdecl
RtkGetPeerName(
  	IN     RTKSOCKET SockHandle,
 	OUT RT_U8 PeerName[248],
  	OUT  PRT_U8 Namelen
);

/**
	retrieves the standard host name for the local computer.
	\param[IN]	SockHandle	<RTKSOCKET>:	The length, in bytes, of the buffer pointed to by the hostname parameter
	\param[OUT]	HostName	<PRT_S8>:	A pointer to a buffer that receives the local host name
	\param[IN]	NameLen		<PRT_U8>:	The length, in bytes, of the buffer pointed to by the hostname parameter
	\return  success returns 0, fail return RTK_SOCKET_ERROR(-1)
*/
RT_S32
__cdecl
RtkGetSockName(
	IN    RTKSOCKET SockHandle,
  	OUT RTK_SOCKADDR* Name,
  	IN OUT   RT_S32  *NameLen
);


/**
	retrieves the standard host name for the local computer.
	\param[OUT]	HostAddr	<BT_ADDR_USER*>:	A pointer to a buffer that receives the local host name
	\return  success returns 0, fail return RTK_SOCKET_ERROR(-1)
*/
RT_S32
__cdecl
RtkGetHostAddress(
  	OUT BT_ADDR_USER *HostAddr
);

/**
	retrieves the address of the peer to which a socket is connected
	\param[IN]		SockHandle	<RTKSOCKET>:				A descriptor identifying a connected socket.
	\param[OUT]		Peeraddr	<BT_ADDR_USER*>:	The SOCKADDR_BTH structure that receives the address of the peer
	\return  success returns 0, fail return RTK_SOCKET_ERROR(-1)
*/
RT_S32
__cdecl
RtkGetPeerAddress(
  	IN    RTKSOCKET SockHandle,
 	OUT BT_ADDR_USER *Peeraddr
);

/**
	Notify user remote socket has disconnected
	Process FD_CLOSE event
	\param[IN]		Sock	<RTKSOCKET>:		Socket handle
	\param[IN]		EventCallback	<PRT_VOID>:			Callback functions when specified socket events happens
	\param[IN]		EventCode	<RTK_SOCKET_EVENT_CODE*>:	Event codes
*/
RT_S32
__cdecl
RtkSocketAsyncSelect(
	IN RTKSOCKET SockHandle,
	IN PRT_VOID EventCallback,
	IN RTK_SOCKET_EVENT_CODE EventCode
);

/**
	The RtkSelect function determines the status of one or more sockets, 
	waiting if necessary, to perform synchronous I/O.
	\param[IN]		nfds	<RT_S32>:		Reserved 
	\param[IN OUT]		ReadFds	<RTK_FD_SET*>: Optional pointer to a set of sockets to be checked for readability.
	\param[IN OUT]		WriteFds	<RTK_FD_SET*>:Optional pointer to a set of sockets to be checked for writability
	\param[IN OUT]		ExceptFds	<RTK_FD_SET*>:Optional pointer to a set of sockets to be checked for errors
	\param[IN]				TimeInterVal	<RT_U64>:maximum time for select to wait
*/
RT_S32
__cdecl 
RtkSelect(
	IN RT_S32 nfds,
	IN OUT RTK_FDSET *ReadFds,
	IN OUT RTK_FDSET *WriteFds,
	IN OUT RTK_FDSET *ExceptFds,
	IN CONST RTK_TIMEVAL *TimeInterVal 
	);

/**
	RtkSetSockOpt sets a socket option
	\param[IN] SockHandle<RTKSOCKET>: A descriptor of socket 
	\param[IN] Level<RT_S32>: The level at which the option is defined
	\param[IN] Optname<RT_S32>: The socket option for which value is retrieved
	\param[IN] Optval<RT_S8*>: A pointer to a buffer in which the option value is stored
	\param[IN] Optlen<RT_S32>: A pointer to the size, in bytes, of the Optval buffer  
	\return if no error occurs, RtkSetSockOpt returns zero, otherwise return RTK_SOCKET_ERROR
*/
RT_S32
__cdecl
RtkSetSockOpt(
  IN  RTKSOCKET SockHandle,
  IN  RT_S32 Level,
  IN  RT_S32 Optname,
  IN  CONST RT_S8 *Optval,
  IN  RT_S32 *Optlen
);

/**
	RtkGetSockOpt retrieves a socket option
	\param[IN] SockHandle<RTKSOCKET>: A descriptor of socket 
	\param[IN] Level<RT_S32>: The level at which the option is defined
	\param[IN] Optname<RT_S32>: The socket option for which value is retrieved
	\param[OUT] Optval<RT_S8*>: A pointer to a buffer in which the option value is returned
	\param[IN OUT] Optlen<RT_S32>: A pointer to the size, in bytes, of the Optval buffer  
	\return If no error occurs, RtkGetSockOpt return zero, otherwise a value of  RTK_SOCKET_ERROR is returned
*/
RT_S32 
__cdecl
RtkGetSockOpt(
  IN  RTKSOCKET SockHandle,
  IN  RT_S32 Level,
  IN  RT_S32 Optname,
  OUT RT_S8 *Optval,
  IN OUT  RT_S32 *Optlen
);

RT_S32 
__cdecl
RtkSetSockMode(
  IN  RTKSOCKET SockHandle,
  IN  RT_U16 iProfileType,
  IN  RT_BOOL bToAMP,
  OUT RT_U64 iFileLength
);


/*************************************************************************************************
	auxiliary function 
*************************************************************************************************/

/**
	Initialize the use of RTK BT socket.
	\param[IN]	VersionRequested	<RT_U16>:	The highest version of Windows Sockets specification that the caller can use
	\param[OUT]	RTKData				<PRTKDATA>:	A pointer to the RTKData data structure that is to receive details of the  Sockets implementation
	\return  success returns 0, fail return RTK_SOCKET_ERROR(-1)
*/
RT_S32
__cdecl
RtkBtStartup(
  IN   RT_U16  VersionRequested,
  OUT  PRTKDATA RTKData
);

/**
	terminates use of RTK Socket
	\return  success returns 0, fail return RTK_SOCKET_ERROR(-1)
*/
RT_S32
__cdecl
RtkBtCleanup(
RT_VOID
);

/**
	get error number of last operation
	\return Last error of socket operation
*/
RT_U32
__cdecl
RtkGetLastError(
RT_VOID
);

/**
    print information of socket
    \param[IN]	 Sock<RTKSOCKET>:Socket handle
*/
RT_VOID
__cdecl
RtkShowSockInfo(
    IN RTKSOCKET  SockHandle
);

/**
    Turn on debug mode
*/
RT_VOID 
__cdecl    
RtkSocketDebugOn(
);

/**
    Turn off debug mode
*/
RT_VOID 
__cdecl    
RtkSocketDebugOff(
);

RT_VOID 
__cdecl
RtkSocketDbgUpdateParameter(
RT_U32 DbgLevel, 
RT_U32 DbgComp
);

#if defined(__cplusplus)
}
#endif

#endif
