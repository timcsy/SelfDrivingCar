#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <ArduinoJson.h>

void controller(String data);
String hook();
String drive(JsonArray& cmd);
void drive_control();
String upload();
void ultrasound();

#endif