#include "lcd.h"
#include "wifi_livadent.h"

static wifi_conn_state_t wifi_state = WIFI_STATE_IDLE;
static uint32_t wifi_connect_start_ms = 0;
static lv_timer_t * wifi_timer = NULL;

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

void WiFi_scan(){
  int16_t apNum = WiFi.scanNetworks();
  Serial.println(apNum);

  lv_obj_t * btn;

  for(int16_t i=0;i<apNum;i++){
    btn = lv_list_add_button(objects.wifi_list, LV_SYMBOL_WIFI, WiFi.SSID(i).c_str());
    lv_obj_add_event_cb(btn, action_list_btn_wifi_network, LV_EVENT_CLICKED, NULL);
  }

  char nAP[25];
  sprintf(nAP, "%d Wi-Fi Terdeteksi", apNum);
  lv_label_set_text(objects.wifi_status, nAP);
}

void WiFi_connect_tick(lv_timer_t * timer){
  if(wifi_state == WIFI_STATE_CONNECTING){
    uint32_t elapsed = millis() - wifi_connect_start_ms;

    if(WiFi.status() == WL_CONNECTED){
      wifi_state = WIFI_STATE_SUCCESS;
      lv_obj_add_flag(objects.wifi_spinner, LV_OBJ_FLAG_HIDDEN);
      lv_obj_remove_flag(objects.wifi_success, LV_OBJ_FLAG_HIDDEN);
      lv_label_set_text(objects.detail_status_wifi_connection, "Berhasil Terhubung ke ");

      lv_obj_add_flag(objects.panel_status_connection, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(objects.keyboard_password, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(objects.panel_input_wifi_password, LV_OBJ_FLAG_HIDDEN);
      Serial.println("Berhasil Terhubung bossqqq");
      lv_timer_del(wifi_timer);
      wifi_timer = NULL;
    }
    else if(elapsed >= WIFI_CONNECT_TIMEOUT){
      wifi_state = WIFI_STATE_FAILED;

      WiFi.disconnect();
      lv_obj_add_flag(objects.wifi_spinner, LV_OBJ_FLAG_HIDDEN);
      lv_obj_remove_flag(objects.wifi_failed, LV_OBJ_FLAG_HIDDEN);
      lv_label_set_text(objects.detail_status_wifi_connection, "Gagal Terhubung ke ");

      Serial.println("WiFi Timeout!");

      lv_timer_del(wifi_timer);
      wifi_timer = NULL;
    }
  }
}

void WiFi_connect(lv_event_t * e){
  Serial.println("TOMBOL KONEK DIPENCET");
  const char* ssid = lv_label_get_text(objects.name_ssid);
  const char* password = lv_textarea_get_text(objects.textarea_password);

  lv_obj_remove_flag(objects.panel_status_connection, LV_OBJ_FLAG_HIDDEN);
  lv_label_set_text(objects.wifi_ssid_status_connection, ssid);
  lv_obj_remove_flag(objects.wifi_spinner, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(objects.wifi_failed, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(objects.wifi_success, LV_OBJ_FLAG_HIDDEN);
  update_UI();

  WiFi.begin(ssid, password);
  wifi_state = WIFI_STATE_CONNECTING;
  wifi_connect_start_ms = millis();
  
  if(wifi_timer != NULL){
    lv_timer_del(wifi_timer);
  }
  wifi_timer = lv_timer_create(WiFi_connect_tick, 100, NULL);
}