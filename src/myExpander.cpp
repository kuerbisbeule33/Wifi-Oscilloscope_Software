#include <stdint.h>
#include <Wire.h>
#include <SparkFun_TCA9534.h>

#include "myExpander.h"

myExpander::myExpander(uint8_t sclk, uint8_t mosi, uint8_t csGainCH1, uint8_t csGainCH2, uint8_t charging, uint8_t acSelCH1, uint8_t acSelCH2, uint8_t status)
    : sclk{sclk}, mosi{mosi}, csGainCH1{csGainCH1}, csGainCH2{csGainCH2}, 
    charging{charging}, acSelCH1{acSelCH1}, acSelCH2{acSelCH2}, status{status}
{
    for (uint8_t i = 0; i < NUM_GPIO; ++i)
    {
        // set pin mode
        if (i == charging)
        {
            currentPinMode[i] = GPIO_IN;
        }
        else
        {
            currentPinMode[i] = GPIO_OUT;
            gpioVal[i] = LOW;
        }
    }
    myGPIO.pinMode(currentPinMode);
}

// gain only 1, 2, 4, 5, 8, 10, 16, 32 //0 means no new gain
// channel 1 or 2
bool myExpander::setGain(uint8_t gain, uint8_t channel)
{
    //nor ready for new gain, still sending old gain
    if (this->gainCommand == 0){
        return false;
    }

    //temporary variables
    uint16_t gainCommand = 0;
    uint8_t gainBits = 0;

    //set bits for gain value
    switch (gain)
    {
    case 1:
        gainBits = B000;
        break;
    case 2:
        gainBits = B001;
        break;
    case 4:
        gainBits = B010;
        break;
    case 5:
        gainBits = B011;
        break;
    case 8:
        gainBits = B100;
        break;
    case 10:
        gainBits = B101;
        break;
    case 16:
        gainBits = B110;
        break;
    case 32:
        gainBits = B111;
        break;
    default:
        return false;
    }

    //check if channel is valid
    if (channel == 1 || channel == 2){
        gainCommand = 1 << 14;    // specify "write to register" command
        gainCommand |= gainBits; // select gain

        //strore data to set gain bits on everey expander communication
        this->gainCommand = gainCommand;
        this->channel = channel;
        return true;
    }

    //wrong input
    return false;
}

void myExpander::AcSelCH1(bool coupleAcOnCH1)
{
    this->gpioVal[acSelCH1] = coupleAcOnCH1;
}

void myExpander::AcSelCH2(bool coupleAcOnCH2)
{
    this->gpioVal[acSelCH2] = coupleAcOnCH2;
}
// returns the charging status
// update all on expander, therfore must be called in every loop
bool myExpander::update(){ 
    this->myGPIO.digitalReadPort(this->gpioVal);//get current values on expander
    this->gpioVal[this->status] = not this->gpioVal[this->status];//invert status every time for pwm
    writeGain();//update gain selection (if no new value, then makes nothing)
    this->myGPIO.digitalWrite(this->gpioVal);//change values on expander
    return this->gpioVal[this->charging];//return charging status
}

void myExpander::writeGain()
{
    static int8_t bitCounter = 0;
    static bool tick = false;

    //start
    if(this->gainCommand != 0 && bitCounter == -10){//new gain that must be send and trasmission is ready
        bitCounter == 15;
        if (channel == 1){
            gpioVal[this->csGainCH1] = LOW;//Chip select low to start
        }
        else if (channel == 2) {
            gpioVal[this->csGainCH2] = LOW;//Chip select low to start
        }
        gpioVal[this->sclk] = LOW;
        gpioVal[this->mosi] = (this->gainCommand >> bitCounter) & 0x1; //write 15. bit (MSB first)
        tick = false;
    }

    //middle (send data exept first bit)
    if (bitCounter >= 0 && bitCounter < 15){ //middle of data is there
        gpioVal[this->sclk] = not gpioVal[this->sclk]; //invert Clock every time
        tick = not tick; //invert tick for data
        if (tick == false){ //write data every second clock tick
            --bitCounter;//new bit value index (msb first)
            gpioVal[this->mosi] = (this->gainCommand >> bitCounter) & 0x1; //write next bit (MSB first)
        }
    }

    //end (no data send, go to idle with clk and cs)
    if (bitCounter == -1){
        gpioVal[this->sclk] = LOW;
        if (channel == 1){
            gpioVal[this->csGainCH1] = HIGH;//Chip select high to stop
        }
        else if (channel == 2) {
            gpioVal[this->csGainCH2] = HIGH;//Chip select high to stop
        }

        this->gainCommand = 0;//delete command on finish
        bitCounter = -10;//indicates transmission end
    }
}
