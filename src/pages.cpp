#include "pages.h"

void setupPages(AsyncWebServer *server){
  server->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    dbgln(F("[webserver] request to /"));
    request->send(200, F("text/plain"), F("ModbusRtuGateway"));
  });
  server->on("/favicon.ico", [](AsyncWebServerRequest *request){
    request->send(204);//TODO add favicon
  });
  server->onNotFound([](AsyncWebServerRequest *request){
    dbg(F("[webserver] request to "));dbg(request->url());dbgln(F(" not found"));
    request->send(404, F("text/plain"), F("404"));
  });
}