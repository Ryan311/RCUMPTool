#pragma once
#include <Windows.h>



#include <map>
#include <vector>
#include <iterator>
#include <algorithm>
#include "GattEntity.h"
#include "gatt_api_export.h"
#include "rtksocket.h"

#pragma warning (disable : 4251)

extern CRITICAL_SECTION g_csLock;

using namespace std;
#define ATTR_TABLE_LEN 64


typedef void 
(*ACCEPT_GATT_CALLBACK)(LPVOID lpvoid, LPVOID pGattEntity);


typedef void 
(*DISCONN_GATT_CALLBACK)(LPVOID lpvoid, ULONGLONG BtAddr);


typedef struct _CONN_PARA_NOTIFY_UPPER
{
	ACCEPT_GATT_CALLBACK pfnAcceptCallBack;
	LPVOID pUpLevel;
	CGattEntity *pGattEntity;
}CONN_PARA_NOTIFY_UPPER, *PCONN_PARA_NOTIFY_UPPER;


typedef struct _DISCONN_PARA_NOTIFY_UPPER
{
	DISCONN_GATT_CALLBACK pfnDisconnCallBack;
	LPVOID pUpLevel;
	ULONGLONG BtAddr;
}DISCONN_PARA_NOTIFY_UPPER, *PDISCONN_PARA_NOTIFY_UPPER;


typedef pair <ULONGLONG,CGattEntity*> BTADDR_GATTENTITY_PAIR;



class CGattInstance
{
public:
	static CGattInstance * GetGattInstance();
	

	~CGattInstance(void);
	void Release();
	
protected:
	CGattInstance();
	CGattInstance(const CGattInstance&);
	CGattInstance& operator= (const CGattInstance&);


//Server
public:

	void RegisterAttrTable(
	__in GATT_ATTRIBUTE * pAttributeTable
	);

	GATT_ATTRIBUTE *GetAttributeTable();

	//Start server and wait for connections
	BOOL StartServer();

	//Stop server to accept sockets
	BOOL StopServer();

	void RtGattRegisterAcceptCallback(ACCEPT_GATT_CALLBACK Callback, LPVOID lpUpper, CGattEntity* pGattEntity);
	void RtGattUnRegisterAcceptCallback(ACCEPT_GATT_CALLBACK Callback);

//Client
public:

	CGattEntity* GATTConnect(RT_U64 RemoteBtAddr);
	//Close a gatt connection by Socket handle
	BOOL GATTClose(ULONGLONG  RemoteBtAddr);

	CGattEntity* GetGattEntityByAddr(ULONGLONG RemoteBtAddr);

private:
	unsigned long AddRef();
	unsigned long GetRef();
	unsigned long ReleaseRef();


	//Remove a Gatt Entity
	void RemoveGattEntity(CGattEntity* pGattEntity);

public:

	LE_STATUS RtGattWaitForIndication();



private:	
	static CGattInstance * m_pGattInstance;
	static bool m_bServerIsRunning;

	static unsigned long m_RefCount;

	GATT_ATTRIBUTE * m_pLeAttrTable;

	//Start a thread waiting for connections	
	static UINT WINAPI StartToAccept(LPVOID lpvoid);
	
	//when a connection comes, notify upper layer to do some thing
	void ConnNotifyUppers(CGattEntity* pLeGattEntity);
	//Function pointer to upperlayer
	vector<CONN_PARA_NOTIFY_UPPER*> m_ArrayConnParaNotifyUpper; 

	//Function pointer to callback function notify upper layer disconnect
	
	//Socket which is listening for connections
	RTKSOCKET	m_hATTSocketServer;
	//thread handle which is listening for connections
	HANDLE  m_hThreadAccept;

	//GATTEntity vector
	vector<CGattEntity*> m_ArrayGattEntity;
	//look up ThreadRecv by Socket Handle, When close socket, we need to exit Recv thread.
	map<ULONGLONG,CGattEntity*> m_MapBtAddrGattEntity;

	
	void ReleaseConnNotifyArray();

};
