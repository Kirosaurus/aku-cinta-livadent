#include "wifi_livadent.h"

bool is_ssid_unique(char unique_ssid[][33], int jumlah_unique_ssid, const char* ssid) {
  for (int i = 0; i < jumlah_unique_ssid; i++) {
    if (strcmp(unique_ssid[i], ssid) == 0) {
      Serial.println(ssid);
      return false;
    }
  }
  return true;
}

void WiFi_scan() {
  int16_t apNum = WiFi.scanNetworks();
  if (apNum < 0) {
    Serial.println("ERROR WIFI SCANNED!");
    return;
  }
  Serial.print("Jumlah WiFi SSID : ");Serial.println(apNum);
  char unique_ssid[MAX_AP_RECORDS][33];
  int jumlah_unique_ssid = 0;
  lv_obj_t* btn;

  lv_obj_clean(objects.wifi_list);

  for (int16_t i = 0; i < apNum; i++) {
    String wifi_ssid = WiFi.SSID(i);

    const char* ssid = wifi_ssid.c_str();

    if (is_ssid_unique(unique_ssid, jumlah_unique_ssid, ssid)) {
      strncpy(unique_ssid[jumlah_unique_ssid], ssid, 32);
      btn = lv_list_add_button(objects.wifi_list, LV_SYMBOL_WIFI, ssid);
      unique_ssid[jumlah_unique_ssid][32] = '\0';

      lv_obj_add_event_cb(btn, list_btn_wifi_network, LV_EVENT_CLICKED, NULL);

      jumlah_unique_ssid++;
      if (jumlah_unique_ssid >= MAX_AP_RECORDS) {
        break;
      }
    }
  }

  char nAP[25];
  if (jumlah_unique_ssid >= MAX_AP_RECORDS) {
    sprintf(nAP, "%d Wi-Fi Terdeteksi (MAX)", jumlah_unique_ssid);
  } else {
    sprintf(nAP, "%d Wi-Fi Terdeteksi", jumlah_unique_ssid);
  }
  lv_label_set_text(objects.wifi_status, nAP);

  // Serial.print("Total Heap: ");
  // Serial.println(ESP.getHeapSize());
  // Serial.print("Free Heap: ");
  // Serial.println(ESP.getFreeHeap());
  Serial.printf("Free DRAM: %u\n", heap_caps_get_free_size(MALLOC_CAP_8BIT));
  WiFi.scanDelete();
  // Serial.print("Total Heap: ");
  // Serial.println(ESP.getHeapSize());
  // Serial.print("Free Heap: ");
  // Serial.println(ESP.getFreeHeap());
  Serial.printf("Free DRAM: %u\n", heap_caps_get_free_size(MALLOC_CAP_8BIT));
}

void wifi_status_conn_timer(lv_timer_t* timer) {
  lv_obj_add_flag(objects.panel_status_connection, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(objects.keyboard_password, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(objects.panel_input_wifi_password, LV_OBJ_FLAG_HIDDEN);

  lv_timer_delete(timer);
}

void WiFi_connect_tick(lv_timer_t* timer) {
  if (wifi_state == WIFI_STATE_CONNECTING) {
    uint32_t elapsed = millis() - wifi_connect_start_ms;

    if (WiFi.status() == WL_CONNECTED) {
      wifi_state = WIFI_STATE_SUCCESS;
      lv_obj_add_flag(objects.wifi_spinner, LV_OBJ_FLAG_HIDDEN);
      lv_obj_remove_flag(objects.wifi_success, LV_OBJ_FLAG_HIDDEN);
      lv_label_set_text(objects.detail_status_wifi_connection, "Berhasil Terhubung ke ");

      Serial.println("Berhasil Terhubung bossqqq");
      lv_timer_del(wifi_timer);
      wifi_timer = NULL;

      bool is_auto_connect = lv_obj_has_state(objects.is_auto_connect, LV_STATE_CHECKED);

      if (is_auto_connect) {
        Serial.println("SSID SAVED!");
        preferences.putString("saved_SSID", WiFi.SSID());
        preferences.putString("saved_Password", lv_textarea_get_text(objects.textarea_password));
      }


      lv_timer_create(wifi_status_conn_timer, 3000, NULL);
      lv_textarea_set_text(objects.textarea_password, "");

      Serial.println("Database Init...");
      database_init();
      local_server_init();
      disconnect_btn_interface_update();
      wifi_settings_heading_interface_update();
    } else if (elapsed >= WIFI_CONNECT_TIMEOUT) {
      wifi_state = WIFI_STATE_FAILED;

      WiFi.disconnect();
      lv_obj_add_flag(objects.wifi_spinner, LV_OBJ_FLAG_HIDDEN);
      lv_obj_remove_flag(objects.wifi_failed, LV_OBJ_FLAG_HIDDEN);
      lv_label_set_text(objects.detail_status_wifi_connection, "Gagal Terhubung ke ");

      Serial.println("WiFi Timeout!");

      lv_timer_del(wifi_timer);
      wifi_timer = NULL;

      lv_timer_create(wifi_status_conn_timer, 3000, NULL);
      lv_textarea_set_text(objects.textarea_password, "");
      disconnect_btn_interface_update();
      wifi_settings_heading_interface_update();
    }
  }
}

void WiFi_connect(lv_event_t* e) {
  const char* ssid = lv_label_get_text(objects.name_ssid);
  const char* password = lv_textarea_get_text(objects.textarea_password);

  lv_obj_remove_flag(objects.panel_status_connection, LV_OBJ_FLAG_HIDDEN);
  lv_label_set_text(objects.detail_status_wifi_connection, "Menghubungkan ke ");
  lv_label_set_text(objects.wifi_ssid_status_connection, ssid);
  lv_obj_remove_flag(objects.wifi_ssid_status_connection, LV_OBJ_FLAG_HIDDEN);
  lv_obj_remove_flag(objects.wifi_spinner, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(objects.wifi_failed, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(objects.wifi_success, LV_OBJ_FLAG_HIDDEN);

  WiFi.begin(ssid, password);
  wifi_state = WIFI_STATE_CONNECTING;
  wifi_connect_start_ms = millis();

  if (wifi_timer != NULL) {
    lv_timer_del(wifi_timer);
  }
  wifi_timer = lv_timer_create(WiFi_connect_tick, 100, NULL);
  // disconnect_btn_interface_update();
}