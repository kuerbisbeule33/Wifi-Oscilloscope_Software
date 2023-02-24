#pragma once

#include <Arduino.h>

extern const char *loginPath;//path of wifi config
extern const char *mappingPath;//path for IO mapping config
extern const char *inputNamePath;//path for input name mapping config
extern const char *outputNamePath;//path for output name config

bool loadFileToGlobalJson(const char* path);
void loadGlobalJsonToFile(const char* path);
void deleteAllConfigFiles();