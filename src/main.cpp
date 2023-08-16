#if defined(ETHERNET)
#include <ETH.h>
#include "ModbusBridgeEthernet.h"
#else
#include <WiFi.h>
#include <WiFiManager.h>
#include <ModbusBridgeWiFi.h>
#endif
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <Logging.h>
#include <ModbusClientRTU.h>
#include "config.h"
#include "pages.h"

AsyncWebServer webServer(80);
Config config;
Preferences prefs;
ModbusClientRTU *MBclient;
ModbusBridgeWiFi MBbridge;
#if !defined(ETHERNET)
WiFiManager wm;
#endif

#if defined(ETHERNET)
void setupNetwork()
{
  LOG_I("[Ethernet] Setup Network\n");
  ETH.begin();
  LOG_I("[Ethernet] Done\n");
}
#else
void setupNetwork()
{
  LOG_I("[Wifi] Setup Network\n");

  WiFi.mode(WIFI_STA);
  wm.setClass("invert");
  auto reboot = false;
  wm.setAPCallback([&reboot](WiFiManager *wifiManager){ reboot = true; });
  wm.autoConnect();
  if (reboot)
  {
    ESP.restart();
  }

  LOG_I("[Wifi] Done\n");
}
#endif

void setup()
{
#if !defined(DEBUG)
  MBUlogLvl = LOG_LEVEL_WARNING;
#endif

  LOGDEVICE = &(DEBUG_SERIAL);

  DEBUG_SERIAL.begin(115200);

  LOG_I("[Config] Loading\n");

  // Load preferences
  prefs.begin("modbusRtuGw");
  config.begin(&prefs);

  // Reset Debug Serial based on the configuration
  DEBUG_SERIAL.flush();
  DEBUG_SERIAL.begin(config.getSerialBaudRate(), config.getSerialConfig());

  LOG_I("[Config] Done\n");

  setupNetwork();

  LOG_I("[Modbus] Setup\n");

  RTUutils::prepareHardwareSerial(MODBUS_SERIAL);
#if defined(RX_PIN) && defined(TX_PIN)
  LOG_I("[Modbus] Using provided RX(%d)/TX(%d) pins\n", RX_PIN, TX_PIN);
  MODBUS_SERIAL.begin(config.getModbusBaudRate(), config.getModbusConfig(), RX_PIN, TX_PIN);
#else
  LOG_I("[Modbus] Using default RX/TX pins\n");
  MODBUS_SERIAL.begin(config.getModbusBaudRate(), config.getModbusConfig());
#endif

  MBclient = new ModbusClientRTU(config.getModbusRtsPin());
  MBclient->setTimeout(config.getRtuTimeout());
  MBclient->begin(MODBUS_SERIAL, 1);
  for (uint8_t i = 1; i <= MODBUS_MAX_ADDRESS; i++)
  {
    MBbridge.attachServer(i, i, ANY_FUNCTION_CODE, MBclient);
  }
  MBbridge.start(config.getTcpPort(), 10, config.getTcpTimeout());
  LOG_I("[Modbus] Done\n");

  LOG_I("[WebServer] Setup\n");
#if defined(ETHERNET)
  setupPages(&webServer, MBclient, &MBbridge, &config);
#else
  setupPages(&webServer, MBclient, &MBbridge, &config, &wm);
#endif
  webServer.begin();
  LOG_I("[WebServer] Done\n");
}

void loop() {}
