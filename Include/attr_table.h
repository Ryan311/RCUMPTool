#ifndef __ATTR_TABLE_H
#define __ATTR_TABLE_H

/**

Copyright (c) Realtek Corporation. All rights reserved.

\file    attr_table.h

\brief   Attribute table functions.

*/


#include "gatt_api.h"



// Definition

// User-defioned UUID
enum
{
	// Head
	ATTR_HEAD = 0xa000,

	// Test only
	ATTR_ATTR_OPCODE_SUPPORTED,
	ATTR_BATTERY_STATE_SERVICE,
	ATTR_BATTERY_STATE,
	ATTR_THERMOMETER_HUMIDITY_SERVICE,
	ATTR_TEMPERATURE,
	ATTR_RELATIVE_HUMIDITY,
	ATTR_MANUFACTURER_SERVICE,
	ATTR_MANUFACTURER_NAME,
	ATTR_VENDOR_SPECIFIC_SERVICE,
	ATTR_VENDOR_SPECIFIC_TYPE,
};

#define ATTR_TABLE_DEVICE_INFORMATION_SERVICE 0x180a
#define ATTR_TABLE_MANUFACTURER_NAME_STRING 0x2a29
#define ATTR_TABLE_SYSTEM_ID 0x2a23
#define ATTR_TABLE_MODEL_NUMBER_STRING 0x2a24
#define ATTR_TABLE_LINK_LOSS_SERVICE 0x1803
#define ATTR_TABLE_ALERT_LEVEL 0x2a30

// Starting handle
#define ATTR_STARTING_HANDLE_HEALTH_THERMOMETER   0x600
#define ATTR_STARTING_HANDLE_IMMEDIATE_ALERT      0x604
#define ATTR_STARTING_HANDLE_DEVICE_INFORMATION   0x700
#define ATTR_STARTING_HANDLE_PROXIMITY            0x707
#define ATT_STARTING_HANDLE_HID                   0x800
#define ATTR_STARTING_HANDLE_TEST_ONLY            0xf000


#ifdef __cplusplus
extern "C" {
#endif


// Attribute table functions
void
__cdecl
BuildAttrTable(
	GATT_ATTRIBUTE *pAttrTable
	);

void
BuildAttrTableHealthThermometer(
	GATT_ATTRIBUTE *pAttrTable,
	unsigned long *pIndex,
	unsigned short StartingHandle
	);

void
BuildAttrTableImmediateAlert(
	GATT_ATTRIBUTE *pAttrTable,
	unsigned long *pIndex,
	unsigned short StartingHandle
	);

void
BuildAttrTableDeviceInformation(
	GATT_ATTRIBUTE *pAttrTable,
	unsigned long *pIndex,
	unsigned short StartingHandle
	);

void
BuildAttrTableProximity(
	GATT_ATTRIBUTE *pAttrTable,
	unsigned long *pIndex,
	unsigned short StartingHandle
	);

void
BuildAttrTableTestOnly(
	GATT_ATTRIBUTE *pAttrTable,
	unsigned long *pIndex,
	unsigned short StartingHandle
	);



// Attribute table
// Note: 1. Offset column is hanlde offset.
//       2. (0xnnnn) is handle offset.

// Health Thermometer
// Entry: Offset,   Type,                       Value
// 0:     0x0,      primary service,            health thermometer service
// 1:     0x1,      characteristic,             'i', (0x2), temperature measurement
// 2:     0x2,      temperature measurement,    0x00
// 3:     0x3,      client char config,         none

// Immediate Alert
// Entry: Offset,   Type,                       Value
// 0:     0x0,      primary service,            immediate alert service
// 1:     0x1,      characteristic,             'w w/o rsp', (0x2), alert level
// 2:     0x2,      alert level,                0x00

// Device Information
// Entry: Offset,   Type,                       Value
// 0:     0x0,      primary service,            device information service
// 1:     0x1,      characteristic,             'r', (0x2), manufacturer name string
// 2:     0x2,      manufacturer name string,   "Realsil Bluetooth LE"
// 3:     0x3,      characteristic,             'r', (0x4), model number string
// 4:     0x4,      model number string,        "0000000001"
// 5:     0x5,      characteristic,             'r', (0x6), system ID
// 6:     0x6,      system ID,                  0x00000001

// Proximity
// Entry: Offset,   Type,                       Value
// 0:     0x0,      primary service,            link loss service
// 1:     0x1,      characteristic,             'r''w', (0x2), alert level
// 2:     0x2,      alert level,                0x00

// Test only
// Entry: Offset,   Type,                       Value
// 0:     0x1,      primary service,            generic access service
// 1:     0x4,      characteristic,             'r', (0x6), device name
// 2:     0x6,      device name,                "A Very Long Device Name Using A Long Attribute"
// 3:     0x10,     primary service,            generic attribute service
// 4:     0x11,     characteristic,             'r', (0x12), attr opcodes supported
// 5:     0x12,     attr opcodes supported,     0x01ff
// 6:     0x100,    primary service,            battery state service
// 7:     0x106,    characteristic,             'r', (0x110), battery state
// 8:     0x110,    battery state,              0x04
// 9:     0x200,    primary service,            thermometer humidity service
// 10:    0x201,    include,                    (0x500), (0x502), manufacturer service
// 11:    0x202,    include,                    (0x550), (0x568)
// 12:    0x203,    characteristic,             'r', (0x204), temperature
// 13:    0x204,    temperature,                0x028a
// 14:    0x206,    char user description,      "Outside Temperature"
// 15:    0x210,    characteristic,             'r', (0x212), relative humidity
// 16:    0x212,    relative humidity,          0x27
// 17:    0x214,    char user description,      "Outside Relative Humidity"
// 18:    0x500,    secondary service,          manufacturer service
// 19:    0x501,    characteristic,             'r', (0x502), manufacture name
// 20:    0x502,    manufacture name,           "ACME"
// 21:    0x550,    secondary service,          vendor specific service
// 22:    0x560,    characteristic,             'r', (0x568), vendor specific type
// 23:    0x568,    vendor specific type,       0x56656e646f72



#ifdef __cplusplus
	}
#endif


#endif