#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <DNSServer.h>
#include "globales.h"

extern AsyncWebServer webServer;
extern AsyncWebSocket ws;
extern DNSServer dnsServer;//redirect to website on wifi connection


class CaptiveRequestHandler : public AsyncWebHandler
{
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}
  bool canHandle(AsyncWebServerRequest *request)
  {
    request->addInterestingHeader("ANY");
    return true;
  }
  void handleRequest(AsyncWebServerRequest *request)
  {
    request->send(LittleFS, "/index.html", "text/html");
  }
};

//callback for websocket Event
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
//callback for valid message from callback
void handleWebSocketMessage(void *arg, uint8_t *msg, size_t len, uint32_t clientID);
void msgToDataPair(uint8_t *msg, String &id, String &data);
void initHttpRequests();//setup callbacks for HTTP Get and HTTP Post requests

