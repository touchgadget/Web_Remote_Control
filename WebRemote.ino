/*
   MIT License

   Copyright (c) 2021 touchgadgetdev@gmail.com

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

#define DEBUG_ON 0
#if DEBUG_ON
#define Dbg_begin(...)  Serial.begin(__VA_ARGS__)
#define Dbg_print(...)  Serial.print(__VA_ARGS__)
#define Dbg_println(...)  Serial.println(__VA_ARGS__)
#define Dbg_printf(...)  Serial.printf(__VA_ARGS__)
#else
#define Dbg_begin(...)
#define Dbg_print(...)
#define Dbg_println(...)
#define Dbg_printf(...)
#endif

#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ESP8266mDNS.h>
#endif  // ESP8266
#if defined(ESP32)
#include <ESPmDNS.h>
#include <WebServer.h>
#include <WiFi.h>
#include <Update.h>
#endif  // ESP32
#include <WiFiClient.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <IRsend.h>
#include "index_html.h"
#include "xbox_media.h"

#if defined(ESP8266)
ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
#endif  // ESP8266
#if defined(ESP32)
WebServer server(80);
#endif  // ESP32
MDNSResponder mdns;
WiFiClient espClient;
WiFiManager wifiManager;

// ESP GPIO pin to use. Recommended: 4 (D2).
IRsend irsend(4); // Set the GPIO to be used to sending the message.

#define HOSTNAME "WebRemote"

void handleRoot(void){
  server.send(200, "text/html", INDEX_HTML);
}

void handleNotFound(void) {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i < server.args(); i++)
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  server.send(404, "text/plain", message);
}

void Json_touch_grid(uint8_t client, Touch_Page_t &page)
{
  // Build remote control grid for web socket client. Encoded in JSON format.
  String json = "[";
  for (int r = 0; r < page.rows; r++) {
    json.concat('[');
    for (int c = 0; c < page.cols; c++) {
      Touch_IRcode_t *ir = (Touch_IRcode_t *)page.IR_cells + ((r * page.cols) + c);
      if (ir->cell_name != NULL) {
        Dbg_printf("c = %d, r = %d, cell_name = %s colSpan = %d\n", c, r, ir->cell_name, ir->colSpan);
        if (c != 0) json.concat(',');
        json.concat("{\"cellLabel\":\"");
        json.concat(ir->cell_name);
        json.concat('"');
        if (ir->colSpan > 0) {
          json.concat(",\"colSpan\":");
          json.concat(ir->colSpan);
        }
        json.concat('}');
      }
    }
    if (r == (page.rows - 1)) {
      json.concat(']');
    }
    else {
      json.concat("],");
    }
  }
  json.concat(']');
  Dbg_println(json);
  webSocket.sendTXT(client, json.c_str(), json.length());
}

void sendIRcode(Touch_Page_t &page, uint8_t row, uint8_t col)
{
  Dbg_printf("sendIRCode (%d,%d)\n", row, col);
  if (row >= page.rows) {
    Dbg_printf("row=%d too high\n", row);
    row = page.rows - 1;
  }
  if (col >= page.cols) {
    Dbg_printf("col=%d too high\n", col);
    col = page.cols - 1;
  }
  Touch_IRcode_t *ir = (Touch_IRcode_t *)page.IR_cells + ((row * page.cols) + col);
  Dbg_printf("type= %d code= 0x%llx\n", ir->IRType, ir->IRCode);
  if (!irsend.send(ir->IRType, ir->IRCode, ir->nbits, ir->repeat)) {
    Dbg_println(F("sendIRCode fail"));
  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length)
{
  Dbg_printf("webSocketEvent(%d, %d, ...)\r\n", num, type);
  switch(type) {
    case WStype_DISCONNECTED:
      Dbg_printf("[%u] Disconnected!\r\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Dbg_printf("[%u] Connected from %d.%d.%d.%d url: %s\r\n", num, ip[0], ip[1], ip[2], ip[3], payload);

        // Send touch grid in JSON format
        Json_touch_grid(num, XBox_Media);
      }
      break;
    case WStype_TEXT:
      {
        Dbg_printf("[%u] get Text: %s\r\n", num, payload);

        StaticJsonDocument<64> doc;

        DeserializationError error = deserializeJson(doc, payload, length);

        if (error) {
          Dbg_print(F("deserializeJson() failed: "));
          Dbg_println(error.f_str());
          return;
        }

        const char * event = doc["event"];
        if (strcmp(event, "touch start") == 0) {
          sendIRcode(XBox_Media, doc["row"], doc["col"]);
        }
      }
      break;
    case WStype_BIN:
      Dbg_printf("[%u] get binary length: %u\r\n", num, length);
      hexdump(payload, length);
      break;
    default:
      Dbg_printf("Invalid WStype [%d]\r\n", type);
      break;
  }
}

void setup()
{
  //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;

  WiFi.mode(WIFI_STA);
  irsend.begin();

  Dbg_begin(115200);
  bool res = wm.autoConnect("WebRemote");
  if(!res) {
    Dbg_println(F("Failed to connect"));
    delay(1000);
    ESP.restart();
  }

#if defined(ESP8266)
  if (mdns.begin(HOSTNAME, WiFi.localIP())) {
#else  // ESP8266
  if (mdns.begin(HOSTNAME)) {
#endif  // ESP8266
    Dbg_println(F("MDNS responder started"));
    mdns.addService("http", "tcp", 80); // Announce http tcp service on port 80
    mdns.addService("ws", "tcp", 81); // Announce web sock tcp service on port 81
  }

  // Start web socket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  // Media Remote Control is the home page
  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.begin();
}

void loop()
{
#if defined(ESP8266)
  mdns.update();
#endif
  webSocket.loop();
  server.handleClient();
}
