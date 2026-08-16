#pragma once

#include <lvgl.h>
#include <LittleFS.h> 
#include <WebServer.h>
#include <ArduinoJson.h>
#include <esp_random.h>
// #include <UI_LCD.h>

#include "global.h"

#define TIMEOUT_WEB_APP_CONNECTION 15000 // 15 Detik
 
extern WebServer server;

extern void init_json();
extern void get_json();
extern void send_cors_headers();
extern void local_server_init();
extern void update_json();
extern void screening_timer(lv_timer_t * timer);