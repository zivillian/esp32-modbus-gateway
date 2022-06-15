#include <WiFi.h>
#include <AsyncTCP.h>
#include <WiFiManager.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <Preferences.h>
#include <Logging.h>
#include <ModbusBridgeWiFi.h>
#include <ModbusClientRTU.h>
#include "config.h"
#include "pages.h"

AsyncWebServer webServer(80);
config_type config;
Preferences prefs;
ModbusClientRTU MBclient(modbusSerial);
ModbusBridgeWiFi MBbridge;
WiFiManager wm;

void setup() {
  debugSerial.begin(9600);
  dbgln();
  dbgln("[config] load")
  prefs.begin("modbusRtuGw");
  if (prefs.getBytesLength("settings") != sizeof(config_type))
  {
    dbgln("[config] no config found - using default");
    config = default_config;
    prefs.putBytes("settings", &config, sizeof(config_type));
  }
  else
  {
    dbgln("[config] config found - loading");
    prefs.getBytes("settings", &config, sizeof(config_type));
  }
  dbgln("[wifi] start");
  WiFi.mode(WIFI_STA);
  wm.autoConnect();
  dbgln("[wifi] finished");
  dbgln("[modbus] start");
  modbusSerial.begin(config.baud, config.serialConfig);
  MBclient.setTimeout(1000);
  MBclient.begin();
  for (uint8_t i = 1; i < 248; i++)
  {
    MBbridge.attachServer(i, i, ANY_FUNCTION_CODE, &MBclient);
  }  
  MBbridge.start(502, 10, 10000);
  dbgln("[modbus] finished");
  setupPages(&webServer, &MBclient, &MBbridge);
  AsyncElegantOTA.begin(&webServer);
  webServer.begin();
  dbgln("[setup] finished");
}

void loop() {
  // put your main code here, to run repeatedly:
}