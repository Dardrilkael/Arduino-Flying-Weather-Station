#pragma once

// ==============================
// 📦 Estrutura de Configuração
// ==============================
struct Config {
    char station_uid[32]{};
    char station_name[64]{};
    char wifi_ssid[64]{};
    char wifi_password[64]{};
    char mqtt_server[64]{};
    char mqtt_username[64]{};
    char mqtt_password[64]{};
    char mqtt_topic[64]{};

    int mqtt_port = 1883;
    int interval = 15000;
};

// Instância global de configuração
inline Config config;

void printConfig(const Config& config);
