#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <ArduinoJson.h>

void controller(String data);
String hook();
String drive(JsonArray& cmd);
void drive_control();
String upload();
void ultrasound();
void light(int red, int green, int blue);
void connection_setup();
void game_server_parser(String res);
void get_position();

#endif