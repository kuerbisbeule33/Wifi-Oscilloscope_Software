#pragma once

#include <stdint.h>
#include <Adafruit_MAX1704X.h>
#include <MCP4726.h>
#include "myExpander.h"

extern const uint8_t SCLK_SPI;
extern const uint8_t MOSI_EXPAND;
extern const uint8_t CS_GAIN_CH1;
extern const uint8_t CS_GAIN_CH2;
extern const uint8_t CH1_AC_SEL;
extern const uint8_t CH2_AC_SEL;
extern const uint8_t STATUS_LED;
extern const uint8_t CHARGING;

extern Adafruit_MAX17048 lipo;
extern float cellPercent;
extern myExpander expander;

extern const uint8_t DAC_CH1;
extern const uint8_t DAC_CH2;
extern MCP4726 triggerDac;

extern double MAX_VAL;
extern double PRESCALER_CH1;
extern double PRESCALER_CH2;

//wifi configuration
extern const IPAddress apIp;
extern const IPAddress gateway;
extern const IPAddress subnetMask;
extern const char *apSsid;
extern const char *apPasswort;