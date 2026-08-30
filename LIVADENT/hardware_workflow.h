#pragma once

#include <Arduino.h>
#include <LittleFS.h>
#include <lvgl.h>
#include <UI_LCD.h>
#include <math.h>
#include <Adafruit_ADS1X15.h>
#include <AccelStepper.h>
#include <ArduinoJson.h>
#include <TMCStepper.h>

#include "mlp.h"
#include "esp_random.h"
#include "global.h"

#define SCAN_TIMEOUT 30000
#define PIN_SENSOR_PH 34
#define FAILED_TIMEOUT 10000
#define PH_INPUT_DUMMY 6.49
#define VISKOSITAS_INPUT_DUMMY 1.43

#define DUMMY_TIMEOUT_SCAN 10000 // INI CUMA DUMMY LEBIH CEPET DARI PADA YANG SEHARUSNYA

// Pin Motor Penggerak Sensor
#define DIR_PIN_X 13
#define STEP_PIN_X 14
#define EN_MOVE_PIN 17
#define STEP_PIN_Y 32
#define DIR_PIN_Y 33

// Pin Viskositas
#define EN_VISC_PIN 16
#define DIR_VISC_PIN 26
#define STEP_VISC_PIN 27

// Konfigurasi Viskositas
#define R_SENSE 0.11f
#define SERIAL_PORT Serial0
#define RMS_CURRENT 367
#define DRIVER_ADDRESS 0b00

// Pin Emergency Button
#define EMERGENCY_BTN_PIN 12

using namespace std;

extern Adafruit_ADS1115 ads;
extern QueueHandle_t xQueue;
extern TaskHandle_t TaskMotor;
extern void motor_task(void* pv);
extern void create_queue_motor(AccelStepper* motor_identifier, long distance, screening_state_t next_state);

// extern void process(String schedule_key, String student_key, String screening_key);
extern void screening_process_tick(lv_timer_t* timer);
extern void start_screening_process(String schedule_key, String student_key, String screening_key);
extern void stop_screening_process();
extern void check_emergency_btn();
extern bool init_ph();
extern bool init_visc();
extern TMC2209Stepper driver;
extern AccelStepper stepperVisc;