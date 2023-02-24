#include "myConversion.h"

#include <SPIFFS.h>
#include "myFiles.h"

DynamicJsonDocument jsonDoc(1024);

void loadStringToGlobalJson(String &data){
    jsonDoc.clear();//make sure json is empty
    deserializeJson(jsonDoc, data);//load new data into global json object
}

//for small json conversion with identivier and data
String toJsonString(String id, int16_t data){
    //static json on stack is faster and has lower overhead
    //only works for small data
    StaticJsonDocument<128> doc;
    doc["id"] = id;
    doc["data"] = String(data);
    String msg;
    serializeJson(doc, msg);
    return msg;
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