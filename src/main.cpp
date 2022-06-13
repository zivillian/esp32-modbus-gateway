#include <WiFi.h>
#include <AsyncTCP.h>
#include <WiFiManager.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <Preferences.h>
#include "config.h"
#include "pages.h"

AsyncWebServer webServer(80);
config_type config;
Preferences prefs;
WiFiManager wm;

void setup() {
  debugSerial.begin(9600);
  dbgln();
  dbgln(F("[config] load"))
  prefs.begin("modbusRtuGw");
  if (prefs.getBytesLength("settings") != sizeof(config_type))
  {
    dbgln(F("[config] no config found - using default"));
    config = default_config;
    prefs.putBytes("settings", &config, sizeof(config_type));
  }
  else
  {
    dbgln(F("[config] config found - loading"));
    prefs.getBytes("settings", &config, sizeof(config_type));
  }
  dbgln(F("[wifi] start"));
  WiFi.mode(WIFI_STA);
  wm.autoConnect();
  dbgln(F("[wifi] finished"));
  setupPages(&webServer);
  AsyncElegantOTA.begin(&webServer);
  webServer.begin();
  dbgln(F("[setup] finished"));
}

void loop() {
  // put your main code here, to run repeatedly:
}