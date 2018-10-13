#include <Arduino.h>
#include "controller.h"
#include "fetch.h"

enum OPCODE {};

StaticJsonBuffer<1024> jsonBuffer;

void start() {
	String data = fetch(GET, "/api/v1");
	JsonObject& res = jsonBuffer.parseObject(data);

  // Test if parsing succeeds.
  if (!res.success()) {
    Serial.println("parseObject() failed");
    return;
  }

	controller(res);
}

void controller(JsonObject& response) {
	int op = response["op"];
	Serial.println(op);
}