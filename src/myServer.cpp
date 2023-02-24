#include "myServer.h"
#include "myConversion.h"
#include "myFiles.h"

AsyncWebServer webServer(80);
AsyncWebSocket ws("/ws");

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    //callback for event of socket
    switch (type) {
    case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
    case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
    case WS_EVT_DATA:
        //message from socket recived and valid
        Serial.println("client data from '" + String(client->id()) + "' : " + String((char *)data));
        handleWebSocketMessage(arg, data, len, client->id());//callback with valid message
        break;
    }
}

//handle valid websocket message
void handleWebSocketMessage(void *arg, uint8_t *msg, size_t len, uint32_t clientID) {
    String id;
    String data;
    // parse string to json and extract data
    msgToDataPair(msg, id, data);

    //all data for main paige when new loaded requested
    if (data == "getAll") {   } 
}

/*
void initHttpRequests() {
    //requested file not there error
    //webServer.onNotFound([](AsyncWebServerRequest *request){
    //request->send(404, "text/plain", "The content you are looking for was not found."); });

    // website content
    // home and root
    webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
                     { request->send(SPIFFS, "/index.html", "text/html"); });
    webServer.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request)
                     { request->send(SPIFFS, "/index.html", "text/html"); });
    webServer.on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request)
                     { request->send(SPIFFS, "/index.js", "text/js"); });                     
    // libraries
    // bootstrap
    webServer.on("/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest *request)
                     { request->send(SPIFFS, "/bootstrap.min.css", "text/css"); });
    webServer.on("/bootstrap.min.js", HTTP_GET, [](AsyncWebServerRequest *request)
                     { request->send(SPIFFS, "/bootstrap.min.js", "text/js"); });
    // chartist
    webServer.on("/chartist.min.css", HTTP_GET, [](AsyncWebServerRequest *request)
                     { request->send(SPIFFS, "/bootstrap.min.css", "text/css"); });
    webServer.on("/chartist.min.js", HTTP_GET, [](AsyncWebServerRequest *request)
                     { request->send(SPIFFS, "/bootstrap.min.js", "text/js"); });
    // jquery
    webServer.on("/jquery-3.63.min.js", HTTP_GET, [](AsyncWebServerRequest *request)
                     { request->send(SPIFFS, "/bootstrap.min.js", "text/js"); });
    // images
    webServer.on("/logo.ico", HTTP_GET, [](AsyncWebServerRequest *request)
                     { request->send(SPIFFS, "/logo.ico", "image/ico"); });

    // Http post with new wifi Login data
    webServer.on("/wifiData", HTTP_POST, [](AsyncWebServerRequest *request) {
        //variables to save data from post
        String ssid;
        String password;
        String ip;
        //loop over parameters from post method
        size_t params = request->params();//number of params from post request
        Serial.println("new wifi credentials with " + String(params) + "parameters send");
        for(size_t i = 0; i < params; i++) {
            AsyncWebParameter* p = request->getParam(i);//load one parameter from parameter list
            // HTTP POST ssid value
            if (p->name() == "ssid") {
                ssid = p->value().c_str();
                Serial.print("SSID set to: ");
                Serial.println(ssid);
            }
            // HTTP POST pass value
            if (p->name() == "password") {
                password = p->value().c_str();
                Serial.print("Password set to: ");
                Serial.println(password);
            }
            // HTTP POST ip value
            if (p->name() == "ip") {
                ip = p->value().c_str();
                Serial.print("IP Address set to: ");
                Serial.println(ip);
                }
        }
        //reply for website
        request->send(200, "text/plain", "Done. ESP will restart, connect to your router and go to IP address: " + ip);
        //convert to json, save and restart
        jsonDoc.clear();
        jsonDoc["ssid"] = ssid;
        jsonDoc["password"] = password;
        jsonDoc["ip"] = ip;
        loadGlobalJsonToFile(loginPath);
        ESP.restart();
    });
}
*/


    