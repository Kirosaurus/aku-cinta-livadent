#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <UI_LCD.h>
#include <esp_wifi.h>

#include "global.h"
#include "event_handler.h"
// #include "esp_event.h"
// #include "esp_netif.h"
// #include "nvs_flash.h"

#define MAX_AP_RECORDS 10
#define WIFI_CONNECT_TIMEOUT 10000 // 10 Detik Timeout (100 * 100 = 10000 milidetik / 10 detik) --> Rumus : 100 * n = t detik | Keterangan : n = nilai WIFI_CONNECT_TIMEOUT, t = waktu timeout yang diharapkan

typedef enum {
  WIFI_STATE_IDLE,
  WIFI_STATE_CONNECTING,
  WIFI_STATE_SUCCESS,
  WIFI_STATE_FAILED
} wifi_conn_state_t;

extern void WiFi_scan();
extern void WiFi_connect(lv_event_t * e);
extern void WiFi_connect_tick(lv_timer_t * timer);
extern String get_mac();

// FUNCTION INI BUAT SETUP WIFI PAKE LIBRARY "esp_wifi.h" (Cara Kuno Low Level Setup WiFi ESP32)
// ================================================================================================================================================================
// extern void wifi_setup();
// extern void wifi_scan();
// extern bool ssid_is_unique(wifi_ap_record_t uniqueRecords[], uint16_t uniqueCount, char* ssid);
// extern void print_ssid(wifi_ap_record_t apRecords[], uint16_t apNum);
// ================================================================================================================================================================