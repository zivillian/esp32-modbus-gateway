#ifndef PAGES_H
    #define PAGES_H

    #include <ESPAsyncWebServer.h>
    #include <ModbusBridgeWiFi.h>
    #include <ModbusClientRTU.h>
    #include "config.h"

    void setupPages(AsyncWebServer* server, ModbusClientRTU *rtu, ModbusBridgeWiFi *bridge);
    void sendResponseHeader(AsyncResponseStream *response, const String &title);
    void sendResponseTrailer(AsyncResponseStream *response);

#endif /* PAGES_H */