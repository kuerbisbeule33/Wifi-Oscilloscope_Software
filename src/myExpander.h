#pragma once

// inspirated from https://github.com/cversek/multimeter/blob/master/arduino/MultimeterLib/MCP6S91.cpp

#include <stdint.h>
#include <Wire.h>
#include <SparkFun_TCA9534.h>

#define NUM_GPIO 8

class myExpander
{
private:
    //expander
    TCA9534 myGPIO;
    bool currentPinMode[NUM_GPIO] = {};
    bool gpioVal[NUM_GPIO] = {};

    // pins
    const uint8_t sclk, mosi, csGainCH1, csGainCH2;
    const uint8_t charging, acSelCH1, acSelCH2, status;

    //spi gain selection
    uint16_t gainCommand = 0;
    uint8_t channel = 0;

    void writeGain();

public:
    myExpander(uint8_t sclk, uint8_t mosi, uint8_t csGainCH1, uint8_t csGainCH2, uint8_t charging, uint8_t acSelCH1, uint8_t acSelCH2, uint8_t status);
    bool setGain(uint8_t gain, uint8_t channel);
    void AcSelCH1(bool coupleAcOnCH1);
    void AcSelCH2(bool coupleAcOnCH2);
    bool update();
};