#ifndef FETCH_H
#define FETCH_H

#include "LWiFi.h"

enum METHOD {GET, POST};

String fetch(int method, String url, String content = "", bool auth = true, String server = "car.tew.tw", int retry = 0);
void login(String username, String password);

#endif