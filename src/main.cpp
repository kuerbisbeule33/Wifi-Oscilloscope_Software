#include <Arduino.h>
#include <DNSServer.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <Wire.h>
#include <Adafruit_MAX1704X.h>
#include <MCP4726.h>
#include <driver/ledc.h>

#include "BufferedAdcSampler.h"
#include "globales.h"
#include "myServer.h"
#include "myExpander.h"

const int XCLK = 27;
const int PCLK = 26;
const int trig1IN = 15;
const int trig2IN = 13;
int trigIN = 15;
 

const int D_inputs[16] = {23,19,18,5,17,16,4,2,36,39,34,35,32,33,14,12}; //first byte ch1 / second byte ch2


//const char * triggerTexts[4] = {"stop\0", "none\0","auto\0","single\0"};
//const int triggerEdgeConfig[2] = {FALLING, RISING};
I2S_AdcSampler adcSampler;
char str[50000];
bool bufferIsFilled = false;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

bool ClockEnable(int pin, int Hz)
{
    periph_module_enable(PERIPH_LEDC_MODULE);

    ledc_timer_config_t timer_conf;
    timer_conf.bit_num = (ledc_timer_bit_t)1;
    timer_conf.freq_hz = Hz;
    timer_conf.speed_mode = LEDC_HIGH_SPEED_MODE;
    timer_conf.timer_num = LEDC_TIMER_0;
    esp_err_t err = ledc_timer_config(&timer_conf);
    if (err != ESP_OK) {
        return false;
    }

    ledc_channel_config_t ch_conf;
    ch_conf.channel = LEDC_CHANNEL_0;
    ch_conf.timer_sel = LEDC_TIMER_0;
    ch_conf.intr_type = LEDC_INTR_DISABLE;
    ch_conf.duty = 1;
    ch_conf.speed_mode = LEDC_HIGH_SPEED_MODE;
    ch_conf.gpio_num = pin;
	  ch_conf.hpoint = 0;
    err = ledc_channel_config(&ch_conf);
    if (err != ESP_OK) {
        return false;
    }
    return true;
}

void ClockDisable()
{
    periph_module_disable(PERIPH_LEDC_MODULE);
}

//trigger interrupt handler (we detect falling edge)
  void IRAM_ATTR handleInterrupt() {
  portENTER_CRITICAL_ISR(&mux);
  detachInterrupt(trigIN);
  adcSampler.stopNonBlocking(1);
  DEBUG_PRINTLN("Trigger detected!");
  portEXIT_CRITICAL_ISR(&mux);
}

