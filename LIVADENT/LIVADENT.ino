#include "firebase_livadent.h"
#include "lcd.h"  // Urutannya harus gini, kalo misal lcd.h duluan nanti malah error FS.h, karena di dalem TFT_eSPI ada statement FS_NO_GLOBAL
#include "wifi_livadent.h"
#include "global.h"

#include <ESP.h>

void setup() {
  Serial.begin(115200);

  preferences.begin("wifi", false);
  
  lcd_setup();

  
  // wifi_setup();
  // wifi_scan();
  // Display total and free heap memory
  Serial.print("Total Heap: ");
  Serial.println(ESP.getHeapSize());
  Serial.print("Free Heap: ");
  Serial.println(ESP.getFreeHeap());

  // Display PSRAM details (if available)
  Serial.print("Total PSRAM: ");
  Serial.println(ESP.getPsramSize());
  Serial.print("Free PSRAM: ");
  Serial.println(ESP.getFreePsram());
}

bool anu = 0;
void loop() {
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