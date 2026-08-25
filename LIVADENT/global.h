#pragma once
#include <Preferences.h>
#include <lvgl.h>
#include <ArduinoJson.h>
#include <AccelStepper.h>

extern Preferences preferences;

extern String saved_SSID;
extern String saved_Password;

extern bool web_app_timeout;

typedef enum {
  WIFI_STATE_IDLE,
  WIFI_STATE_CONNECTING,
  WIFI_STATE_SUCCESS,
  WIFI_STATE_FAILED
} wifi_conn_state_t;

typedef enum {
  SCREENING_IDLE,
  SCREENING_SENSOR_MOVE_X_PH,
  SCREENING_SENSOR_MOVE_Y_PH,
  SCREENING_SENSOR_READ_PH,
  SCREENING_SENSOR_MOVE_Y_RETURN_PH,
  SCREENING_SENSOR_MOVE_X_VISC,
  SCREENING_SENSOR_MOVE_Y_VISC,
  SCREENING_SENSOR_MOVE_Y_RETURN_VISC,
  SCREENING_SENSOR_READ_VISC,
  SCREENING_SEND_TO_FIREBASE,
  SCREENING_WAIT_CLASSIFICATION,
  SCREENING_SENSOR_MOVE_X_FINISH,
  SCREENING_SENSOR_MOVE_Y_FINISH,
  SCREENING_DONE,
  SCREENING_STOPPING 
} screening_state_t;

struct motor {
  AccelStepper* motor;
  long distance;
  screening_state_t next_state;
};

extern screening_state_t screening_state;
extern lv_timer_t* screening_timer;
extern uint32_t screening_poll_start_ms;
extern String screening_schedule_key;
extern String screening_student_key;
extern String screening_key_active;

extern wifi_conn_state_t wifi_state;
extern uint32_t wifi_connect_start_ms;
extern lv_timer_t* wifi_timer;

extern const char* CONFIG_PATH;

extern float ph_sensor;
extern float visc_sensor;
extern uint8_t mlp_risk_label;

extern JsonDocument global_variable;

// MACRO CONFIGURATION
#define DEBUG