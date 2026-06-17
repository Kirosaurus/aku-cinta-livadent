#include "lcd.h"
#include "wifi_livadent.h"

void setup() {
  Serial.begin(115200);
  lcd_setup();
  WiFi_scan();
  // wifi_setup();
  // wifi_scan();
}

// bool anu = 0;
void loop() {
  update_UI();
  // if(!anu){
  //    wifi_setup();
  //    wifi_scan();  
  //    anu = 1;
  // }
  delay(5);
}