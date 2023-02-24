#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

//global Json for big conversion (used for all to avoid memory defragmentation)
extern DynamicJsonDocument jsonDoc;

void loadStringToGlobalJson(String &data);

//helper function for short Json String converison
String toJsonString(String id, int16_t data);
void msgToDataPair(uint8_t *msg, String &id, String &data);



