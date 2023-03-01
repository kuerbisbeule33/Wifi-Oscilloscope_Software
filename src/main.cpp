#include <Arduino.h>
#include <DNSServer.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

#include "myWifi.h"
#include "myServer.h"

DNSServer dnsServer;
String user_name;
String proficiency;
bool name_received = false;
bool proficiency_received = false;

/*
void setupServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
    Serial.println("Client Connected"); });
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request) {
    String inputMessage; String inputParam;
    if (request->hasParam("name")) {
    inputMessage = request->getParam("name")->value();
    inputParam = "name";
    user_name = inputMessage;
    Serial.println(inputMessage);
    name_received = true; }
    if (request->hasParam("proficiency")) {
      inputMessage = request->getParam("proficiency")->value();
      inputParam = "proficiency";
      proficiency = inputMessage; Serial.println(inputMessage);
      proficiency_received = true; }
      request->send(200, "text/html", "The values entered by you have been successfully sent to the device <br><a href=\"/\">Return to Home Page</a>"); });
}
*/

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

void setup()
{
  Serial.begin(115200);
  //Serial.setDebugOutput(false);
  SPIFFS.begin();
  if (needAp())
  {
    initAp();
  }

  // webserver initialisation
  Serial.println("Starting DNS Server");
  dnsServer.start(53, "*", WiFi.softAPIP());
  
  initHttpRequests();

  webServer.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
  ws.onEvent(onEvent); // callback for websocket event
  webServer.addHandler(&ws);
  webServer.serveStatic("/", SPIFFS, "/");
  webServer.begin();
  Serial.println("All Done!");
}



void loop()
{
  dnsServer.processNextRequest();
  ws.cleanupClients();
}