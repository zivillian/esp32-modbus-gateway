#include "pages.h"

#define ETAG "\"" __DATE__ "" __TIME__ "\""

#if defined(ETHERNET)
void setupPages(AsyncWebServer *server, ModbusClientRTU *rtu, ModbusBridgeWiFi *bridge, Config *config){
#else
void setupPages(AsyncWebServer *server, ModbusClientRTU *rtu, ModbusBridgeWiFi *bridge, Config *config, WiFiManager *wm){
#endif
  server->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    LOG_D("[webserver] GET /\n");
    auto *response = request->beginResponseStream("text/html");
    sendResponseHeader(response, "Main");
    sendButton(response, "Status", "status");
    sendButton(response, "Config", "config");
    sendButton(response, "Debug", "debug");
    sendButton(response, "Firmware update", "update");
#if !defined(ETHERNET)
    sendButton(response, "WiFi reset", "wifi", "r");
#endif
    sendButton(response, "Reboot", "reboot", "r");
    sendResponseTrailer(response);
    request->send(response);
  });
  server->on("/status", HTTP_GET, [rtu, bridge](AsyncWebServerRequest *request){
    LOG_D("[Webserver] GET /status\n");
    auto *response = request->beginResponseStream("text/html");
    sendResponseHeader(response, "Status");
    response->print("<table>");

    // show ESP infos...
    sendTableRow(response, "ESP Uptime (sec)", esp_timer_get_time() / 1000000);
    response->print("<tr><td>&nbsp;</td><td></td></tr>");
#if defined(ETHERNET)
    sendTableRow(response, "ESP Full Duplex", ETH.fullDuplex());
    sendTableRow(response, "ESP Link Speed", ETH.linkSpeed());
    sendTableRow(response, "ESP MAC", ETH.macAddress());
    sendTableRow(response, "ESP IP",  ETH.localIP().toString() );
#else
    sendTableRow(response, "ESP SSID", WiFi.SSID());
    sendTableRow(response, "ESP RSSI", WiFi.RSSI());
    sendTableRow(response, "ESP WiFi Quality", WiFiQuality(WiFi.RSSI()));
    sendTableRow(response, "ESP MAC", WiFi.macAddress());
    sendTableRow(response, "ESP IP",  WiFi.localIP().toString() );
#endif
    response->print("<tr><td>&nbsp;</td><td></td></tr>");
    sendTableRow(response, "RTU Messages", rtu->getMessageCount());
    sendTableRow(response, "RTU Pending Messages", rtu->pendingRequests());
    sendTableRow(response, "RTU Errors", rtu->getErrorCount());
    response->print("<tr><td>&nbsp;</td><td></td></tr>");
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
  server->on("/reboot", HTTP_GET, [](AsyncWebServerRequest *request){
    LOG_D("[Webserver] GET /reboot\n");
    auto *response = request->beginResponseStream("text/html");
    sendResponseHeader(response, "Really?");
    sendButton(response, "Back", "/");
    response->print("<form method=\"post\">"
        "<button class=\"r\">Yes, do it!</button>"
      "</form>");
    sendResponseTrailer(response);
    request->send(response);
  });
  server->on("/reboot", HTTP_POST, [](AsyncWebServerRequest *request){
    LOG_D("[Webserver] POST /reboot\n");
    request->redirect("/");
    LOG_D("[Webserver] rebooting...");
    ESP.restart();
    LOG_D("[Webserver] rebooted...");
  });
  server->on("/config", HTTP_GET, [config](AsyncWebServerRequest *request){
    LOG_D("[Webserver] GET /config\n");
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
        "<tr>"
          "<td colspan=\"2\"><h3>Modbus RTU</h3></td>"
        "</tr>"
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
        "<tr>"
          "<td>"
            "<label for=\"rt\">RTU Timeout (ms)</label>"
          "</td>"
          "<td>");
    response->printf("<input type=\"number\" min=\"1\" id=\"rt\" name=\"rt\" value=\"%d\">", config->getRtuTimeout());
    response->print("</td>"
        "</tr>"
        "<tr>"
          "<td colspan=\"2\"><h3>Serial (Debug)</h3></td>"
        "</tr>"
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
        "</table>"
        "<p></p>");
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
    LOG_D("[Webserver] POST /config\n");
    if (request->hasParam("tp", true)){
      auto port = request->getParam("tp", true)->value().toInt();
      config->setTcpPort(port);
      LOG_D("[Webserver] Saved TCP Port: %ld\n", port);
    }
    if (request->hasParam("tt", true)){
      auto tcpTimeout = request->getParam("tt", true)->value().toInt();
      config->setTcpTimeout(tcpTimeout);
      LOG_D("[Webserver] Saved TCP Timeout: %ld\n", tcpTimeout);
    }
    if (request->hasParam("mb", true)){
      auto baud = request->getParam("mb", true)->value().toInt();
      config->setModbusBaudRate(baud);
      LOG_D("[Webserver] Saved Modbus Baud rate: %ld\n", baud);
    }
    if (request->hasParam("md", true)){
      auto data = request->getParam("md", true)->value().toInt();
      config->setModbusDataBits(data);
      LOG_D("[Webserver] Saved Modbus Data bits: %ld\n", data);
    }
    if (request->hasParam("mp", true)){
      auto parity = request->getParam("mp", true)->value().toInt();
      config->setModbusParity(parity);
      LOG_D("[Webserver] Saved Modbus Parity: %ld\n", parity);
    }
    if (request->hasParam("ms", true)){
      auto stop = request->getParam("ms", true)->value().toInt();
      config->setModbusStopBits(stop);
      LOG_D("[Webserver] Saved Modbus Stop bits: %ld\n", stop);
    }
    if (request->hasParam("mr", true)){
      auto rts = request->getParam("mr", true)->value().toInt();
      config->setModbusRtsPin(rts);
      LOG_D("[Webserver] Saved Modbus RTS pin: %ld\n", rts);
    }
    if (request->hasParam("rt", true)){
      auto rtuTimeout = request->getParam("rt", true)->value().toInt();
      config->setRtuTimeout(rtuTimeout);
      LOG_D("[Webserver] Saved RTU Timeout: %ld\n", rtuTimeout);
    }
    if (request->hasParam("sb", true)){
      auto baud = request->getParam("sb", true)->value().toInt();
      config->setSerialBaudRate(baud);
      LOG_D("[Webserver] Saved Debug Serial Baud rate: %ld\n", baud);
    }
    if (request->hasParam("sd", true)){
      auto data = request->getParam("sd", true)->value().toInt();
      config->setSerialDataBits(data);
      LOG_D("[Webserver] Saved Debug Serial Data bits: %ld\n", data);
    }
    if (request->hasParam("sp", true)){
      auto parity = request->getParam("sp", true)->value().toInt();
      config->setSerialParity(parity);
      LOG_D("[Webserver] Saved Debug Serial Parity: %ld\n", parity);
    }
    if (request->hasParam("ss", true)){
      auto stop = request->getParam("ss", true)->value().toInt();
      config->setSerialStopBits(stop);
      LOG_D("[Webserver] Saved Debug Serial Stop bits: %ld\n", stop);
    }
    request->redirect("/");    
  });
  server->on("/debug", HTTP_GET, [](AsyncWebServerRequest *request){
    LOG_D("[Webserver] GET /debug\n");
    auto *response = request->beginResponseStream("text/html");
    sendResponseHeader(response, "Debug");
    sendDebugForm(response, "1", "1", "3", "1");
    sendButton(response, "Back", "/");
    sendResponseTrailer(response);
    request->send(response);
  });
  server->on("/debug", HTTP_POST, [rtu](AsyncWebServerRequest *request){
    LOG_D("[Webserver] POST /debug\n");
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
    response->print("</div id=\"content\"><pre>");
    auto previous = LOGDEVICE;
    auto previousLevel = MBUlogLvl;
    auto debug = WebPrint(previous, response);
    LOGDEVICE = &debug;
    MBUlogLvl = LOG_LEVEL_DEBUG;
    ModbusMessage answer = rtu->syncRequest(0xdeadbeef, slaveId.toInt(), func.toInt(), reg.toInt(), count.toInt());
    MBUlogLvl = previousLevel;
    LOGDEVICE = previous;
    response->print("</pre><div id=\"content\">");
    auto error = answer.getError();
    if (error == SUCCESS){
      auto count = answer[2];
      response->print("<p>Answer: 0x");
      for (size_t i = 0; i < count; i++)
      {
        response->printf("%02x", answer[i + 3]);
      }      
      response->print("</p>");
    }
    else{
      response->printf("<p class=\"e\">Error: %#02x (%s)</p>", error, ErrorName(error).c_str());
    }
    sendDebugForm(response, slaveId, reg, func, count);
    sendButton(response, "Back", "/");
    sendResponseTrailer(response);
    request->send(response);
  });
  server->on("/update", HTTP_GET, [](AsyncWebServerRequest *request){
    LOG_D("[Webserver] GET /update\n");
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
  server->on("/update", HTTP_POST, [](AsyncWebServerRequest *request){
    request->onDisconnect([](){
      ESP.restart();
    });
    LOG_D("[Webserver] OTA finished\n");
    if (Update.hasError()) {
      auto *response = request->beginResponse(500, "text/plain", "Ota failed");
      response->addHeader("Connection", "close");
      request->send(response);
    }
    else
    {
      auto *response = request->beginResponseStream("text/html");
      response->addHeader("Connection", "close");
      sendResponseHeader(response, "Firmware Update", true);
      response->print("<p>Update successful.</p>");
      sendButton(response, "Back", "/");
      sendResponseTrailer(response);
      request->send(response);
    }
  }, [&](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
    static size_t lastIndex = 0;
    if(index == 0) {
      lastIndex = 0;
    }
    if(index - lastIndex > 32 * 1024)
    {
      lastIndex = index;
      LOG_D("[webserver] OTA progress %d\n", index);
    }
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
#if !defined(ETHERNET)
  server->on("/wifi", HTTP_GET, [](AsyncWebServerRequest *request){
    LOG_D("[Webserver] GET /wifi\n");
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
  server->on("/wifi", HTTP_POST, [wm](AsyncWebServerRequest *request){
    LOG_D("[Webserver] POST /wifi\n");
    request->redirect("/");
    wm->erase();
    LOG_D("[Webserver] erased wifi config\n");
    LOG_D("[Webserver] rebooting...\n");
    ESP.restart();
    LOG_D("[Webserver] rebooted...\n");
  });
#endif
  server->on("/favicon.ico", [](AsyncWebServerRequest *request){
    LOG_D("[Webserver] GET /favicon.ico\n");
    request->send(204);//TODO add favicon
  });
  server->on("/style.css", [](AsyncWebServerRequest *request){
    if (request->hasHeader("If-None-Match")){
      auto header = request->getHeader("If-None-Match");
      if (header->value() == String(ETAG)){
        request->send(304);
        return;
      }
    }
    LOG_D("[Webserver] GET /style.css\n");
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
      "width:max-content;"
      "margin:auto;"
    "}"
    );
    response->addHeader("ETag", ETAG);
    request->send(response);
  });
  server->onNotFound([](AsyncWebServerRequest *request){
    LOG_D("[webserver] request to %s not found\n", request->url().c_str());
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
    "}"
    "table>tr>td>h3{"
      "text-align: center;"
    "}"
    );
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
      "</table>"
      "<p></p>");
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
