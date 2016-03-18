// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LEGAP_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LEGAP_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifndef LEGAP_H
#define LEGAP_H

#include "rt_type.h"

#ifdef LEGAP_EXPORTS
#define LEGAP_API __declspec(dllexport)
#else
#define LEGAP_API __declspec(dllimport)
#endif

//Export Examples:
//extern LEGAP_API int nLEGap;
//LEGAP_API int fnLEGap(void);

//================================================================
/**
    define RT Status Code.
    Add more status if neccessary
*/    
typedef enum _ERR_STATUS{
    ERR_SUCCESS                  = 0,  ///< means everything is OK. 
    ERR_FAILURE                  = 1,  ///< means operation failed
    ERR_INVALID_PARAMETER        = 2,  ///< means parameter is invalid
    ERR_COMMAND_FAILED           = 3,  ///< means can¡¯t send the command to driver. 
    ERR_ENABLE_BLUETOOTH_FAILED  = 4,  ///< means can¡¯t enable bluetooth function. 
    ERR_NOT_FOUND_BLE_CHIP       = 5,  ///< means not found BLE chip on this device.   
    ERR_LE_IS_DISABLE            = 6,  ///< means le func is disabled
    /// more ...
}ERR_STATUS,*PERR_STATUS;


// Prefix: RLE_GAP
#pragma pack(push, 1)
//--------------------------Command Assist Struct----------------------//


/**
    Command Parameters in Le Set Scan Para command.
*/
typedef struct _RTK_UI_LE_SET_SCAN_PARA
{
    unsigned char   LeScanType;
    unsigned short  LeScanInterval;
    unsigned short  LeScanWindows;
    unsigned char   OwnAddrType;
    unsigned char   ScanFilterPolicy;
    unsigned char   FilterDuplicates;
}RTK_UI_LE_SET_SCAN_PARA;

/**
    Command Parameters in Le Set Advertising Parameters command.
*/
typedef struct _RTK_UI_LE_SET_ADV_PARA
{
    unsigned short   AdvIntervalMin;
    unsigned short   AdvIntervalMax;
    unsigned char    AdvType;
    unsigned char    OwnAddrType;
    unsigned char    DirectAddrType;
    unsigned char    DirectAddr[6];
    unsigned char    AdvChannelMap;
    unsigned char    AdvFilterPolicy;
}RTK_UI_LE_SET_ADV_PARA;

typedef enum _SCAN_MODE
{
    PASSIVE  = 0,  //passive mode
    ACTIVE   = 1   //active mode
}SCAN_MODE;

// AD_TYPE defined by SIG
enum AD_TYPE{
    AT_FLAGS                            = 1,
    AT_MORE_16UUID                  	= 2,
    AT_COMPLETE_16UUID              	= 3,
    AT_MORE_32UUID                  	= 4,
    AT_COMPLETE_32UUID              	= 5,
    AT_MORE_128UUID                 	= 6,
    AT_COMPLETE_128UUID             	= 7,
    AT_SHORTEN_LOCAL_NAME           	= 8,
    AT_COMPLETE_LOCAL_NAME          	= 9,
    AT_TX_POWER_LEVEL               	= 0xA,
    AT_CLASS_OF_DEVICE              	= 0xD,
    AT_SIMPLE_PAIR_HASH_C           	= 0xE,
    AT_SIMPLE_PAIR_RAND_R           	= 0xF,
    AT_TK_VALUE                     	= 0x10,
    AT_OOB_FLAGS                    	= 0x11,
    AT_SLAVE_CONNECTION_INTERVAL_RANGE  = 0x12,
    AT_SERVICE_SOLICATION_UUID16	    = 0x14,
    AT_SERVICE_SOLICATION_UUID128       = 0x15,
    AT_SERVICE_DATA                     = 0x16,
    AT_SERVICE_DATA_32                  = 0x20, 
    AT_SERVICE_DATA_128                 = 0x21, 
    AT_PUBLIC_TARGET_ADDRESS            = 0x17, 
    AT_RANDOM_TARGET_ADDRESS            = 0x18, 
    AT_APPEARANCE                       = 0x19, 
    AT_ADVERTISING_INTERVAL             = 0x1A,
    AT_LE_BT_ADDRESS                    = 0x1B,
    AT_LE_ROLE                          = 0x1C,
    AT_SIMPLE_PAIR_HASH                 = 0x1D,
    AT_SIMPLE_PAIR_Randomizer           = 0x1E,
    AT_3D_INROMATION_DATA               = 0x3D,

    //MORE ADDED BELOW!!
    AT_MANUFACTURE_SPEC_DATA            = 0xFF
};

enum AD_FLAG_BIT{
    AF_LIMIT_DISCOVER = BIT0,
    AF_GENER_DISCOVER = BIT1,
    AF_BREDR_NOT_PRESENT = BIT2,
    AF_SIMULT_CONTROLLER_CAPABLE = BIT3,
    AF_SIMULT_HOST_CAPABLE = BIT4
};

#pragma warning(disable:4200) //  zero-sized array in struct/union 
typedef struct _ADV_DATA{
    RT_U8 Length;
    RT_U8 AdType;
    RT_U8 AdData[0];
}ADV_DATA, *PADV_DATA;

#pragma pack(pop)

LEGAP_API typedef void (*BLESCANCALLBACK)(
	    RT_U8* pAddress, 
		const int nSizeAddresss, 
        RT_U8* pPkt, 
		const int nSizePkt , 
		void* pUserData
		); 

// Common Interface
LEGAP_API ERR_STATUS SetBluetoothEnable(IN RT_BOOL bEnable);
LEGAP_API ERR_STATUS BleIsSupported(OUT RT_BOOL *pSupport);
LEGAP_API ERR_STATUS BleEnableScan(IN RT_BOOL bEnable, IN unsigned char FilterDuplicates);
LEGAP_API ERR_STATUS BleEnableAdvertising(IN RT_BOOL bEnable);

/// Advertiser Interface
LEGAP_API ERR_STATUS BleSetAdvertisingPktInterval(IN RT_U32 nInterval);
LEGAP_API ERR_STATUS BleGetAdvertisingPktInterval(OUT RT_U32 *pInterval);
//LEGAP_API ERR_STATUS BleSetAdvertisingPktTxPower(IN RT_U32 nTxPow);
LEGAP_API ERR_STATUS BleGetAdvertisingPktTxPower(OUT RT_U32* pTxPow);
LEGAP_API ERR_STATUS BleSetAdvertisingPkt(IN RT_U8* pPkt, IN RT_U32 nSize);
LEGAP_API ERR_STATUS BleIsAdvertisingPkt(OUT RT_BOOL* bIsAdv);

/// Scan Interface
LEGAP_API ERR_STATUS BleSetScanMode(IN SCAN_MODE scanMode);
LEGAP_API ERR_STATUS BleSetScanningInterval(IN RT_U32 nInterval);
LEGAP_API ERR_STATUS BleGetScanningInterval(OUT RT_U32* pInterval);
LEGAP_API ERR_STATUS BleRegisterScanCallback(BLESCANCALLBACK callbackFun);
LEGAP_API ERR_STATUS BleUnregisterScanCallback();
LEGAP_API ERR_STATUS BleIsScanning(RT_BOOL* bIsAdv);
//LEGAP_API ERR_STATUS BleGetRSSI(OUT RT_U32* pRSSI);    // no need

#endif