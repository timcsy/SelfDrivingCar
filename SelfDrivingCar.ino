#include <LTimer.h>
#include "connection.h"
#include "controller.h"
#include "config.h"

char ssid[] = WIFI_SSID; //  your network SSID (name)
char pass[] = WIFI_PASS; // your network password (use for WPA, or use as key for WEP)

LTimer timer0(LTIMER_0);
LTimer timer1(LTIMER_1);

// callback function for timer0
void _callback0(void *usr_data) {
  ultrasound();
}

// callback function for timer1
void _callback1(void *usr_data) {
  drive_control();
}

void setup() {
	Serial.begin(115200);

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
	
	timer0.begin();
	timer1.begin();
  // start the execution
  timer0.start(256, LTIMER_REPEAT_MODE, _callback0, NULL);
	timer1.start(200, LTIMER_REPEAT_MODE, _callback1, NULL);

	wifi_connect(ssid, pass);
}

void loop() {
	controller("{}");
}