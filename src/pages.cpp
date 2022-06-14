#include "pages.h"

void setupPages(AsyncWebServer *server){
  server->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    dbgln("[webserver] request to /");
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    sendResponseHeader(response, "Main");
    response->print("ModbusRtuGateway");
    sendResponseTrailer(response);
    request->send(response);
  });
  server->on("/favicon.ico", [](AsyncWebServerRequest *request){
    request->send(204);//TODO add favicon
  });
  server->on("/style.css", [](AsyncWebServerRequest *request){
    AsyncWebServerResponse *response = request->beginResponse(200, "text/css", "body{"
    "}");
    response->addHeader("ETag", __DATE__ "" __TIME__);
    request->send(response);
  });
  server->onNotFound([](AsyncWebServerRequest *request){
    dbg("[webserver] request to ");dbg(request->url());dbgln(" not found");
    request->send(404, "text/plain", "404");
  });
}

void sendResponseHeader(AsyncResponseStream *response, const String &title){
    response->addHeader("ETag", __DATE__ "" __TIME__);
    response->print("<!DOCTYPE html>"
      "<html lang=\"en\" class="">"
      "<head>"
      "<meta charset='utf-8'>"
      "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1,user-scalable=no\"/>");
    response->printf("<title>Modbus RTU/TCP Gateway - %s</title>", title);
    response->print("<link rel=\"stylesheet\" href=\"style.css\">"
      "</head>"
      "<body>");
    response->printf("<h3>%s</h3>", title);
}

void sendResponseTrailer(AsyncResponseStream *response){
    response->print("</body></html>");
}