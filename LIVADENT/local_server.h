#pragma once

#include <LittleFS.h> 
#include <WebServer.h>
#include <ArduinoJson.h>

#define TIMEOUT_WEB_APP_CONNECTION 10000 // 10 Detik
 
extern WebServer server;
 
extern void get_json();
extern void send_cors_headers();
extern void local_server_init();
extern void update_json();