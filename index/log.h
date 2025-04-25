#pragma once

// ===============================
// 🛠️  Modo de depuração
// ===============================
#define DEBUG_MODE  // Comente essa linha para desativar os logs

#ifdef DEBUG_MODE
  #define OnDebug(x) x
#else
  #define OnDebug(x)
#endif

// ===============================
// 🧾 Logs com contexto e formato
// ===============================

/**
 * @brief Log simples (sem contexto).
 * Aceita qualquer expressão compatível com Serial.println().
 */
#define Log(msg) OnDebug(Serial.print(msg))
#define Logln(msg) OnDebug(Serial.println(msg))
/**
 * @brief Log com contexto nomeado (ex: função, módulo).
 * Usa printf-style com dois argumentos: contexto e mensagem.
 */
#define LogContext(ctx, msg) OnDebug(Serial.printf("[%s] %s\n", ctx, msg))

/**
 * @brief Log com printf formatado (ex: Logf("Temp: %.2f C", temp))
 */
#define Logf(...) OnDebug(Serial.printf(__VA_ARGS__))