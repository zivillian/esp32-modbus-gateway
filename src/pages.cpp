#include "pages.h"
#define ETAG "\"" __DATE__ "" __TIME__ "\""
#define ADMIN_WEB_PASS  \
        if ((!config->getWebPassword().equals("")) && (!request->authenticate("admin", config->getWebPassword().c_str()))) \
            return request->requestAuthentication();
#define WEB_PASS_PLACEHOLDER "****"


void setupPages(AsyncWebServer *server, ModbusClientRTU *rtu, ModbusBridgeWiFi *bridge, Config *config, WiFiManager *wm){
  server->on("/", HTTP_GET, [config](AsyncWebServerRequest *request){
    
    ADMIN_WEB_PASS;

    dbgln("[webserver] GET /");
    auto *response = request->beginResponseStream("text/html");
    sendResponseHeader(response, "Main");
    sendButton(response, "Status", "status");
    sendButton(response, "Config", "config");
    sendButton(response, "Debug", "debug");
    sendButton(response, "Firmware update", "update");
    sendButton(response, "WiFi reset", "wifi", "r");
    sendButton(response, "Reboot", "reboot", "r");
    sendResponseTrailer(response);
    request->send(response);
  });
  server->on("/status", HTTP_GET, [rtu, bridge, config](AsyncWebServerRequest *request){
    
    ADMIN_WEB_PASS;

    dbgln("[webserver] GET /status");
    auto *response = request->beginResponseStream("text/html");
    sendResponseHeader(response, "Status");
    response->print("<table>");

    // show ESP infos...
    sendTableRow(response, "ESP Uptime (sec)", esp_timer_get_time() / 1000000);
    sendTableRow(response, "ESP SSID", WiFi.SSID());
    sendTableRow(response, "ESP RSSI", WiFi.RSSI());
    sendTableRow(response, "ESP WiFi Quality", WiFiQuality(WiFi.RSSI()));
    sendTableRow(response, "ESP MAC", WiFi.macAddress());
    sendTableRow(response, "ESP IP",  WiFi.localIP().toString() );

    sendTableRow(response, "RTU Messages", rtu->getMessageCount());
    sendTableRow(response, "RTU Pending Messages", rtu->pendingRequests());
    sendTableRow(response, "RTU Errors", rtu->getErrorCount());
    sendTableRow(response, "Bridge Message", bridge->getMessageCount());
    sendTableRow(response, "Bridge Clients", bridge->activeClients());
    sendTableRow(response, "Bridge Errors", bridge->getErrorCount());
    response->print("<tr><td>&nbsp;</td><td></td></tr>");
    sendTableRow(response, "Build time", __DATE__ " " __TIME__);
    response->print("</table><p></p>");
    sendButton(response, "Back", "/");
    sendResponseTrailer(response);
    request->send(response);
  });
  server->on("/reboot", HTTP_GET, [config](AsyncWebServerRequest *request){
    
    ADMIN_WEB_PASS;

    dbgln("[webserver] GET /reboot");
    auto *response = request->beginResponseStream("text/html");
    sendResponseHeader(response, "Really?");
    sendButton(response, "Back", "/");
    response->print("<form method=\"post\">"
        "<button class=\"r\">Yes, do it!</button>"
      "</form>");
    sendResponseTrailer(response);
    request->send(response);
  });
  server->on("/reboot", HTTP_POST, [config](AsyncWebServerRequest *request){
    
    ADMIN_WEB_PASS;

    dbgln("[webserver] POST /reboot");
    request->redirect("/");
    dbgln("[webserver] rebooting...")
    ESP.restart();
    dbgln("[webserver] rebooted...")
  });
  server->on("/config", HTTP_GET, [config](AsyncWebServerRequest *request){
    
    ADMIN_WEB_PASS;

    dbgln("[webserver] GET /config");
    auto *response = request->beginResponseStream("text/html");
    sendResponseHeader(response, "Modbus TCP");
    response->print("<form method=\"post\">");
    response->print("<table>"
      "<tr>"
        "<td>"
          "<label for=\"tp\">TCP Port</label>"
        "</td>"
        "<td>");
    response->printf("<input type=\"number\" min=\"1\" max=\"65535\" id=\"tp\" name=\"tp\" value=\"%d\">", config->getTcpPort());
    response->print("</td>"
      "</tr>"
      "<tr>"
        "<td>"
          "<label for=\"tt\">TCP Timeout (ms)</label>"
        "</td>"
        "<td>");
    response->printf("<input type=\"number\" min=\"1\" id=\"tt\" name=\"tt\" value=\"%d\">", config->getTcpTimeout());
    response->print("</td>"
        "</tr>"
        "</table>"
        "<h3>Modbus RTU</h3>"
        "<table>"
        "<tr>"
          "<td>"
            "<label for=\"mb\">Baud rate</label>"
          "</td>"
          "<td>");
    response->printf("<input type=\"number\" min=\"0\" id=\"mb\" name=\"mb\" value=\"%lu\">", config->getModbusBaudRate());
    response->print("</td>"
        "</tr>"
        "<tr>"
          "<td>"
            "<label for=\"md\">Data bits</label>"
          "</td>"
          "<td>");
    response->printf("<input type=\"number\" min=\"5\" max=\"8\" id=\"md\" name=\"md\" value=\"%d\">", config->getModbusDataBits());
    response->print("</td>"
        "</tr>"
        "<tr>"
          "<td>"
            "<label for=\"mp\">Parity</label>"
          "</td>"
          "<td>");
    response->printf("<select id=\"mp\" name=\"mp\" data-value=\"%d\">", config->getModbusParity());
    response->print("<option value=\"0\">None</option>"
              "<option value=\"2\">Even</option>"
              "<option value=\"3\">Odd</option>"
            "</select>"
          "</td>"
        "</tr>"
        "<tr>"
          "<td>"
            "<label for=\"ms\">Stop bits</label>"
          "</td>"
          "<td>");
    response->printf("<select id=\"ms\" name=\"ms\" data-value=\"%d\">", config->getModbusStopBits());
    response->print("<option value=\"1\">1 bit</option>"
              "<option value=\"2\">1.5 bits</option>"
              "<option value=\"3\">2 bits</option>"
            "</select>"
          "</td>"
        "</tr>"
        "<tr>"
          "<td>"
            "<label for=\"mr\">RTS Pin</label>"
          "</td>"
          "<td>");
    response->printf("<select id=\"mr\" name=\"mr\" data-value=\"%d\">", config->getModbusRtsPin());
    response->print("<option value=\"-1\">Auto</option>"
              "<option value=\"4\">D4</option>"
              "<option value=\"13\">D13</option>"
              "<option value=\"14\">D14</option>"
              "<option value=\"18\">D18</option>"
              "<option value=\"19\">D19</option>"
              "<option value=\"21\">D21</option>"
              "<option value=\"22\">D22</option>"
              "<option value=\"23\">D23</option>"
              "<option value=\"25\">D25</option>"
              "<option value=\"26\">D26</option>"
              "<option value=\"27\">D27</option>"
              "<option value=\"32\">D32</option>"
              "<option value=\"33\">D33</option>"
            "</select>"
          "</td>"
        "</tr>"
        "</table>"
        "<h3>Serial (Debug)</h3>"
        "<table>"
        "<tr>"
          "<td>"
            "<label for=\"sb\">Baud rate</label>"
          "</td>"
          "<td>");
    response->printf("<input type=\"number\" min=\"0\" id=\"sb\" name=\"sb\" value=\"%lu\">", config->getSerialBaudRate());
    response->print("</td>"
        "</tr>"
        "<tr>"
          "<td>"
            "<label for=\"sd\">Data bits</label>"
          "</td>"
          "<td>");
    response->printf("<input type=\"number\" min=\"5\" max=\"8\" id=\"sd\" name=\"sd\" value=\"%d\">", config->getSerialDataBits());
    response->print("</td>"
        "</tr>"
        "<tr>"
          "<td>"
            "<label for=\"sp\">Parity</label>"
          "</td>"
          "<td>");
    response->printf("<select id=\"sp\" name=\"sp\" data-value=\"%d\">", config->getSerialParity());
    response->print("<option value=\"0\">None</option>"
              "<option value=\"2\">Even</option>"
              "<option value=\"3\">Odd</option>"
            "</select>"
          "</td>"
        "</tr>"
        "<tr>"
          "<td>"
            "<label for=\"ss\">Stop bits</label>"
          "</td>"
          "<td>");
    response->printf("<select id=\"ss\" name=\"ss\" data-value=\"%d\">", config->getSerialStopBits());
    response->print("<option value=\"1\">1 bit</option>"
              "<option value=\"2\">1.5 bits</option>"
              "<option value=\"3\">2 bits</option>"
            "</select>"
          "</td>"
        "</tr>"
        "</table>");
    
    response->print(        "<h3>Other</h3>"
        "<table>"
        "<tr>"
          "<td>"
            "<label for=\"wp\">Web password</label>"
          "</td>"
          "<td>");
    response->printf("<input type=\"password\" min=\"0\" id=\"wp\" name=\"wp\" value=\"%s\">", WEB_PASS_PLACEHOLDER); // we're not returning configured password to user instead we're sending placeholder
    response->print("</td>"
        "</tr>"
        "</table>");

    response->print("<button class=\"r\">Save</button>"
      "</form>"
      "<p></p>");
    sendButton(response, "Back", "/");
    response->print("<script>"
      "(function(){"
        "var s = document.querySelectorAll('select[data-value]');"
        "for(d of s){"
          "d.querySelector(`option[value='${d.dataset.value}']`).selected=true"
      "}})();"
      "</script>");
    sendResponseTrailer(response);
    request->send(response);
  });
  server->on("/config", HTTP_POST, [config](AsyncWebServerRequest *request){
    
    ADMIN_WEB_PASS;

    dbgln("[webserver] POST /config");
    if (request->hasParam("tp", true)){
      auto port = request->getParam("tp", true)->value().toInt();
      config->setTcpPort(port);
      dbgln("[webserver] saved port");
    }
    if (request->hasParam("tt", true)){
      auto timeout = request->getParam("tt", true)->value().toInt();
      config->setTcpTimeout(timeout);
      dbgln("[webserver] saved timeout");
    }
    if (request->hasParam("mb", true)){
      auto baud = request->getParam("mb", true)->value().toInt();
      config->setModbusBaudRate(baud);
      dbgln("[webserver] saved modbus baud rate");
    }
    if (request->hasParam("md", true)){
      auto data = request->getParam("md", true)->value().toInt();
      config->setModbusDataBits(data);
      dbgln("[webserver] saved modbus data bits");
    }
    if (request->hasParam("mp", true)){
      auto parity = request->getParam("mp", true)->value().toInt();
      config->setModbusParity(parity);
      dbgln("[webserver] saved modbus parity");
    }
    if (request->hasParam("ms", true)){
      auto stop = request->getParam("ms", true)->value().toInt();
      config->setModbusStopBits(stop);
      dbgln("[webserver] saved modbus stop bits");
    }
    if (request->hasParam("mr", true)){
      auto rts = request->getParam("mr", true)->value().toInt();
      config->setModbusRtsPin(rts);
      dbgln("[webserver] saved modbus rts pin");
    }
    if (request->hasParam("sb", true)){
      auto baud = request->getParam("sb", true)->value().toInt();
      config->setSerialBaudRate(baud);
      dbgln("[webserver] saved serial baud rate");
    }
    if (request->hasParam("sd", true)){
      auto data = request->getParam("sd", true)->value().toInt();
      config->setSerialDataBits(data);
      dbgln("[webserver] saved serial data bits");
    }
    if (request->hasParam("sp", true)){
      auto parity = request->getParam("sp", true)->value().toInt();
      config->setSerialParity(parity);
      dbgln("[webserver] saved serial parity");
    }
    if (request->hasParam("ss", true)){
      auto stop = request->getParam("ss", true)->value().toInt();
      config->setSerialStopBits(stop);
      dbgln("[webserver] saved serial stop bits");
    }
    if (request->hasParam("wp", true)){
      String wp = request->getParam("wp", true)->value();
      if (!wp.equals(WEB_PASS_PLACEHOLDER)) { // if we get default value prefilled in the wp input we're not changing current one
        config->setWebPassword(wp);
        dbgln("[webserver] saved web password");
      } else {
        dbgln("[webserver] web password not changed");
      }
    }
    request->redirect("/");    
  });
  server->on("/debug", HTTP_GET, [config](AsyncWebServerRequest *request){
    
    ADMIN_WEB_PASS;

    dbgln("[webserver] GET /debug");
    auto *response = request->beginResponseStream("text/html");
    sendResponseHeader(response, "Debug");
    sendDebugForm(response, "1", "1", "3", "1");
    sendButton(response, "Back", "/");
    sendResponseTrailer(response);
    request->send(response);
  });
  server->on("/debug", HTTP_POST, [rtu, config](AsyncWebServerRequest *request){
    
    ADMIN_WEB_PASS;

    dbgln("[webserver] POST /debug");
    String slaveId = "1";
    if (request->hasParam("slave", true)){
      slaveId = request->getParam("slave", true)->value();
    }
    String reg = "1";
    if (request->hasParam("reg", true)){
      reg = request->getParam("reg", true)->value();
    }
    String func = "3";
    if (request->hasParam("func", true)){
      func = request->getParam("func", true)->value();
    }
    String count = "1";
    if (request->hasParam("count", true)){
      count = request->getParam("count", true)->value();
    }
    auto *response = request->beginResponseStream("text/html");
    sendResponseHeader(response, "Debug");
    response->print("<pre>");
    auto previous = LOGDEVICE;
    auto previousLevel = MBUlogLvl;
    auto debug = WebPrint(previous, response);
    LOGDEVICE = &debug;
    MBUlogLvl = LOG_LEVEL_DEBUG;
    ModbusMessage answer = rtu->syncRequest(0xdeadbeef, slaveId.toInt(), func.toInt(), reg.toInt(), count.toInt());
    MBUlogLvl = previousLevel;
    LOGDEVICE = previous;
    response->print("</pre>");
    auto error = answer.getError();
    if (error == SUCCESS){
      auto count = answer[2];
      response->print("<span >Answer: 0x");
      for (size_t i = 0; i < count; i++)
      {
        response->printf("%02x", answer[i + 3]);
      }      
      response->print("</span>");
    }
    else{
      response->printf("<span class=\"e\">Error: %#02x (%s)</span>", error, ErrorName(error).c_str());
    }
    sendDebugForm(response, slaveId, reg, func, count);
    sendButton(response, "Back", "/");
    sendResponseTrailer(response);
    request->send(response);
  });
  server->on("/update", HTTP_GET, [config](AsyncWebServerRequest *request){
    
    ADMIN_WEB_PASS;

    dbgln("[webserver] GET /update");
    auto *response = request->beginResponseStream("text/html");
    sendResponseHeader(response, "Firmware Update");
    response->print("<form method=\"post\" enctype=\"multipart/form-data\">"
      "<input type=\"file\" name=\"file\" id=\"file\" required/>"
      "<p></p>"
      "<button class=\"r\">Upload</button>"
      "</form>"
      "<p></p>");
    sendButton(response, "Back", "/");
    sendResponseTrailer(response);
    request->send(response);
  });
  server->on("/update", HTTP_POST, [config](AsyncWebServerRequest *request){
    
    ADMIN_WEB_PASS;

    request->onDisconnect([](){
      ESP.restart();
    });
    dbgln("[webserver] OTA finished");
    if (Update.hasError()){
      auto *response = request->beginResponse(500, "text/plain", "Ota failed");
      response->addHeader("Connection", "close");
      request->send(response);
    }
    else{
      auto *response = request->beginResponseStream("text/html");
      response->addHeader("Connection", "close");
      sendResponseHeader(response, "Firmware Update", true);
      response->print("<p>Update successful.</p>");
      sendButton(response, "Back", "/");
      sendResponseTrailer(response);
      request->send(response);
    }
  }, [&](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
    
    ADMIN_WEB_PASS;

    dbg("[webserver] OTA progress ");dbgln(index);
    if (!index) {
      //TODO add MD5 Checksum and Update.setMD5
      int cmd = (filename == "filesystem") ? U_SPIFFS : U_FLASH;
      if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd)) { // Start with max available size
        Update.printError(Serial);
        return request->send(400, "text/plain", "OTA could not begin");
      }
    }
    // Write chunked data to the free sketch space
    if(len){
      if (Update.write(data, len) != len) {
        return request->send(400, "text/plain", "OTA could not write data");
      }
    }
    if (final) { // if the final flag is set then this is the last frame of data
      if (!Update.end(true)) { //true to set the size to the current progress
        Update.printError(Serial);
        return request->send(400, "text/plain", "Could not end OTA");
      }
    }else{
      return;
    }
  });
  server->on("/wifi", HTTP_GET, [config](AsyncWebServerRequest *request){
    
    ADMIN_WEB_PASS;

    dbgln("[webserver] GET /wifi");
    auto *response = request->beginResponseStream("text/html");
    sendResponseHeader(response, "WiFi reset");
    response->print("<p class=\"e\">"
        "This will delete the stored WiFi config<br/>"
        "and restart the ESP in AP mode.<br/> Are you sure?"
      "</p>");
    sendButton(response, "Back", "/");
    response->print("<p></p>"
      "<form method=\"post\">"
        "<button class=\"r\">Yes, do it!</button>"
      "</form>");    
    sendResponseTrailer(response);
    request->send(response);
  });
  server->on("/wifi", HTTP_POST, [wm, config](AsyncWebServerRequest *request){
    
    ADMIN_WEB_PASS;

    dbgln("[webserver] POST /wifi");
    request->redirect("/");
    wm->erase();
    dbgln("[webserver] erased wifi config");
    dbgln("[webserver] rebooting...");
    ESP.restart();
    dbgln("[webserver] rebooted...");
  });
  server->on("/favicon.ico", [](AsyncWebServerRequest *request){
    dbgln("[webserver] GET /favicon.ico");
    request->send(204);//TODO add favicon
  });
  server->on("/style.css", [config](AsyncWebServerRequest *request){
    
    ADMIN_WEB_PASS;

    if (request->hasHeader("If-None-Match")){
      auto header = request->getHeader("If-None-Match");
      if (header->value() == String(ETAG)){
        request->send(304);
        return;
      }
    }
    dbgln("[webserver] GET /style.css");
    auto *response = request->beginResponseStream("text/css");
    sendMinCss(response);
    response->print(
    "button.r{"
	    "background: #d43535;"
    "}"
    "button.r:hover{"
	    "background: #931f1f;"
    "}"
    "table{"
      "text-align:left;"
      "width:100%;"
    "}"
    "input{"
      "width:100%;"
    "}"
    ".e{"
      "color:red;"
    "}"
    "pre{"
      "text-align:left;"
    "}"
    );
    response->addHeader("ETag", ETAG);
    request->send(response);
  });
  server->onNotFound([config](AsyncWebServerRequest *request){
    
    ADMIN_WEB_PASS;

    dbg("[webserver] request to ");dbg(request->url());dbgln(" not found");
    request->send(404, "text/plain", "404");
  });
}

