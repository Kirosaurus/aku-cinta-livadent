#pragma once

#include <Arduino.h>
#include <LittleFS.h>
#include <lvgl.h>
#include <UI_LCD.h>
// #include <vector>
#include <Adafruit_ADS1X15.h>

#include "mlp.h"
#include "esp_random.h"
#include "global.h"

#define SECOND_OFFSET 30000
#define PIN_SENSOR_PH 34

using namespace std;

extern Adafruit_ADS1115 ads;

extern void process();