#include "lcd.h"
#include "wifi.h"

void setup() {
  Serial.begin(115200);
  lcd_setup();
  // wifi_setup();
  // wifi_scan();
}

// bool anu = 0;
void loop() {

  // if(!anu){
  //    wifi_setup();
  //    wifi_scan();
  //    anu = 1;
  // }

  lv_timer_handler();  // let the GUI do its work
  lv_tick_inc(5);     // tell LVGL how much time has passed
  delay(5);
}