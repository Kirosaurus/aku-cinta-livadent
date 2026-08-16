#include "firebase_livadent.h"
#include "wifi_livadent.h"
#include "lcd.h"  // Urutannya harus gini, kalo misal lcd.h duluan nanti malah error FS.h, karena di dalem TFT_eSPI ada statement FS_NO_GLOBAL
#include "mlp.h"
#include "global.h"
#include "hardware_workflow.h"
#include "esp_heap_caps.h"
#include "websockets.h"

// #include <ESP.h>

void setup() {
  Serial.begin(115200);
  while (!Serial) {
  }

  pinMode(PIN_SENSOR_PH, INPUT);

  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS gagal dimount");
  }

  ads.setGain(GAIN_TWOTHIRDS);

  preferences.begin("wifi", false);

  lcd_setup();
  mlp_init();
  init_json();  // No Mac Address dan No IP kalo misal egak ada auto connect SSID (only "on_progress" attribute)
  WiFi.setSleep(false);

  // wifi_setup();
  // wifi_scan();

  // Serial.print("Total Heap: ");
  // Serial.println(ESP.getHeapSize());
  // Serial.print("Free Heap: ");
  // Serial.println(ESP.getFreeHeap());
  Serial.printf("Free DRAM: %u\n", heap_caps_get_free_size(MALLOC_CAP_8BIT));

  // Serial.print("Total PSRAM: ");
  // Serial.println(ESP.getPsramSize());
  // Serial.print("Free PSRAM: ");
  // Serial.println(ESP.getFreePsram());
  // Serial.println("BEFORE websocket.begin()");
  // webSocket.begin();
  // Serial.println("AFTER websocket.begin()");
  // Serial.println("BEFORE websocket.onEvent()");
  // webSocket.onEvent(webSocketEvent);
  // Serial.println("AFTER websocket.onEvent()");
}

bool anu = 0;
void loop() {

  // Serial.println("Still Online");

  if (WiFi.status() == WL_CONNECTED) {
    firebaseApp.loop();
    server.handleClient();
    updateSessionKey();
    webSocket.loop();
    update_json();
  }

  if (firebaseApp.ready() && !anu) {
    set_device_active();
    // predict_test();
    anu = 1;
  }

  update_UI();

  // if (firebaseApp.ready() && !anu)
  // {
  //   set_device_active();
  //   anu = true;

  //   database.get(async_client, "/account_data/sdn_1_livadent/kelas_1/A/Puji%20Astuti", processData, false, "Get Puji Astuti");

  //   // if (async_client.lastError().code() == 0)
  //   // {
  //   //   tft.println("Value get complete.");
  //   // }
  //   // else
  //   //   Firebase.printf("Error, msg: %s, code: %d\n", async_client.lastError().message().c_str(), async_client.lastError().code());
  //   Serial.println("[APP] Free memory: " + String(esp_get_free_heap_size()) + " bytes");
  // }

  delay(5);
}