#include "log.h"
#include "SdManager.h"
#include "network_integration.h"
SdManager sd_manager;
void setup(){
  Serial.begin(115200);
  delay(1000);
  Logln("🚀 Starting program...\n");

  sd_manager.begin();
  std::string configString;
  sd_manager.loadConfiguration("/config.txt",config,configString);
  Logln(configString.c_str());
  printConfig(config);
  Logf("(%s - %s)\n",config.wifi_ssid,config.wifi_password);
  setupWifi("Wifi",config.wifi_ssid,config.wifi_password);
  NTP::setupTime();
}

void loop(){

  if(Serial.available()){
    char incomingByte = Serial.read();
    if (incomingByte =='R' || incomingByte == 'r'){
      Logln("\nReiniciando 🔄\n");
      delay(1000);
      esp_restart();
    }
  }
  

  static int counter = 0;
  Logf("Looping (%i) hora: %s\n",counter++,NTP::getFormattedTime().c_str());
  delay(4000);
}


void printConfig(const Config& config) {
  Logln(F("   -------- Configuração Atual --------"));
  Log(F("\t|UID:             "));
  Logln(config.station_uid);
  Log(F("\t|Nome da Estação: "));
  Logln(config.station_name);
  Log(F("\t|WiFi SSID:       "));
  Logln(config.wifi_ssid);
  Log(F("\t|WiFi Password:   "));
  Logln(config.wifi_password);
  Log(F("\t|MQTT Server:     "));
  Logln(config.mqtt_server);
  Log(F("\t|MQTT Username:   "));
  Logln(config.mqtt_username);
  Log(F("\t|MQTT Password:   "));
  Logln(config.mqtt_password);
  Log(F("\t|MQTT Topic:      "));
  Logln(config.mqtt_topic);
  Log(F("\t|MQTT Port:       "));
  Logln(config.mqtt_port);
  Log(F("\t|Intervalo (ms):  "));
  Logln(config.interval);
  Logln(F("    ------------------------------------\n"));
}