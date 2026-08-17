#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <UI_LCD.h>
// #include <esp_wifi.h>
#include <LittleFS.h>

#include "global.h"
#include "event_handler.h"
#include "firebase_livadent.h"
#include "lcd.h"
#include "websockets.h"
// #include "esp_event.h"
// #include "esp_netif.h"
// #include "nvs_flash.h"

#define MAX_WIFI_RESULTS 15
#define WIFI_ITEMS_PER_PAGE 3
#define WIFI_CONNECT_TIMEOUT 10000 // 10 Detik Timeout (100 * 100 = 10000 milidetik / 10 detik) --> Rumus : 100 * n = t detik | Keterangan : n = nilai WIFI_CONNECT_TIMEOUT, t = waktu timeout yang diharapkan

// typedef enum {
//   WIFI_STATE_IDLE,
//   WIFI_STATE_CONNECTING,
//   WIFI_STATE_SUCCESS,
//   WIFI_STATE_FAILED
// } wifi_conn_state_t;

extern char wifi_ssid_list[MAX_WIFI_RESULTS][33];
extern int wifi_ssid_count;
extern int wifi_current_page;

extern void render_wifi_page();

extern void WiFi_scan();
extern void WiFi_connect(lv_event_t * e);
extern void WiFi_connect_tick(lv_timer_t * timer);
extern String get_mac();
