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
        //ws.textAll(toJsonString("data", 55));
        break;
    case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
    case WS_EVT_DATA:
        //message from socket recived and valid
        //Serial.println("client data from '" + String(client->id()) + "' : " + String((char *)data));
        handleWebSocketMessage(arg, data, len, client->id());//callback with valid message
        break;
    }
}

//handle valid websocket message
void handleWebSocketMessage(void *arg, uint8_t *msg, size_t len, uint32_t clientID) {
    String id;
    String data;
    msgToDataPair(msg, id, data);
    if (id == "trigger-run"){
            
    }
    else if (id == "trigger-mode"){

    }
    else if (id == "trigger-edge"){

    }
    else if (id == "trigger-channel"){

    }
    else if (id == "offset-CH1"){

    }
    else if (id == "offset-CH2"){

    }
    else if (id == "trigger"){

    }
    Serial.println("id: " + id + " | data: " + data);
}


void initHttpRequests() {
    //requested file not there error
    webServer.onNotFound([](AsyncWebServerRequest *request){
    request->send(404, "text/plain", "The content you are looking for was not found."); });

    // website content
    // home and root
    webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(SPIFFS, "/index.html", "text/html"); });
    webServer.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(SPIFFS, "/index.html", "text/html"); });
    // libraries
    // jquery
    webServer.on("/jquery-3.6.3.min.js", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(SPIFFS, "/jquery-3.6.3.min.js", "text/js"); });
    // bootstrap
    webServer.on("/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(SPIFFS, "/bootstrap.min.css", "text/css"); });
    webServer.on("/bootstrap.min.js", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(SPIFFS, "/bootstrap.min.js", "text/js"); });
    // chartist
    webServer.on("/chartist.min.css", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(SPIFFS, "/chartist.min.css", "text/css"); });
    webServer.on("/chartist.min.js", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(SPIFFS, "/chartist.min.js", "text/js"); });
    // images
    webServer.on("/logo.ico", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(SPIFFS, "/logo.ico", "image/ico"); });
}