void sendMinCss(AsyncResponseStream *response){
  response->print("body{"    
      "font-family:sans-serif;"
	    "text-align: center;"
      "background: #252525;"
	    "color: #faffff;"
    "}"
    "#content{"
	    "display: inline-block;"
	    "min-width: 340px;"
    "}"
    "button{"
	    "width: 100%;"
	    "line-height: 2.4rem;"
	    "background: #1fa3ec;"
	    "border: 0;"
	    "border-radius: 0.3rem;"
	    "font-size: 1.2rem;"
      "-webkit-transition-duration: 0.4s;"
      "transition-duration: 0.4s;"
	    "color: #faffff;"
    "}"
    "button:hover{"
	    "background: #0e70a4;"
    "}");
}

void sendResponseHeader(AsyncResponseStream *response, const char *title, bool inlineStyle){
    response->print("<!DOCTYPE html>"
      "<html lang=\"en\" class=\"\">"
      "<head>"
      "<meta charset='utf-8'>"
      "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1,user-scalable=no\"/>");
    response->printf("<title>ESP32 Modbus Gateway - %s</title>", title);
    if (inlineStyle){
      response->print("<style>");
      sendMinCss(response);
      response->print("</style>");
    }
    else{
      response->print("<link rel=\"stylesheet\" href=\"style.css\">");
    }
    response->print(
      "</head>"
      "<body>"
      "<h2>ESP32 Modbus Gateway</h2>");
    response->printf("<h3>%s</h3>", title);
    response->print("<div id=\"content\">");
}

