#include "SdManager.h"
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include "constants.h"
#include "log.h"
#include <ArduinoJson.h>
#include <sstream>

bool SdManager::begin() {
    SPI.begin(SD_CLK_PIN, SD_MISO_PIN, SD_MOSI_PIN);
    int attempts = 0;

    while (!SD.begin(SD_CS_PIN, SPI)) {
        Logf("[SD] Cartão não encontrado. Tentando novamente em %d segundos...\n", RETRY_INTERVAL / 1000);
        delay(RETRY_INTERVAL);
        if (++attempts > 5) {
            Log("[SD] Falha após várias tentativas.\n");
            return false;
        }
    }

    Log("[SD] Cartão iniciado com sucesso.\n");
    return true;
}

bool SdManager::createDirectory(const char* directory) {
    Logf("[SD] Verificando/criando diretório: %s\n", directory);
    if (!SD.exists(directory)) {
        if (SD.mkdir(directory)) {
            Log("[SD] Diretório criado com sucesso.\n");
            return true;
        } else {
            Log("[SD] Falha ao criar diretório.\n");
            return false;
        }
    }
    Log("[SD] Diretório já existe.\n");
    return true;
}

bool SdManager::appendToFile(const char* path, const char* message) {
    File file = SD.open(path, FILE_APPEND);
    if (!file) {
        Log("[SD] Falha ao abrir arquivo para escrita.\n");
        return false;
    }

    if (file.print(message)) {
        Log("[SD] Dados gravados com sucesso.\n");
    } else {
        Log("[SD] Falha ao gravar dados.\n");
        file.close();
        return false;
    }

    file.close();
    return true;
}

bool SdManager::storeMeasurement(const char* directory, const char* fileName, const char* payload, const char* ext) {
    if (!createDirectory(directory)) return false;

    String fullPath = String(directory) + "/" + fileName + "." + ext;

    // Check if file exists before writing
    if (!SD.exists(fullPath)) {
        // File doesn't exist, write header first
        const char* header = "timestamp,datetime,temperature,humidity,pressure,co2,ch4,co";; // Customize this based on your data format
        if (!appendToFile(fullPath.c_str(), header)) {
            Log("⚠️ Falha ao escrever o cabeçalho do arquivo.\n");
            return false;
        }
    }

    return appendToFile(fullPath.c_str(), (String(payload)+"\n").c_str());
}

bool SdManager::loadConfiguration(const char* path, Config &config, std::string& configJson) {
  File file = SD.open(path);
  if (!file) {
    Log("❌ Erro ao abrir arquivo de configuração.");
    return false;
  }

  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    Logf("❌ Erro ao fazer parsing do JSON: %s",error.c_str());
    return false;
  }

  strlcpy(config.station_uid,   doc["UID"] | "", sizeof(config.station_uid));
  strlcpy(config.station_name,  doc["SLUG"] | "", sizeof(config.station_name));

  // Wi-Fi
  const char* wifi_raw = doc["WIFI"] | "";
  sscanf(wifi_raw, "%63[^:]:%63s", config.wifi_ssid, config.wifi_password);

  // MQTT
  const char* mqtt_raw = doc["MQTT_HOST"] | "";
  sscanf(mqtt_raw, "mqtt://%63[^:]:%63[^@]@%63[^:]:%d", config.mqtt_username, config.mqtt_password, config.mqtt_server, &config.mqtt_port);


  strlcpy(config.mqtt_topic, doc["MQTT_TOPIC"] | "", sizeof(config.mqtt_topic));
  config.interval = doc["INTERVAL"] | 15000;
  serializeJson(doc, configJson);
  return true;
}

