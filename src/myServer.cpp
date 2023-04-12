#include "myServer.h"

AsyncWebServer webServer(80);
AsyncWebSocket ws("/ws");

void i8ToStringNoTerm(int8_t number, char* string, uint16_t startIndex) {
    uint8_t minusIndex = 0;
    bool negative = false;
    bool firstIsSet = false;
    string[startIndex] = ' ';
    string[startIndex + 4] = ',';
    // abs and save minus
    if (number < 0) {
        number = -number;
        negative = true;
    }
    // 100 number
    uint8_t val1 = number / 100;
    if (val1 == 0) {
        ++minusIndex;
        string[startIndex + 1] = ' ';
    }
    else {
        string[startIndex + 1] = val1 + '0';//erste stelle
        firstIsSet = true;
    }
    // 10 number
    number = number - val1 * 100;
    uint8_t val2 = number / 10;
    if (val2 == 0 && !firstIsSet) {
        ++minusIndex;
        string[startIndex + 2] = ' ';
    }
    else {
        string[startIndex + 2] = val2 + '0';//zweite stelle
    }
    // 1 number
    number = number - val2 * 10;
    string[startIndex + 3] = number + '0';//dritte stelle

    // minus, else fill empty
    if (negative) {
        string[startIndex + minusIndex] = '-';
    }
}

#define SendBlockOffset 8
#define SendCharBlockSize 5
#define samplesPerSend 1500
void sendArray(int16_t* data) {
    static char sendString[SendCharBlockSize * samplesPerSend + SendBlockOffset + 10] = "{\"CH1\":[";
    sendString[samplesPerSend * SendCharBlockSize + SendBlockOffset - 1] = ']';
    sendString[samplesPerSend * SendCharBlockSize + SendBlockOffset - 0] = '}';
    sendString[samplesPerSend * SendCharBlockSize + SendBlockOffset + 1] = '\0';
    int8_t* dataArray = (int8_t*)data;
    int8_t val;

    sendString[4] = '1';
    for (uint16_t i = 0; i < samplesPerSend; ++i) {
        val = dataArray[i * 2 + timeOffset];
        i8ToStringNoTerm(val, sendString, i * 5 + SendBlockOffset);
    }
    ws.textAll(sendString);

    sendString[4] = '2';
    for (uint16_t i = 0; i < samplesPerSend; ++i) {
        val = dataArray[i * 2 + 1 + timeOffset];
        i8ToStringNoTerm(val, sendString, i * 5 + SendBlockOffset);
    }
    ws.textAll(sendString);
}

//for small json conversion with identivier and data
String toJsonString(String id, int16_t data){
    //static json on stack is faster and has lower overhead
    //only works for small data
    StaticJsonDocument<500> doc;
    doc["id"] = id;
    doc["data"] = String(data);
    String msg;
    serializeJson(doc, msg);
    return msg;
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    //callback for event of socket
    switch (type) {
    case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        //for (uint16_t i = 0; i < samples; ++i){
            //dataArray[i] = 127;
        //}
        //sendArray(2, dataArray, 0.04, samples);   
        //ws.textAll("{\"test\":1}\n");
        //Serial.println("send data");
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
        sendArray(testSamples);  
        if (data == "start")
        {
            DEBUG_PRINTLN("ADC Sampler started with last trigger mode");
            trigger = lastTrigger;
        }
        if (data == "stop"){
	        DEBUG_PRINTLN("Set STOP trigger mode");
            lastTrigger = trigger;
	        trigger = stopTrig;
	        //adcSampler.cancel(); //stop current mode as fast as possible
        }    
    }
    else if (id == "trigger-mode"){
        switch (data.toInt()){
            case autoTrig:
            	DEBUG_PRINTLN("Set AUTO trigger mode");
	            trigger = autoTrig;
	            //adcSampler.cancel(); //stop current mode as fast as possible
            break;
            case singleTrig:
                DEBUG_PRINTLN("Set SINGLE trigger mode");
	            trigger = singleTrig;
	            //adcSampler.cancel(); //stop current mode as fast as possible
            break;
            case noneTrig:
	            DEBUG_PRINTLN("Set NONE trigger mode");
	            trigger = noneTrig;
	            //adcSampler.cancel(); //stop current mode as fast as possible
            break;
        }
    }
    else if (id == "trigger-edge"){
        switch (data.toInt()){
            case falling:
                DEBUG_PRINTLN("Set FALLING edge trigger mode");
	            edge = falling;
	            trigger = stopTrig;
	            //adcSampler.cancel(); //stop current mode as fast as possible
            break; 
            case rising:
            	DEBUG_PRINTLN("Set RISING edge trigger mode");
	            edge = rising;
	            trigger = stopTrig;
	            //adcSampler.cancel(); //stop current mode as fast as possible
            break;

        }
    }
    else if (id == "trigger-channel"){
        if (data == "CH1" ){
            DEBUG_PRINTLN("Set Channel 1 trigger source");
            //trigIN = trig1IN;

        }
        if (data == "CH2"){
            DEBUG_PRINTLN("Set Channel 1 trigger source");
            //trigIN = trig2IN;
        }

    }
    else if (id == "offset-CH1"){
        double valNorm = data.toDouble() * 255.0 /  MAX_VAL;
        //dacWrite(DAC_CH1, (uint8_t)valNorm);
    }
    else if (id == "offset-CH2"){
        double valNorm = data.toDouble() * 255.0 /  MAX_VAL;
        //dacWrite(DAC_CH2, (uint8_t)valNorm);
    }
    else if (id == "trigger"){
        double valNorm = data.toDouble() * 4096.0 /  MAX_VAL;
        //triggerDac.setVoltage((uint16_t)valNorm);
    }
    else if (id == "gain-CH1"){
        //expander.setGain((uint8_t)data.toInt(), 1);
    }
    else if (id == "gain-CH2"){
        //expander.setGain((uint8_t)data.toInt(), 2);
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
    // redirect to home on file not found (redirect on wifi connect with windows)
    webServer.onNotFound([](AsyncWebServerRequest *request)
        { request->send(LittleFS, "/index.html", "text/html"); });
    //redirect for android
    webServer.on("/generate_204", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(LittleFS, "/index.html", "text/html"); });
    // website content
    // home and root
    webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(LittleFS, "/index.html", "text/html"); });
    webServer.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(LittleFS, "/index.html", "text/html"); });
    webServer.on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(LittleFS, "/index.js", "text/js"); });
    // libraries
    // bootstrap
    webServer.on("/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(LittleFS, "/bootstrap.min.css", "text/css"); });
    // chartist
    webServer.on("/chartist.min.css", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(LittleFS, "/chartist.min.css", "text/css"); });
    webServer.on("/chartist.min.js", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(LittleFS, "/chartist.min.js", "text/js"); });
    // images
    webServer.on("/logo.png", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(LittleFS, "/logo.png", "image/png"); });
}