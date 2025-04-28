#include "log.h"
#include "SdManager.h"
#include "network_integration.h"
#include "GPSSensor.h"
#include "Mqtt.h"
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_AHTX0.h>
#include "constants.h"
#include "GasSensor.h"

//MQGasSensor(uint8_t analogPin, float rl, float ro, float a, float b)
MQ4GasSensor mq4(MQ4_PIN,10, 20.83, 116.602, -0.893); 
MQ7 mq7(MQ7_PIN,5.0);
SdManager sd_manager;
GPSSensor gps(Serial2, 16, 17);
MQTT mqtt;
Adafruit_BMP085 bmp;
Adafruit_AHTX0 aht;


struct{
  float temperature = 0.0f;  // Temperature in Celsius
  float humidity = 0.0f;     // Humidity in percentage
  float pressure = 0.0f;     // Pressure in hPa
  float co2 = 0.0f;          // CO2 concentration in ppm
  float ch4 = 0.0f;          // CH4 concentration in ppm
  float co = 0.0f;           // CO concentration in ppm
} data;

float analogToVoltage(int analogValue, float vref = 3.3, int resolution = 4095) {
  return (analogValue / (float)resolution) * vref*2.0;
}
void readVoltages(float& mg811Volt, float& mq4Volt,float& mq7Volt ) {
  int mg811Raw = analogRead(MG811_PIN);
  int mq4Raw   = analogRead(MQ4_PIN);
  int mq7Raw   = analogRead(MQ7_PIN);

  mg811Volt = analogToVoltage(mg811Raw);
  mq4Volt  = analogToVoltage(mq4Raw);
  mq7Volt  = analogToVoltage(mq7Raw);

  Serial.printf("MG811: %.2f V, MQ-4: %.2f V, MQ-7: %.2f V\n", mg811Volt, mq4Volt, mq7Volt);
}

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
  gps.begin();
  mqtt.setup(config.station_name,config.mqtt_server,config.mqtt_port,config.mqtt_username,config.mqtt_password);
  mqtt.connect();


   Wire.begin(); // Default SDA=21, SCL=22 on ESP32

  if (!bmp.begin()) {
    Serial.println("Could not find BMP180 sensor!");
    while (1);
  }

  if (!aht.begin()) {
    Serial.println("Could not find AHT10/AHT110 sensor!");
    while (1);
  }
}
String timeString = "00/00/2025 00:00:00";
int timestamp = 0;
char dataBuffer[512]{0};
void loop(){

  if(Serial.available()){
    char incomingByte = Serial.read();
    if (incomingByte =='R' || incomingByte == 'r'){
      Logln("\nReiniciando 🔄\n");
      delay(1000);
      esp_restart();
    }
  }
 

  gps.update();
  if (gps.isLocationValid()) {
      Log("Lat: "); Logln(gps.getLatitude());
      Log("Lng: "); Logln(gps.getLongitude());
  }


  String GPStime = gps.getDateTime();
  String NTPtime = NTP::getFormattedTime();

  if (GPStime != "Invalid"){
    timeString = GPStime;
    timestamp=gps.getUnixTime();
  }
  else{
    if (NTPtime !="Invalid"){
      timeString = NTPtime;
      timestamp = NTP::getTimestamp();
    }
    else{
        Logln("Error: Both GPS and NTP times are invalid.");
        // Use fallback time or handle error
    }
  }

  //Logf("\nNTP TIME: %s\n",NTPtime.c_str());
  //Logf("GPS TIME: %s\n",GPStime.c_str());
  //Logf("TIM TIME: %s\n",timeString.c_str());


  data.pressure = bmp.readPressure() / 100.0f;  // Convert to hPa
  float bmpTemp = bmp.readTemperature();  // BMP temperature
  
  // Read from AHT110
  sensors_event_t humidityEvent, tempEvent;
  aht.getEvent(&humidityEvent, &tempEvent);
  data.humidity = humidityEvent.relative_humidity;
  data.temperature = tempEvent.temperature; // Use this if you trust AHT's temp more
  readVoltages(data.co2, data.ch4, data.co);
  Logf("reading mq7: %i\n",mq4.readRs());
  setupWifi("Wifi",config.wifi_ssid,config.wifi_password);
   if(mqtt.connect())
    mqtt.loop();
  int bytesWritten = snprintf(dataBuffer, 512,
            "%i,%s,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f",
            timestamp,
            timeString.c_str(),  // Pre-formatted time
            data.temperature, data.humidity, data.pressure, data.co2, data.ch4, data.co);
  if (bytesWritten < 0) {
    Logln("Error: snprintf failed.");
} else {
    Logln(dataBuffer);
    
    mqtt.publish(config.mqtt_topic, dataBuffer);
    sd_manager.storeMeasurement("/medidas","time",dataBuffer);
    
    //const char* csvHeader = "timestamp,datetime,temperature,humidity,pressure,co2,ch4,co";
    

}
  delay(10000);
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