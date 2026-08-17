#include <stdint.h>
#include "global.h"

Preferences preferences;

String saved_SSID = "";
String saved_Password = "";

bool web_app_timeout = false;

wifi_conn_state_t wifi_state = WIFI_STATE_IDLE;
uint32_t wifi_connect_start_ms = 0;
lv_timer_t* wifi_timer = NULL;

const char* CONFIG_PATH = "/web_app_config.json";

float ph_sensor = -1;
float visc_sensor = -1;
uint8_t mlp_risk_label = 3;

JsonDocument global_variable;

screening_state_t screening_state = SCREENING_IDLE;
lv_timer_t* screening_timer = NULL;
uint32_t screening_poll_start_ms = 0;
String screening_schedule_key = "";
String screening_student_key = "";
String screening_key_active = "";