#include "pages.h"

void setupPages(AsyncWebServer *server){
  server->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    dbgln("[webserver] request to /");
    request->send(200, "text/plain", "ModbusRtuGateway");
  });
  server->on("/favicon.ico", [](AsyncWebServerRequest *request){
    request->send(204);//TODO add favicon
  });
  server->onNotFound([](AsyncWebServerRequest *request){
    dbg("[webserver] request to ");dbg(request->url());dbgln(" not found");
    request->send(404, "text/plain", "404");
  });
}