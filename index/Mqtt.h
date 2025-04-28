#pragma once

#include <WiFi.h>
#include <PubSubClient.h>

class MQTT {
public:
    MQTT();                              // Constructor
    ~MQTT();                             // Destructor

    bool setup(const char* clientId, const char* server, int port,
               const char* username, const char* password);

    bool connect();                      // Connect to MQTT broker
    bool loop();                         // Keep connection alive

    bool publish(const char* topic, const char* payload, bool retained = false);
    bool subscribe(const char* topic);
    
    void setCallback(MQTT_CALLBACK_SIGNATURE);
    void setBufferSize(int size);

private:
    WiFiClient wifiClient;
    PubSubClient client;
    
    const char* mqttClientId;
    const char* mqttUsername;
    const char* mqttPassword;
};
