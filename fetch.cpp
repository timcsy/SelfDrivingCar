#include <Arduino.h>
#include "fetch.h"
#include "controller.h"
#include "config.h"

WiFiClient tcp_client;

void game_server_setup(String server, String ID, String team) {
	Serial.println("\nStarting connection to game server...");
	while (!tcp_client.connect(server.c_str(), 5000));
	Serial.println("connected to game server");

	String command = "join " + ID + " " + team;
  delay(1000);
	tcp_client.write(command.c_str());  //join <ID> <team>
}

void fetch_game_server(String command) {
	tcp_client.write(command.c_str());
}

String get_fetch_result() {
	String response = "";
	while (tcp_client.available()) {
		char c = tcp_client.read();
		delayMicroseconds(10);
		response += c;
	}
	return response;
}
