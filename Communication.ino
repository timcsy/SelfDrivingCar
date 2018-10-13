#include "fetch.h"
#include "config.h"

char ssid[] = WIFI_SSID; //  your network SSID (name)
char pass[] = WIFI_PASS; // your network password (use for WPA, or use as key for WEP)

void setup() {
	Serial.begin(115200);
	wifi_connect(ssid, pass);
}

void loop() {
	fetch(GET, "/secret");
	while (true);
}