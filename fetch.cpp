#include <Arduino.h>
#include "fetch.h"
#include "controller.h"
#include "config.h"

WiFiClient tcp_client;

// Initialize the Ethernet TLS_client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
TLSClient TLS_client;

String token = "";

String fetch(int method, String url, String content, bool auth, String server, int retry) {
	if (auth && token == "") login(CAR_USERNAME, CAR_PASSWORD);
	Serial.println("\nStarting connection to server...");
	if (TLS_client.connect(server.c_str(), 443)) {
		Serial.println("connected to server");
		// Make a HTTP request over SSL (HTTPS)
		if (method == GET) TLS_client.print("GET ");
		else if (method == POST) TLS_client.print("POST ");
		TLS_client.print(url);
		TLS_client.println(" HTTP/1.1");
		TLS_client.print("Host: ");
		TLS_client.println(server);
		TLS_client.println("Accept: */*");
		if (auth) TLS_client.println("Authorization: JWT " + token);
		// TLS_client.println("Connection: close");
		if (method == POST) TLS_client.print("Content-Length: ");
		if (method == POST) TLS_client.println(content.length());
		if (method == POST) TLS_client.println("Content-Type: application/json");
		TLS_client.println();
		TLS_client.println(content);
	}

	int is_head = 1, is_end = 0, length = 0;
	String STATUS = "", data = "";
	String currentLine = "";
	while (TLS_client.available()) {
		char c = TLS_client.read();
		// if there are incoming bytes available
		// from the server, read them and print them:
		// Serial.write(c);
		
		// Reading response data
		if (!is_head && !is_end) {
			data += c;
			length--;
			if (length == 0) is_end = 1;
		}

		if (is_end) {
			TLS_client.stop();
			Serial.println(STATUS);
			Serial.println(data);
			if (STATUS.startsWith("2")) {
				return data;
			} else if (STATUS.startsWith("401")) {
				// re-login
				if (auth) token = "";
				if (retry < 1) return fetch(method, url, content, auth, server, retry + 1);
				else return "";
			} else {
				if (retry < 1) return fetch(method, url, content, auth, server, retry + 1);
				else return "";
			}
		}

		if (c == '\n') { // if the byte is a newline character
			if (currentLine.startsWith("HTTP/1.1 ")) {
				STATUS = currentLine;
				STATUS.replace("HTTP/1.1 ", "");
				STATUS.trim();
			}
			if (currentLine.startsWith("Content-Length:")) {
				currentLine.replace("Content-Length:", "");
				currentLine.trim();
				length = currentLine.toInt();
			}
			// if the current line is blank, you got two newline characters in a row.
			// that's the end of the TLS_client HTTP request, so send a response:
			if (currentLine.length() == 0) {
				is_head = 0;
			} else { // if you got a newline, then clear currentLine:
				currentLine = "";
			}
		} else if (c != '\r') {  // if you got anything else but a carriage return character,
			currentLine += c;      // add it to the end of the currentLine
		}


		// if the server's disconnected, stop the TLS_client:
		if (!TLS_client.connected()) {
			Serial.println();
			Serial.println("disconnecting from server.");
			TLS_client.stop();

			// do nothing forevermore:
			// while (true);
			return "";
		}
	}
}

void login(String username, String password) {
	token = fetch(POST, "/api/v1/login", "{\"username\":\"" + username + "\",\"password\":\"" + password + "\"}", false);
}

void game_server_setup(String server, String ID, String team) {
	Serial.println("\nStarting connection to game server...");
	while (!tcp_client.connect(server.c_str(), 5000));
	Serial.println("connected to game server");

	String command = "join " + ID + " " + team;
	tcp_client.write(command.c_str());  //join <ID> <team>
}

String fetch_game_server(String command) {
	String response = "";
	tcp_client.write(command.c_str());
	while (tcp_client.available()) {
		char c = tcp_client.read();
		delayMicroseconds(10);
		response += c;
	}
	return response;
}

// WiFiClient& connect_game_server(String server) {
// 	Serial.println("\nStarting connection to server...");
// 	while (!tcp_client.connect(server.c_str(), 5000));
// 	Serial.println("connected to game server");
// 	return tcp_client;

// 	// int is_head = 1, is_end = 0, length = 0;
// 	// String STATUS = "", data = "";
// 	// String currentLine = "";
// 	// while (TLS_client.available()) {
// 	// 	char c = TLS_client.read();
// 	// 	// if there are incoming bytes available
// 	// 	// from the server, read them and print them:
// 	// 	// Serial.write(c);
		
// 	// 	// Reading response data
// 	// 	if (!is_head && !is_end) {
// 	// 		data += c;
// 	// 		length--;
// 	// 		if (length == 0) is_end = 1;
// 	// 	}

// 	// 	if (is_end) {
// 	// 		TLS_client.stop();
// 	// 		Serial.println(STATUS);
// 	// 		Serial.println(data);
// 	// 		if (STATUS.startsWith("2")) {
// 	// 			return data;
// 	// 		} else if (STATUS.startsWith("401")) {
// 	// 			// re-login
// 	// 			if (auth) token = "";
// 	// 			if (retry < 1) return fetch(method, url, content, auth, server, retry + 1);
// 	// 			else return "";
// 	// 		} else {
// 	// 			if (retry < 1) return fetch(method, url, content, auth, server, retry + 1);
// 	// 			else return "";
// 	// 		}
// 	// 	}

// 	// 	if (c == '\n') { // if the byte is a newline character
// 	// 		if (currentLine.startsWith("HTTP/1.1 ")) {
// 	// 			STATUS = currentLine;
// 	// 			STATUS.replace("HTTP/1.1 ", "");
// 	// 			STATUS.trim();
// 	// 		}
// 	// 		if (currentLine.startsWith("Content-Length:")) {
// 	// 			currentLine.replace("Content-Length:", "");
// 	// 			currentLine.trim();
// 	// 			length = currentLine.toInt();
// 	// 		}
// 	// 		// if the current line is blank, you got two newline characters in a row.
// 	// 		// that's the end of the TLS_client HTTP request, so send a response:
// 	// 		if (currentLine.length() == 0) {
// 	// 			is_head = 0;
// 	// 		} else { // if you got a newline, then clear currentLine:
// 	// 			currentLine = "";
// 	// 		}
// 	// 	} else if (c != '\r') {  // if you got anything else but a carriage return character,
// 	// 		currentLine += c;      // add it to the end of the currentLine
// 	// 	}


// 	// 	// if the server's disconnected, stop the TLS_client:
// 	// 	if (!TLS_client.connected()) {
// 	// 		Serial.println();
// 	// 		Serial.println("disconnecting from server.");
// 	// 		TLS_client.stop();

// 	// 		// do nothing forevermore:
// 	// 		// while (true);
// 	// 		return "";
// 	// 	}
// 	// }
// }