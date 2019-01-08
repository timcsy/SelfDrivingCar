#include <Arduino.h>
#include "server.h"

#define WIFI_CLIENT_MAX 10

WiFiServer server(5000);
WiFiClient * clients[WIFI_CLIENT_MAX] = {NULL};
int wifi_counter = 0;

void server_setup() {
  server.begin();
  Serial.println("Server is started.");
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

void server_loop() {
  if (wifi_counter < WIFI_CLIENT_MAX) {
    WiFiClient newClient = server.available();
    if (newClient) {
      Serial.println("new client");
      clients[wifi_counter++] = new WiFiClient(newClient);
    }
  }
  for (int i = 0; i < wifi_counter; i++) {
    if (clients[i]->connected()) {
      String req = "";
      while (clients[i]->available()) {
        char c = clients[i]->read();
        Serial.print(c);
        if (c == '\n') {
          if (req.startsWith("join ")) {
            req.replace("join ", "");
            Serial.println(req);
          } else if (req.startsWith("send-to ")) {
            req.remove(0, 8);
          }
        } else req += c;
        delay(1);
      }
    }
  }
}