void sendResponseTrailer(AsyncResponseStream *response){
    response->print("</div></body></html>");
}

void sendButton(AsyncResponseStream *response, const char *title, const char *action, const char *css){
    response->printf(
      "<form method=\"get\" action=\"%s\">"
        "<button class=\"%s\">%s</button>"
      "</form>"
      "<p></p>", action, css, title);
}

void sendTableRow(AsyncResponseStream *response, const char *name, String value){
    response->printf(
      "<tr>"
        "<td>%s:</td>"
        "<td>%s</td>"
      "</tr>", name, value.c_str());
}

void sendTableRow(AsyncResponseStream *response, const char *name, uint32_t value){
    response->printf(
      "<tr>"
        "<td>%s:</td>"
        "<td>%d</td>"
      "</tr>", name, value);
}

void sendDebugForm(AsyncResponseStream *response, String slaveId, String reg, String function, String count){
    response->print("<form method=\"post\">");
    response->print("<table>"
      "<tr>"
        "<td>"
          "<label for=\"slave\">Slave ID</label>"
        "</td>"
        "<td>");
    response->printf("<input type=\"number\" min=\"0\" max=\"247\" id=\"slave\" name=\"slave\" value=\"%s\">", slaveId.c_str());
    response->print("</td>"
        "</tr>"
        "<tr>"
          "<td>"
            "<label for=\"func\">Function</label>"
          "</td>"
          "<td>");
    response->printf("<select id=\"func\" name=\"func\" data-value=\"%s\">", function.c_str());
    response->print("<option value=\"1\">01 Read Coils</option>"
              "<option value=\"2\">02 Read Discrete Inputs</option>"
              "<option value=\"3\">03 Read Holding Register</option>"
              "<option value=\"4\">04 Read Input Register</option>"
            "</select>"
          "</td>"
        "</tr>"
        "<tr>"
          "<td>"
            "<label for=\"reg\">Register</label>"
          "</td>"
          "<td>");
    response->printf("<input type=\"number\" min=\"0\" max=\"65535\" id=\"reg\" name=\"reg\" value=\"%s\">", reg.c_str());
    response->print("</td>"
        "</tr>"
        "<tr>"
          "<td>"
            "<label for=\"count\">Count</label>"
          "</td>"
          "<td>");
    response->printf("<input type=\"number\" min=\"0\" max=\"65535\" id=\"count\" name=\"count\" value=\"%s\">", count.c_str());
    response->print("</td>"
        "</tr>"
      "</table>");
    response->print("<button class=\"r\">Send</button>"
      "</form>"
      "<p></p>");
    response->print("<script>"
      "(function(){"
        "var s = document.querySelectorAll('select[data-value]');"
        "for(d of s){"
          "d.querySelector(`option[value='${d.dataset.value}']`).selected=true"
      "}})();"
      "</script>");
}

