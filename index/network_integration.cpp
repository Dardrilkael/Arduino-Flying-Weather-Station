// Título: Integração HTTP & MQTT
// Data: 30/07/2023
#pragma once

#include "log.h"
#include <WiFi.h>              // Para conexão WiFi
#include <time.h>
#include "esp_sntp.h"
// ----------- CONFIGURAÇÕES -----------
const char* ntpServer = "br.pool.ntp.org"; // Servidor NTP


// ----------- FUNÇÕES -----------

/**
 * Inicializa a conexão WiFi em modo estação (STA).
 * 
 * @param contextName Nome do contexto para debug
 * @param ssid SSID da rede WiFi
 * @param password Senha da rede WiFi
 * @return int 0 se conectado com sucesso
 */
int setupWifi(const char* contextName, const char* ssid, const char* password)
{
  LogContext(contextName,"Estabelecendo conexão WiFi...");

  WiFi.mode(WIFI_STA);               // Modo estação (cliente)
  WiFi.begin(ssid, password);        // Conecta à rede
  WiFi.setAutoReconnect(true);       // Reconnect automático
  WiFi.persistent(true);             // Salva rede nos registros

  // secureWifiClient.setCACert(root_ca); // Descomente se for usar HTTPS com certificado

  // Aguarda conexão
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 40) {
    delay(500);
    Logf(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
   Logf("\n%s: Conectado com sucesso. IP: %s\n", contextName, WiFi.localIP().toString().c_str());
    return 0;
  } else {
    Logf("\n%s: Falha na conexão WiFi.\n", contextName);
    return -1;
  }
}

namespace NTP{
/**
 * Inicializa o cliente NTP para sincronização de horário.
 */

// 📌 1. Inicializa o NTP
void setupTime() {
  configTime(0, 0, ntpServer); // offset UTC e DST (pode ajustar depois)
  sntp_set_sync_interval(600000); // sincroniza a cada 10 minutos
  Log("⏱️  Sincronização de tempo NTP iniciada.\n");
}

// 📌 2. Retorna o horário formatado como string "dd/mm/yyyy HH:MM:SS"
String getFormattedTime() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    char buffer[32];
    strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", &timeinfo);
    return String(buffer);
  }
  return "Invalid";
}

// 📌 3. Retorna o timestamp atual (epoch time)
time_t getTimestamp() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    return mktime(&timeinfo);
  }
  return 0;
}
}
