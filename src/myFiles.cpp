#include "myFiles.h"

#include <SPIFFS.h>
#include "myConversion.h"

//config files
const char *loginPath = "/login.json";

bool loadFileToGlobalJson(const char* path){
    File file = SPIFFS.open(path);
    if (file){// file was there
        //load data from file to globale json object
        jsonDoc.clear();
        deserializeJson(jsonDoc, file);
        file.close();
        return true;
    }
    return false;
}

void loadGlobalJsonToFile(const char* path) {
    //stores data from global json object to config file
    File file = SPIFFS.open(path, "w");
    serializeJson(jsonDoc, file);
    file.close();
}

void deleteAllConfigFiles(){
    //delete all configuration files for factory settings
    Serial.println("remove all config files");
    SPIFFS.remove(loginPath);
}