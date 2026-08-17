#include <stdio.h>
#include "esp32-hal.h"
#include "Arduino.h"
#include "local_server.h"
#include "wifi_livadent.h"

uint32_t last_milli = millis();

void init_json() {
  global_variable["mac"] = WiFi.macAddress();
  global_variable["local_ip"] = WiFi.localIP().toString();
  global_variable["is_ph"] = true;    // Ini nanti diganti sesuai dengan kondisi sensor
  global_variable["is_visc"] = true;  // Ini juga
  global_variable["on_going"] = false;
  // doc["web_app"] = false;
  global_variable["ready"] = false;
  global_variable["stop"] = false;
}