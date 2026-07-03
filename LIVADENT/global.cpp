#include "global.h"

Preferences preferences;

String saved_SSID = "";
String saved_Password = "";

bool web_app_timeout = false;

wifi_conn_state_t wifi_state = WIFI_STATE_IDLE;
uint32_t wifi_connect_start_ms = 0;
lv_timer_t* wifi_timer = NULL;

const char* CONFIG_PATH = "/web_app_config.json";