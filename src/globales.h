#pragma once

#include <stdint.h>
#include <Adafruit_MAX1704X.h>
#include <MCP4726.h>
#include "myExpander.h"
#include "BufferedAdcSampler.h"

#define DEBUG_LOG_ENABLE

extern bool shouldStart;

extern const uint8_t SCLK_SPI;
extern const uint8_t MOSI_EXPAND;
extern const uint8_t CS_GAIN_CH1;
extern const uint8_t CS_GAIN_CH2;
extern const uint8_t CH1_AC_SEL;
extern const uint8_t CH2_AC_SEL;
extern const uint8_t STATUS_LED;
extern const uint8_t CHARGING;

//extern Adafruit_MAX17048 lipo;
//extern float cellPercent;
//extern myExpander expander;

//extern const uint8_t DAC_CH1;
//extern const uint8_t DAC_CH2;
//extern MCP4726 triggerDac;

//extern I2S_AdcSampler adcSampler;
//extern int trigIN;
//extern const int trig1IN;
//extern const int trig2IN;

typedef enum {falling = 2, rising = 1} TRIGGER_EDGE_E;
typedef enum {stopTrig = 0, noneTrig = 1, autoTrig = 2, singleTrig = 3} TRIGGER_MODE_E;
extern TRIGGER_MODE_E trigger;
extern TRIGGER_EDGE_E edge;

extern const double MAX_VAL;
extern uint16_t timeOffset;
extern int16_t testSamples[];

//wifi configuration
extern const IPAddress apIp;
extern const IPAddress gateway;
extern const IPAddress subnetMask;
extern const char *apSsid;
extern const char *apPasswort;

//debugging print statements
#ifdef DEBUG_LOG_ENABLE
  #define DEBUG_PRINTLN(a) Serial.println(a)
  #define DEBUG_PRINT(a) Serial.print(a)
  #define DEBUG_PRINTLNF(a, f) Serial.println(a, f)
  #define DEBUG_PRINTF(a, f) Serial.print(a, f)
#else
  #define DEBUG_PRINTLN(a)
  #define DEBUG_PRINT(a)
  #define DEBUG_PRINTLNF(a, f)
  #define DEBUG_PRINTF(a, f)
#endif