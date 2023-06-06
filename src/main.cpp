#include <WiFi.h>
#include <AsyncTCP.h>
#include <WiFiManager.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <Logging.h>
#include <ModbusBridgeWiFi.h>
#include <ModbusClientRTU.h>
#include "config.h"
#include "pages.h"
#include "Button2.h"



AsyncWebServer webServer(80);
Config config;
Preferences prefs;
ModbusClientRTU *MBclient;
ModbusBridgeWiFi MBbridge;
WiFiManager wm;
Button2 wifiButton;    

// select which pin will trigger the configuration portal when set to LOW

 const int timeout = 120; // seconds to run for
#ifndef LONG_PRESS_TIME
  #define LONG_PRESS_TIME  3000 // 4000 milliseconds
#endif


void onLongClick(Button2 &btn){
   
        // set configportal timeout
        wm.setConfigPortalTimeout(timeout);

        if (!wm.startConfigPortal()) {
            dbgln("failed to connect and hit timeout");
            delay(1000);
            //reset and try again, or maybe put it to deep sleep
            ESP.restart();
        }
 
        //if you get here you have connected to the WiFi
        dbgln("connected to wifi...");
}
 


void setup() {
  debugSerial.begin(115200);
  dbgln();
  dbgln("[config] load")
  prefs.begin("modbusRtuGw");
  #if defined(WIFI_RST_BUTTON_PIN)  
    wifiButton.begin(WIFI_RST_BUTTON_PIN);
    wifiButton.setLongClickDetectedHandler( &onLongClick);
    wifiButton.setLongClickTime(LONG_PRESS_TIME);
  #endif
  config.begin(&prefs);
  debugSerial.end();
  debugSerial.begin(config.getSerialBaudRate(), config.getSerialConfig());
  dbgln("[wifi] start");
  WiFi.mode(WIFI_STA);
  wm.setClass("invert");
  wm.autoConnect();
  dbgln("[wifi] finished");
  dbgln("[modbus] start");

  RTUutils::prepareHardwareSerial(modbusSerial);
#if defined(RX_PIN) && defined(TX_PIN)
  // use rx and tx-pins if defined in platformio.ini
  modbusSerial.begin(config.getModbusBaudRate(), config.getModbusConfig(), RX_PIN, TX_PIN );
  dbgln("Use user defined RX/TX pins");
#else
  // otherwise use default pins for hardware-serial2
  modbusSerial.begin(config.getModbusBaudRate(), config.getModbusConfig());
#endif

  MBclient = new ModbusClientRTU(config.getModbusRtsPin());
  MBclient->setTimeout(1000);
  MBclient->begin(modbusSerial);
  for (uint8_t i = 1; i < 248; i++)
  {
    MBbridge.attachServer(i, i, ANY_FUNCTION_CODE, MBclient);
  }  
  MBbridge.start(config.getTcpPort(), 10, config.getTcpTimeout());
  dbgln("[modbus] finished");
  setupPages(&webServer, MBclient, &MBbridge, &config, &wm);
  webServer.begin();
  dbgln("[setup] finished");
}




void loop() {
  wifiButton.loop();
}