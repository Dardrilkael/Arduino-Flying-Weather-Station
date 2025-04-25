// Título: Integração HTTP & MQTT
// Data: 30/07/2023
#pragma once




// ----------- FUNÇÕES -----------

/**
 * Inicializa a conexão WiFi em modo estação (STA).
 * 
 * @param contextName Nome do contexto para debug
 * @param ssid SSID da rede WiFi
 * @param password Senha da rede WiFi
 * @return int 0 se conectado com sucesso
 */
int setupWifi(const char* contextName, const char* ssid, const char* password);

namespace NTP{
/**
 * Inicializa o cliente NTP para sincronização de horário.
 */
void setupTime();

// 📌 2. Retorna o horário formatado como string "dd/mm/yyyy HH:MM:SS"
String getFormattedTime();

// 📌 3. Retorna o timestamp atual (epoch time)
time_t getTimestamp();
}