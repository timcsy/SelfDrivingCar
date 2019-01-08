#ifndef FETCH_H
#define FETCH_H

#include "LWiFi.h"

void game_server_setup(String server, String ID, String team);
void fetch_game_server(String command);
String get_fetch_result();

#endif