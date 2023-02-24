#include "myWifi.h"

#include "myFiles.h"
#include "myConversion.h"

//timeout for wifi login try
unsigned long maxTryConnectTime = 10000;

//wifi configuration
const IPAddress apIp(192, 168, 0, 1);//ip to access webserver when access point
const IPAddress gateway(192, 168, 0,1);//mostly router address
const IPAddress subnetMask(255, 255, 255, 0);
const char *apSsid = "Oscilloscope";
const char *apPasswort = "Oscilloscope";
const char *hostName = "Oscilloscope";

// check for configured wifi
bool needAp() {
    if (loadFileToGlobalJson(loginPath)){//data could be loaded from file
        String ip = jsonDoc["ip"];
        String ssid = jsonDoc["ssid"];
        Serial.println("In File saved ip:" + ip + ":");
        Serial.println("In File saved ssid:" + ssid + ":");
        if (ssid == "null" || ssid == "" || ip == "null" || ip == "") {//chekc for invalid settings saved
            Serial.println("Undefined SSID or IP address");
            return true;//setings are invalid, request access point
        }
        else{//try connect to wifi (settings where correct)
            WiFi.mode(WIFI_STA);//station mode (login to wifi)
            IPAddress ipAddr;
            ipAddr.fromString(ip);//cast ip address
            if (!WiFi.config(ipAddr, gateway, subnetMask)){//configure static ip
                Serial.println("STA Failed to configure");
                return true;//cinfiguration falied, therfore open access point
            }
            WiFi.setHostname(hostName);//identifier in network
            String passwort = jsonDoc["password"];
            WiFi.begin(ssid.c_str(), passwort.c_str());//try to login to saved wifi
            Serial.println("Connecting to WiFi...");

            // try to connect timeout configuraiton
            unsigned long currentMillis = millis();
            unsigned long previousMillis = currentMillis;

            //check if connected
            while (WiFi.status() != WL_CONNECTED){//check if connected
                currentMillis = millis();
                if (currentMillis - previousMillis >= maxTryConnectTime){//check for timout (could not connect)
                    Serial.println("Failed to connect");
                    return true;//therfore open access point
                }
            }
        }
        //successfuly connected to wifi
        Serial.println(WiFi.localIP());
        return false;
    }
    Serial.println("Could not open ssid and passwort config file");
    return true;//no configuration saved
}

void initAp() {
    Serial.println("Setting up Access Point"); 
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIp, apIp, subnetMask);//confiugre staitc ip of access point
    WiFi.softAP(apSsid, apPasswort);//configure wifi login
    IPAddress IP = WiFi.softAPIP();//configured ip, if .softAPConfig dont work
    Serial.print("AP IP address: ");
    Serial.println(IP);
}