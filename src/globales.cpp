#include "globales.h"

bool shouldStart = false;

const uint8_t SCLK_SPI = 0;
const uint8_t MOSI_EXPAND = 1;
const uint8_t CS_GAIN_CH1 = 3;
const uint8_t CS_GAIN_CH2 = 5;
const uint8_t CH1_AC_SEL = 2;
const uint8_t CH2_AC_SEL = 4;
const uint8_t STATUS_LED = 7;
const uint8_t CHARGING = 6;

Adafruit_MAX17048 lipo;
float cellPercent;
myExpander expander(SCLK_SPI, MOSI_EXPAND, CS_GAIN_CH1, CS_GAIN_CH2, CHARGING, CH1_AC_SEL, CH2_AC_SEL, STATUS_LED);

const uint8_t DAC_CH1 = 25;
const uint8_t DAC_CH2 = 26;
MCP4726 triggerDac;

const double MAX_VAL = 5.0f;
uint16_t timeOffset = 0;
int16_t testSamples[1500];

//wifi configuration
const IPAddress apIp(8, 8, 8, 8);//ip to access webserver when access point
const IPAddress subnetMask(255, 255, 255, 0);
const char *apSsid = "Oscilloscope";
const char *apPasswort = "Oscilloscope";

TRIGGER_MODE_E trigger = stopTrig;
TRIGGER_EDGE_E edge = falling;