#pragma once

#include <Arduino.h>
#include <LittleFS.h>
#include <lvgl.h>
#include <UI_LCD.h>
#include <math.h>
#include <Adafruit_ADS1X15.h>
#include <AccelStepper.h>
#include <ArduinoJson.h>

#include "mlp.h"
// #include "esp_random.h"
#include "global.h"

#define SCAN_TIMEOUT 30000
#define PIN_SENSOR_PH 34
#define FAILED_TIMEOUT 10000
#define PH_INPUT_DUMMY 6.2
#define VISKOSITAS_INPUT_DUMMY 2.32

using namespace std;

extern Adafruit_ADS1115 ads;

extern void process(String schedule_key, String student_key, String screening_key);