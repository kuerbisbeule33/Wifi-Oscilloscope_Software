#include <stdint.h>
#include <Wire.h>
#include <SparkFun_TCA9534.h>

#define NUM_GPIO 8

class myExpander
{
private:
    TCA9534 myGPIO;

    bool currentPinMode[NUM_GPIO];
    bool gpioVal[NUM_GPIO];

    // pins
    const uint8_t SCLK, MOSI, CS_GAIN_CH1, CS_GAIN_CH2;
    const uint8_t CHARGING, CH1_AC_SEL, CH2_AC_SEL, STATUS;

    uint16_t gainCommand;
    uint8_t channel;
    bool coupleAcOnCH1, coupleAcOnCH2;

public:
    myExpander(uint8_t SCLK, uint8_t MOSI, uint8_t CS_GAIN_CH1, uint8_t CS_GAIN_CH2, uint8_t CHARGING, uint8_t CH1_AC_SEL, uint8_t CH2_AC_SEL);
    bool setGain(uint8_t gain, uint8_t channel);
    void writeGain();
    void AcSelCH1(bool coupleAcOnCH1);
    void AcSelCH2(bool coupleAcOnCH2);
    bool update();
};

myExpander::myExpander(uint8_t SCLK, uint8_t MOSI, uint8_t CS_GAIN_CH1, uint8_t CS_GAIN_CH2, uint8_t CHARGING, uint8_t CH1_AC_SEL, uint8_t CH2_AC_SEL)
    : myGPIO{}, SCLK{SCLK}, MOSI{MOSI}, CS_GAIN_CH1{CS_GAIN_CH1}, CS_GAIN_CH2{CS_GAIN_CH2}, CHARGING{CHARGING}, CH1_AC_SEL{CH1_AC_SEL}, CH2_AC_SEL{CH2_AC_SEL}
{
    for (uint8_t i = 0; i < NUM_GPIO; ++i)
    {
        // set pin mode
        if (i == CHARGING)
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
    this->gpioVal[CH1_AC_SEL] = coupleAcOnCH1;
}

void myExpander::AcSelCH2(bool coupleAcOnCH2)
{
    this->gpioVal[CH1_AC_SEL] = coupleAcOnCH1;
}
// returns the charging status
// update all on expander, therfore must be called in every loop
bool myExpander::update(){ 
    this->myGPIO.digitalReadPort(this->gpioVal);//get current values on expander
    this->gpioVal[this->STATUS] = not this->gpioVal[this->STATUS];//invert status every time for pwm
    writeGain();//update gain selection (if no new value, then makes nothing)
    this->myGPIO.digitalWrite(this->gpioVal);//change values on expander
    return this->gpioVal[this->CHARGING];//return charging status
}

void myExpander::writeGain()
{
    static int8_t bitCounter = 0;
    static bool tick = false;

    //start
    if(this->gainCommand != 0 && bitCounter == -10){//new gain that must be send and trasmission is ready
        bitCounter == 15;
        if (channel == 1){
            gpioVal[this->CH1_AC_SEL] = LOW;//Chip select low to start
        }
        else if (channel == 2) {
            gpioVal[this->CH2_AC_SEL] = LOW;//Chip select low to start
        }
        gpioVal[this->SCLK] = LOW;
        gpioVal[this->MOSI] = (this->gainCommand >> bitCounter) & 0x1; //write 15. bit (MSB first)
        tick = false;
    }

    //middle (send data exept first bit)
    if (bitCounter >= 0 && bitCounter < 15){ //middle of data is there
        gpioVal[this->SCLK] = not gpioVal[this->SCLK]; //invert Clock every time
        tick = not tick; //invert tick for data
        if (tick == false){ //write data every second clock tick
            --bitCounter;//new bit value index (msb first)
            gpioVal[this->MOSI] = (this->gainCommand >> bitCounter) & 0x1; //write next bit (MSB first)
        }
    }

    //end (no data send, go to idle with clk and cs)
    if (bitCounter == -1){
        gpioVal[this->SCLK] = LOW;
        if (channel == 1){
            gpioVal[this->CH1_AC_SEL] = HIGH;//Chip select high to stop
        }
        else if (channel == 2) {
            gpioVal[this->CH2_AC_SEL] = HIGH;//Chip select high to stop
        }

        this->gainCommand = 0;//delete command on finish
        bitCounter = -10;//indicates transmission end
    }
}
