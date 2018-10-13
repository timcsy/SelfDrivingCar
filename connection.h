#ifndef CONNECTION_H
#define CONNECTION_H

#include <LWiFi.h>

void wifi_connect(char * ssid, char * pass);
void printWifiStatus();

#endif