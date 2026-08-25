#include "local_server.h"
#include "firebase_livadent.h"
#include "wifi_livadent.h"
#include "lcd.h"  // Urutannya harus gini, kalo misal lcd.h duluan nanti malah error FS.h, karena di dalem TFT_eSPI ada statement FS_NO_GLOBAL
#include "mlp.h"
#include "global.h"
#include "hardware_workflow.h"
#include "esp_heap_caps.h"
// #include "websockets.h"

// #include <ESP.h>

void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  // HARDWARE
  pinMode(EN_PIN_X, OUTPUT);
  pinMode(PIN_SENSOR_PH, INPUT);
  //

  xQueue = xQueueCreate(4, sizeof(motor));
  xTaskCreatePinnedToCore(motor_task, "MotorTask", 4096, NULL, 1, &TaskMotor, 0);

  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS gagal dimount");
  }

  ads.setGain(GAIN_TWOTHIRDS);

  preferences.begin("wifi", false);

  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  lcd_setup();
  mlp_init();
  init_json();  // No Mac Address dan No IP kalo misal egak ada auto connect SSID (only "on_progress" attribute)
  WiFi.setSleep(false);

#ifdef DEBUG
  Serial.print("Total Heap: ");
  Serial.println(ESP.getHeapSize());
  Serial.print("Free Heap: ");
  Serial.println(ESP.getFreeHeap());
  Serial.printf("Free DRAM: %u\n", heap_caps_get_free_size(MALLOC_CAP_8BIT));
#endif
}

bool anu = 0;
void loop() {

  // webSocket.loop();
  // #ifdef DEBUG
  //   Serial.print("Total Heap: ");
  //   Serial.println(ESP.getHeapSize());
  //   Serial.print("Free Heap: ");
  //   Serial.println(ESP.getFreeHeap());
  //   Serial.printf("Free DRAM: %u\n", heap_caps_get_free_size(MALLOC_CAP_8BIT));
  // #endif
  update_json();
  if (WiFi.status() == WL_CONNECTED) {
    firebaseApp.loop();
    // updateSessionKey();
  }

  if (firebaseApp.ready() && !anu) {
    set_device_active();
    // predict_test();
    anu = 1;
  }

  update_UI();

  // webSocket.loop();
  delay(5);
}