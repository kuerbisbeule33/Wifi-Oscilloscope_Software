#include "myServer.h"

AsyncWebServer webServer(80);
AsyncWebSocket ws("/ws");
DNSServer dnsServer;//redirect to website on wifi connection

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
    TRIGGER_MODE_E lastTrigger;
    msgToDataPair(msg, id, data);
    if (id == "trigger-run"){
        if (data == "start")
        {
            DEBUG_PRINTLN("ADC Sampler started with last trigger mode");
            trigger = lastTrigger;
        }
        if (data == "stop"){
	        DEBUG_PRINTLN("Set STOP trigger mode");
            lastTrigger = trigger;
	        trigger = stopTrig;
	        adcSampler.cancel(); //stop current mode as fast as possible
        }
            
    }
    else if (id == "trigger-mode"){
        switch (data.toInt()){
            case autoTrig:
            	DEBUG_PRINTLN("Set AUTO trigger mode");
	            trigger = autoTrig;
	            adcSampler.cancel(); //stop current mode as fast as possible
            break;
            case singleTrig:
                DEBUG_PRINTLN("Set SINGLE trigger mode");
	            trigger = singleTrig;
	            adcSampler.cancel(); //stop current mode as fast as possible
            break;
            case noneTrig:
	            DEBUG_PRINTLN("Set NONE trigger mode");
	            trigger = noneTrig;
	            adcSampler.cancel(); //stop current mode as fast as possible
            break;
        }

    }
    else if (id == "trigger-edge"){
        switch (data.toInt()){
            case falling:
                DEBUG_PRINTLN("Set FALLING edge trigger mode");
	            edge = falling;
	            trigger = stopTrig;
	            adcSampler.cancel(); //stop current mode as fast as possible
            break; 
            case rising:
            	DEBUG_PRINTLN("Set RISING edge trigger mode");
	            edge = rising;
	            trigger = stopTrig;
	            adcSampler.cancel(); //stop current mode as fast as possible
            break;

        }

    }
    else if (id == "trigger-channel"){
        if (data == "CH1" ){
            DEBUG_PRINTLN("Set Channel 1 trigger source");
            trigIN = trig1IN;

        }
        if (data == "CH2"){
            DEBUG_PRINTLN("Set Channel 1 trigger source");
            trigIN = trig2IN;
        }

    }
    else if (id == "offset-CH1"){
        double valNorm = data.toDouble() * 255.0 /  MAX_VAL;
        dacWrite(DAC_CH1, (uint8_t)valNorm);
    }
    else if (id == "offset-CH2"){
        double valNorm = data.toDouble() * 255.0 /  MAX_VAL;
        dacWrite(DAC_CH2, (uint8_t)valNorm);
    }
    else if (id == "trigger"){
        double valNorm = data.toDouble() * 4096.0 /  MAX_VAL;
        triggerDac.setVoltage((uint16_t)valNorm);
    }
    else if (id == "gain-CH1"){
        expander.setGain((uint8_t)data.toInt(), 1);
    }
    else if (id == "gain-CH2"){
        expander.setGain((uint8_t)data.toInt(), 2);
    }
    else if (id == "CH1-Probe"){
        PRESCALER_CH1 = data.toDouble();
    }
    else if (id == "CH2-Probe"){
        PRESCALER_CH2 = data.toDouble();
    }
    Serial.println("id: " + id + " | data: " + data);
}

//converts msg string from Socket to id and data
void msgToDataPair(uint8_t *msg, String &id, String &data){
    //static json on stack is faster and has lower overhead
    //only works for small data
    StaticJsonDocument<256> doc;
    deserializeJson(doc, msg);
    //get identiver and data of recived message
    String idTemp = doc["id"];
    id = idTemp;
    String dataTemp = doc["data"];
    data = dataTemp;
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
    webServer.on("/logo.png", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(SPIFFS, "/logo.png", "image/png"); });
}