/* send data points in responce to "/scope" request */
void updateADC() {
  int strPointer = 0;
	uint16_t samplesCount = 0;
  uint16_t samplePointer = 0;
  //check if we already have the data
	if((I2S_AdcSampler::READY == adcSampler.state()) &&	(trigger != stopTrig))
	{

		//the value before first comma is name of current trigger selected.
		//strPointer += sprintf(&str[strPointer], "%s,", triggerTexts[trigger]);

		//trigger fired, we have data in the buffer
		switch (trigger) {
			case autoTrig:
        sprintf;
				//buffer is already filled with data
				samplesCount = adcSampler.samplesNumber();
				DEBUG_PRINTLN("Buffer filled");
				break;
			case singleTrig:
				//buffer is already filled with data
				samplesCount = adcSampler.samplesNumber();
				trigger = stopTrig;//stop data acquisition
				DEBUG_PRINTLN("Buffer filled. Stop data acquisition");
				break;
			case noneTrig:
				//buffer is already filled with data
				samplesCount = adcSampler.samplesNumber();
				DEBUG_PRINTLN("Buffer filled");
				break;
			default: //unknown trigger
				samplesCount = 0;
				break;
		}
		//fill the buff with samples converted from bin to str
		for(uint16_t samplePointer = 0; samplePointer < samplesCount; samplePointer++) {
			strPointer += sprintf(&str[strPointer], "%u,", adcSampler.readSample(samplePointer));
			if(strPointer > sizeof(str)) {
				str[sizeof(str) - 1] = 0;
				break; //to prevent buffer overflow
			}
		}
		//start/restart data acquisition if needed
		if((autoTrig == trigger) || (singleTrig == trigger) || (noneTrig == trigger)) {
			DEBUG_PRINTLN("Repeat data acquisition");
			adcSampler.start(); //start data acquisition if needed
			if(noneTrig == trigger) {
				adcSampler.stopNonBlocking(3); //request acquisition stop after adc buffers are filled
			} else {
				DEBUG_PRINTLN("Wait for the next trigger");
				//GPIO.status_w1tc = BIT(trigIN); //clear interrupt flag
				attachInterrupt(digitalPinToInterrupt(trigIN), handleInterrupt, edge);
			}
		}
	} else {
		if (stopTrig != trigger) {
			if (I2S_AdcSampler::STOPPED == adcSampler.state()) {
				//We still waiting for trigger. Add one zero dot for empty plot
				strPointer += sprintf(&str[strPointer], "%s,", "0");
				DEBUG_PRINTLN("Start data acquisition");
				adcSampler.start(); //start data acquisition if needed
				//request acquisition stop after adc buffers are filled
				if(noneTrig == trigger) {
					adcSampler.stopNonBlocking(3);
				} else {
					//GPIO.status_w1tc = BIT(trigIN); //clear interrupt flag
					attachInterrupt(digitalPinToInterrupt(trigIN), handleInterrupt, edge);
				}
			}
		} else {
			//We still waiting for trigger. Add one zero dot for empty plot
			strPointer += sprintf(&str[strPointer], "%s,", "0");
		}
	}
}

void setup()
{
  Serial.begin(115200);
  //Serial.setDebugOutput(false);
  SPIFFS.begin();
  Serial.println("Setting up Access Point"); 
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIp, apIp, subnetMask);//confiugre staitc ip of access point
  WiFi.softAP(apSsid, apPasswort);//configure wifi login
  IPAddress IP = WiFi.softAPIP();//configured ip, if .softAPConfig dont work
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // webserver initialisation
  Serial.println("Starting DNS Server");
  dnsServer.start(53, "*", WiFi.softAPIP());
  ws.onEvent(onEvent); // callback for websocket event
  webServer.addHandler(&ws);
  initHttpRequests();
  webServer.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
  webServer.serveStatic("/", SPIFFS, "/");
  webServer.begin();
  Serial.println("All Done!");

  //other peripherals
  lipo.begin();
  triggerDac.begin(MCP4726_DEFAULT_ADDR);

}

void loop()
{
  dnsServer.processNextRequest();
  ws.cleanupClients();
  

  //lipo and expander update
  bool charging = expander.update();
  float newCellPercent = lipo.cellPercent();
  if (newCellPercent != cellPercent){
    StaticJsonDocument<128> doc;
    JsonArray jsonArray = doc.createNestedArray("battery");
    jsonArray.add(newCellPercent);
    jsonArray.add(charging);
    String msg;
    serializeJson(doc, msg);
    ws.textAll(msg);
  }
  cellPercent = newCellPercent;
  // check if buffer is full




  //test code
  /*
  if (millis() - start > 1000){
    start = millis();
    jsonDoc.clear();
    JsonArray jsonArray = jsonDoc.createNestedArray("CH1");
    for (int i = 0; i < 10; i++) {
      if (sinState){
        jsonArray.add(sin1[i]);
      }
      else{
        jsonArray.add(cos1[i]);
      }
    }
    sinState = not sinState;
    JsonArray jsonArray2 = jsonDoc.createNestedArray("CH2");
    for (int i = 0; i < 10; i++) {
      if (sinState){
        jsonArray2.add(sin1[i]);
      }
      else{
        jsonArray2.add(cos1[i]);
      }
    }
    String msg;
    serializeJson(jsonDoc, msg);

    ws.textAll(msg);
  }
  */
}