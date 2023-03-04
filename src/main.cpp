#include <Arduino.h>
#include <DNSServer.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include "myServer.h"
#include "myConversion.h"

//wifi configuration
const IPAddress apIp(192, 168, 0, 1);//ip to access webserver when access point
const IPAddress gateway(192, 168, 0,1);//mostly router address
const IPAddress subnetMask(255, 255, 255, 0);
const char *apSsid = "Oscilloscope";
const char *apPasswort = "Oscilloscope";

DNSServer dnsServer;
String user_name;
String proficiency;
bool name_received = false;
bool proficiency_received = false;

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
    request->send(SPIFFS, "/index.html", "text/html");
  }
};

float sin1[10] = {0, 1, 2, 1, 0, -1, -2, -1, 0, 1};
float cos1[10] = {2, 1, 0, -1, -2, -1, 0, 1, 2, 1};
uint32_t start;
bool sinState = false;

void setup()
{
  Serial.begin(115200);
  //Serial.setDebugOutput(false);
  SPIFFS.begin();
  Serial.println("Setting up Access Point"); 
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIp, apIp, subnetMask);//confiugre staitc ip of access point
  WiFi.softAP(apSsid, apPasswort);//configure wifi login
  IPAddress IP = WiFi.softAPIP();//configured ip, if .softAPConfig dont work
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // webserver initialisation
  Serial.println("Starting DNS Server");
  dnsServer.start(53, "*", WiFi.softAPIP());
  ws.onEvent(onEvent); // callback for websocket event
  webServer.addHandler(&ws);
  initHttpRequests();
  webServer.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
  webServer.serveStatic("/", SPIFFS, "/");
  webServer.begin();
  Serial.println("All Done!");

  //test code
  start = millis();
}

void loop()
{
  dnsServer.processNextRequest();
  ws.cleanupClients();

  //test code
  if (millis() - start > 1000){
    start = millis();
    jsonDoc.clear();
    JsonArray jsonArray = jsonDoc.createNestedArray("CH1");
    for (int i = 0; i < 10; i++) {
      if (sinState){
        jsonArray.add(sin1[i]);
      }
      else{
        jsonArray.add(cos1[i]);
      }
    }
    sinState = not sinState;
    JsonArray jsonArray2 = jsonDoc.createNestedArray("CH2");
    for (int i = 0; i < 10; i++) {
      if (sinState){
        jsonArray2.add(sin1[i]);
      }
      else{
        jsonArray2.add(cos1[i]);
      }
    }
    String msg;
    serializeJson(jsonDoc, msg);

    ws.textAll(msg);
  }
}