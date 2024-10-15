// =================================================================================================
// Modified version of ModbusBridgeWiFi.h to user native Ethernet support
// =================================================================================================
#ifndef _MODBUS_BRIDGE_ETHERNET_H
#define _MODBUS_BRIDGE_ETHERNET_H
#include "options.h"
#include <ETH.h>

#undef SERVER_END
#define SERVER_END server.end();

#include "ModbusServerTCPtemp.h"
#include "ModbusBridgeTemp.h"

// Same name as ModBusBridgeWiFi
using ModbusBridgeWiFi = ModbusBridge<ModbusServerTCP<WiFiServer, WiFiClient>>;

#endif
