#pragma once
#include <Preferences.h>
#include <lvgl.h>

extern Preferences preferences;

extern String saved_SSID;
extern String saved_Password;

typedef enum {
  WIFI_STATE_IDLE,
  WIFI_STATE_CONNECTING,
  WIFI_STATE_SUCCESS,
  WIFI_STATE_FAILED
} wifi_conn_state_t;

extern wifi_conn_state_t wifi_state;
extern uint32_t wifi_connect_start_ms;
extern lv_timer_t* wifi_timer;