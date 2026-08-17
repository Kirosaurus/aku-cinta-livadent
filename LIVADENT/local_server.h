#pragma once

#include <lvgl.h>
#include <ArduinoJson.h>
#include <esp_random.h>
// #include <UI_LCD.h>

#include "global.h"

#define TIMEOUT_WEB_APP_CONNECTION 15000 // 15 Detik

extern void init_json();