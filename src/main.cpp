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

AsyncWebServer webServer(80);
Config config;
Preferences prefs;
ModbusClientRTU *MBclient;
ModbusBridgeWiFi MBbridge;
WiFiManager wm;

void setup() {
  debugSerial.begin(115200);
  dbgln();
  dbgln("[config] load")
  prefs.begin("modbusRtuGw");
  config.begin(&prefs);
  debugSerial.end();
  debugSerial.begin(config.getSerialBaudRate(), config.getSerialConfig());
  dbgln("[wifi] start");
  WiFi.mode(WIFI_STA);
  wm.setClass("invert");
  auto reboot = false;
  wm.setAPCallback([&reboot](WiFiManager *wifiManager){reboot = true;});
  wm.autoConnect();
  if (reboot){
    ESP.restart();
  }
  dbgln("[wifi] finished");
  dbgln("[modbus] start");

  MBUlogLvl = LOG_LEVEL_WARNING;
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
  MBclient->begin(modbusSerial, 1);
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
  unsigned long currentMillis = millis();
  // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >=interval)) {
    Serial.print(millis());
    dbgln("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis = currentMillis;
  }  
}