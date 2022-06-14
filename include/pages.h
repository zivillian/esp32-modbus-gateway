#ifndef PAGES_H
    #define PAGES_H

    #include <ESPAsyncWebServer.h>
    #include "config.h"

    void setupPages(AsyncWebServer* server);
    void sendResponseHeader(AsyncResponseStream *response, const String &title);
    void sendResponseTrailer(AsyncResponseStream *response);

#endif /* PAGES_H */