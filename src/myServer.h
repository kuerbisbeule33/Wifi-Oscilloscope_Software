#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include "globales.h"

extern AsyncWebServer webServer;
extern AsyncWebSocket ws;

//callback for websocket Event
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
//callback for valid message from callback
void handleWebSocketMessage(void *arg, uint8_t *msg, size_t len, uint32_t clientID);
void msgToDataPair(uint8_t *msg, String &id, String &data);
void initHttpRequests();//setup callbacks for HTTP Get and HTTP Post requests

