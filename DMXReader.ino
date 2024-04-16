#include <WiFi.h>
#include <WebServer.h>
#include <Arduino.h>
#include <esp_dmx.h>
#include "HtmlContent.h"
#include <ESPmDNS.h>

// Configuration for Access Point
const char* ssid = "DMXReader";
const char* password = "12345678";

// Web server runs on port 80
WebServer server(80);

int transmitPin = 17;
int receivePin = 16;
int enablePin = 21;
dmx_port_t dmxPort = 1;
byte data[DMX_PACKET_SIZE];
bool dmxIsConnected = false;

void setup() {
  Serial.begin(115200);
  
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  dmx_config_t config = DMX_CONFIG_DEFAULT;
  dmx_personality_t personalities[] = {{1, "Default Personality"}};
  int personality_count = 1;
  dmx_driver_install(dmxPort, &config, personalities, personality_count);
  dmx_set_pin(dmxPort, transmitPin, receivePin, enablePin);

  server.on("/", HTTP_GET, []() {
    server.send_P(200, "text/html", INDEX_HTML);
  });

  server.on("/data", HTTP_GET, handleDataRequest);

  server.begin();
  Serial.println("HTTP server started");
  MDNS.begin("DMXReader");
  MDNS.addService("http", "tcp", 80);
}

void loop() {
  server.handleClient();
  checkDMXConnection();
}

void checkDMXConnection() {
  dmx_packet_t packet;
  if (dmx_receive(dmxPort, &packet, DMX_TIMEOUT_TICK)) {
    if (!packet.err) {
      dmxIsConnected = true;
      dmx_read(dmxPort, data, packet.size);
    }
  } else if (dmxIsConnected) {
    Serial.println("DMX was disconnected.");
    ESP.restart();
  }
}

void handleDataRequest() {
  String json = "{";
  for (int channel = 1; channel <= 512; channel++) {
    json += "\"" + String(channel) + "\": " + String(data[channel]);
    if (channel < 512) json += ", ";
  }
  json += "}";
  server.send(200, "application/json", json);
}