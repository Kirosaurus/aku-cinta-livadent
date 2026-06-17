// #include <Arduino.h>
#include <UI_LCD.h>
#include <WiFi.h>
// #include <esp_wifi.h>
#include "lcd.h"
#include "wifi.h"
// #include "esp_event.h"
// #include "esp_netif.h"
// #include "nvs_flash.h"


// FUNCTION INI BUAT SETUP WIFI PAKE LIBRARY "esp_wifi.h" (Cara Kuno Low Level Setup WiFi ESP32)
// ================================================================================================================================================================
// void wifi_setup(){
//   nvs_flash_init();
//   esp_netif_init();
//   esp_event_loop_create_default();
//   wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//   esp_err_t err = esp_wifi_init(&cfg);
//   if(err != ESP_OK){
//     Serial.println("Error Init Wifi");
//     return;
//   }

//   // esp_wifi_set_storage(WIFI_STORAGE_RAM);

//   err = esp_wifi_set_mode(WIFI_MODE_STA);
//   if(err != ESP_OK){
//     Serial.println("Error Set WiFi Mode");
//     return;
//   }

//   err = esp_wifi_start();
//   if (err != ESP_OK)
//   {
//     Serial.print("esp_wifi_start error: ");
//     Serial.println(esp_err_to_name(err));
//     return;
//   }

//   Serial.println("Berhasil Setup Wifi!");
// }

// void wifi_scan(){
//   esp_err_t err = esp_wifi_scan_start(NULL, true);
//   if(err != ESP_OK){
//     Serial.print("esp_wifi_scan_start error : ");
//     Serial.println(esp_err_to_name(err));
//     return;
//   }

//   uint16_t apNum  = MAX_AP_RECORDS;
//   wifi_ap_record_t apRecords[MAX_AP_RECORDS];
//   memset(apRecords, 0, sizeof(apRecords));

//   err = esp_wifi_scan_get_ap_records(&apNum, apRecords);
//   if(err != ESP_OK){
//     Serial.print("esp_wifi_scan_get_ap_record error : ");
//     Serial.println(esp_err_to_name(err));
//     return;
//   }

//   lv_obj_clean(objects.wifi_list);
  
//   if(apNum > 0){
//     print_ssid(apRecords, apNum);
//   }
//   else{
//     Serial.println("Ga ada WiFi yang kedetect bejir");
//   }
// }

// bool ssid_is_unique(wifi_ap_record_t uniqueRecords[], uint16_t uniqueCount, char* ssid){
//   for(uint16_t i=0;i<uniqueCount;i++){
//     if(strcmp((char*)uniqueRecords[i].ssid, ssid) == 0){
//       return false;
//     }
//   }
//   return true;
// }

// void print_ssid(wifi_ap_record_t apRecords[], uint16_t apNum){
//   wifi_ap_record_t uniqueRecords[MAX_AP_RECORDS];
//   uint16_t uniqueCount = 0;

//   lv_obj_t * btn;

//   for(uint16_t i = 0;i<apNum;i++){
//     if(strlen((char *)apRecords[i].ssid) == 0) continue;

//     if(ssid_is_unique(uniqueRecords, uniqueCount, (char*)apRecords[i].ssid)){
//       btn = lv_list_add_button(objects.wifi_list, LV_SYMBOL_WIFI, (char *) apRecords[i].ssid);
//       lv_obj_add_event_cb(btn, action_list_btn_wifi_network, LV_EVENT_CLICKED, NULL);
//       uniqueRecords[uniqueCount++] = apRecords[i];
//     }
//   }
//   char apStatus[20];
//   sprintf(apStatus, "%d Wi-Fi terdeteksi", uniqueCount);

//   lv_label_set_text(objects.wifi_status, apStatus);
//   for(uint16_t i=0;i<uniqueCount;i++){
//     Serial.print("SSID : "); Serial.println((char*)uniqueRecords[i].ssid); 
//   }
// }

// ================================================================================================================================================================

// void connect_wifi(){
//   const char* ssid = lv_label_get_text(object.name_ssid);
//   const char* password = lv_textarea_get_text(object.textarea_password);

//   lv_obj_remove_flag(object.panel_status_connection, LV_OBJ_FLAG_HIDDEN);
//   lv_label_set_text(object.wifi_ssid_status_connection, ssid);

// }