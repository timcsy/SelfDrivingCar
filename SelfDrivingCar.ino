#include <LTimer.h>
#include "connection.h"
#include "controller.h"
#include "config.h"
#include "server.h"

char ssid[] = WIFI_SSID; //  your network SSID (name)
char pass[] = WIFI_PASS; // your network password (use for WPA, or use as key for WEP)

void setup() {
	Serial.begin(115200);

//  server_setup();

	pinMode(R_LED, OUTPUT);
	pinMode(G_LED, OUTPUT);
	pinMode(B_LED, OUTPUT);

	pinMode(leftpin1, OUTPUT);
	pinMode(leftpin2, OUTPUT);
	pinMode(rightpin1, OUTPUT);
	pinMode(rightpin2, OUTPUT);

	pinMode(timerpinin, INPUT);
	pinMode(timerpinout, OUTPUT);
	int is_short;
	do {
		is_short = 0;
		digitalWrite(timerpinout, HIGH);
		delay(100);
		if (digitalRead(timerpinin) == HIGH) is_short++;
		digitalWrite(timerpinout, LOW);
		delay(100);
		if (digitalRead(timerpinin) == LOW) is_short++;
	} while (is_short == 2);
	
	wifi_connect(ssid, pass);
	light(0, 0, 255);
	connection_setup();
	controller_setup();
}

void loop() {
	controller();
}