const String ErrorName(Modbus::Error code)
{
    switch (code)
    {
        case Modbus::Error::SUCCESS: return "Success";
        case Modbus::Error::ILLEGAL_FUNCTION: return "Illegal function";
        case Modbus::Error::ILLEGAL_DATA_ADDRESS: return "Illegal data address";
        case Modbus::Error::ILLEGAL_DATA_VALUE: return "Illegal data value";
        case Modbus::Error::SERVER_DEVICE_FAILURE: return "Server device failure";
        case Modbus::Error::ACKNOWLEDGE: return "Acknowledge";
        case Modbus::Error::SERVER_DEVICE_BUSY: return "Server device busy";
        case Modbus::Error::NEGATIVE_ACKNOWLEDGE: return "Negative acknowledge";
        case Modbus::Error::MEMORY_PARITY_ERROR: return "Memory parity error";
        case Modbus::Error::GATEWAY_PATH_UNAVAIL: return "Gateway path unavailable";
        case Modbus::Error::GATEWAY_TARGET_NO_RESP: return "Gateway target no response";
        case Modbus::Error::TIMEOUT: return "Timeout";
        case Modbus::Error::INVALID_SERVER: return "Invalid server";
        case Modbus::Error::CRC_ERROR: return "CRC error";
        case Modbus::Error::FC_MISMATCH: return "Function code mismatch";
        case Modbus::Error::SERVER_ID_MISMATCH: return "Server id mismatch";
        case Modbus::Error::PACKET_LENGTH_ERROR: return "Packet length error";
        case Modbus::Error::PARAMETER_COUNT_ERROR: return "Parameter count error";
        case Modbus::Error::PARAMETER_LIMIT_ERROR: return "Parameter limit error";
        case Modbus::Error::REQUEST_QUEUE_FULL: return "Request queue full";
        case Modbus::Error::ILLEGAL_IP_OR_PORT: return "Illegal ip or port";
        case Modbus::Error::IP_CONNECTION_FAILED: return "IP connection failed";
        case Modbus::Error::TCP_HEAD_MISMATCH: return "TCP header mismatch";
        case Modbus::Error::EMPTY_MESSAGE: return "Empty message";
        case Modbus::Error::ASCII_FRAME_ERR: return "ASCII frame error";
        case Modbus::Error::ASCII_CRC_ERR: return "ASCII crc error";
        case Modbus::Error::ASCII_INVALID_CHAR: return "ASCII invalid character";
        default: return "undefined error";
    }
}

// translate RSSI to quality string
const String WiFiQuality(int rssiValue)
{
    switch (rssiValue)
    {
        case -30 ... 0: return "Amazing"; 
        case -67 ... -31: return "Very Good"; 
        case -70 ... -68: return "Okay"; 
        case -80 ... -71: return "Not Good"; 
        default: return "Unusable";
    }
}
