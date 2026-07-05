#include <Arduino.h>
#include <LittleFS.h>
#include <lvgl.h>
#include <UI_LCD.h>
#include <vector>

#include "mlp.h"
#include "esp_random.h"
#include "global.h"

using namespace std;

extern void process();