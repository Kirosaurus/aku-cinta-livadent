#pragma once
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

extern WebSocketsServer webSocket;

extern void updateSessionKey();
extern void webSocketEvent(uint8_t clientNum, WStype_t type, uint8_t *payload, size_t length);