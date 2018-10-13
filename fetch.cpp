#include <Arduino.h>
#include "fetch.h"
#include "controller.h"
#include "config.h"

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
TLSClient client;

String token = "";

String fetch(int method, String url, String content, bool auth, String server, int retry) {
	if (auth && token == "") login(CAR_USERNAME, CAR_PASSWORD);
	Serial.println("\nStarting connection to server...");
	if (client.connect(server.c_str(), 443)) {
		Serial.println("connected to server");
		// Make a HTTP request over SSL (HTTPS)
		if (method == GET) client.print("GET ");
		else if (method == POST) client.print("POST ");
		client.print(url);
		client.println(" HTTP/1.1");
		client.print("Host: ");
		client.println(server);
		client.println("Accept: */*");
		if (auth) client.println("Authorization: JWT " + token);
		client.println("Connection: close");
		if (method == POST) client.print("Content-Length: ");
		if (method == POST) client.println(content.length());
		if (method == POST) client.println("Content-Type: application/json");
		client.println();
		client.println(content);
	}

	int is_head = 1, is_end = 0, length = 0;
	String STATUS = "", data = "";
	String currentLine = "";
	while (client.available()) {
		char c = client.read();
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
			client.stop();
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
			// that's the end of the client HTTP request, so send a response:
			if (currentLine.length() == 0) {
				is_head = 0;
			} else { // if you got a newline, then clear currentLine:
				currentLine = "";
			}
		} else if (c != '\r') {  // if you got anything else but a carriage return character,
			currentLine += c;      // add it to the end of the currentLine
		}


		// if the server's disconnected, stop the client:
		if (!client.connected()) {
			Serial.println();
			Serial.println("disconnecting from server.");
			client.stop();

			// do nothing forevermore:
			// while (true);
			return "";
		}
	}
}

void login(String username, String password) {
	token = fetch(POST, "/api/v1/login", "{\"username\":\"" + username + "\",\"password\":\"" + password + "\"}", false);
}