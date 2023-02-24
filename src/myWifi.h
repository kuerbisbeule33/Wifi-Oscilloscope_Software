#pragma once

#include <WiFi.h>

extern unsigned long maxTryConnectTime; //ms //after that wifi connect counts as failure and Access Point gets opened

extern const IPAddress apIp;//ip to access webserver if configured as Access Point
extern const IPAddress gateway;//mostly router address
extern const IPAddress subnetMask;
extern const char *apSsid;//Wifi network name of Access Point
extern const char *apPasswort;//wifi network password of Access Point
extern const char *hostName;//host name to identify in Network

bool needAp();//try saved wifi credentials //if wifi access not possible request Access Point
void initAp();//setup Access Point