#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <ArduinoJson.h>

void controller();
void controller_setup();
void hard_coding(int path[], int size);
void drive(int Vl, int Vr, int ms = -1);
void straight(int direction, int V, int ms, double threshold = 0.97);
void go_init();
void go_right();
void go_left();
void go_center();
void go_step();
void go_until();
int parallel(int direction, bool clockwise = true, bool leave = false, double threshold = 0.8, int V = 255);
int quarter_adj(int round, bool clockwise = true, int ms = 100, int V = 255);
void turn(int direction);
void drive_control();
void ultrasound();
void light(int red, int green, int blue);
void connection_setup();
void game_server_parser(String res);
void get_position();
void get_command();

#endif
