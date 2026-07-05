#include "WiFiType.h"
#include "event_handler.h"

bool is_hide = true;
// EVENT HANDLER SECTION!!

void display_result() {
}

void main_menu_loaded(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_SCREEN_LOADED) {
    if (WiFi.status() == WL_CONNECTED) {
      lv_image_set_src(objects.wifi_indicator_home, &img_connected_black);
      lv_label_set_text(objects.wifi_status_home, "Terhubung ke");
      lv_obj_set_style_text_color(objects.wifi_status_home, lv_color_hex(0x000000), NULL);
      lv_obj_set_style_text_color(objects.wifi_ssid_home, lv_color_hex(0x000000), NULL);
      lv_label_set_text(objects.wifi_ssid_home, WiFi.SSID().c_str());
      lv_obj_remove_flag(objects.web_app, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(objects.mulai_btn, LV_OBJ_FLAG_HIDDEN);
      lv_obj_remove_flag(objects.is_prototype_ready_container, LV_OBJ_FLAG_HIDDEN);
    } else {
      lv_image_set_src(objects.wifi_indicator_home, &img_no_connection_red);
      lv_label_set_text(objects.wifi_status_home, "Tdk Terhubung ke");
      lv_label_set_text(objects.wifi_ssid_home, "Wi-Fi");
      lv_obj_set_style_text_color(objects.wifi_status_home, lv_color_hex(0xeb5757), NULL);
      lv_obj_set_style_text_color(objects.wifi_ssid_home, lv_color_hex(0xeb5757), NULL);
      lv_obj_add_flag(objects.web_app, LV_OBJ_FLAG_HIDDEN);
      lv_label_set_text(objects.mulai_text, "Mulai Mode Offline");
      lv_obj_remove_flag(objects.mulai_btn, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(objects.is_prototype_ready_container, LV_OBJ_FLAG_HIDDEN);
    }
  }
}

void result_loaded(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);
  File file = LittleFS.open(CONFIG_PATH, "r");
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();
  doc["on_going"] = false;
  File write_file = LittleFS.open(CONFIG_PATH, "w");
  serializeJson(doc, write_file);
  write_file.close();
  if (code == LV_EVENT_SCREEN_LOADED) {
    if (WiFi.status() != WL_CONNECTED) {
      lv_image_set_src(objects.wifi_indicator_screening, &img_no_connection);
      lv_label_set_text(objects.wifi_status_screening, "Tidak Terhubung ke");
      lv_label_set_text(objects.wifi_ssid_screening, "Wi-Fi");
    } else {
      lv_image_set_src(objects.wifi_indicator_screening, &img_connected);
      lv_label_set_text(objects.wifi_status_screening, "Terhubung ke");
      lv_label_set_text(objects.wifi_ssid_screening, WiFi.SSID().c_str());
    }

    char ph_value[10];
    char visc_value[10];
    snprintf(ph_value, sizeof(ph_value), "%.2f", ph_sensor);
    snprintf(visc_value, sizeof(visc_value), "%.2f cP", visc_sensor);
    lv_label_set_text(objects.ph_value, ph_value);
    lv_label_set_text(objects.visc_value, visc_value);

    if (mlp_risk_label == 0) {
      lv_image_set_src(objects.risk_indicator, &img_low_risk);
      lv_obj_set_style_bg_color(objects.risk_bg, lv_color_hex(0xC8F4DB), NULL);
      lv_label_set_text(objects.risk_label, "Risiko Karies Gigi Rendah");
      lv_label_set_text(objects.risk_desc, "Kondisi gigi dalam keadaan baik");
    } else if (mlp_risk_label == 1) {
      lv_image_set_src(objects.risk_indicator, &img_medium_risk);
      lv_obj_set_style_bg_color(objects.risk_bg, lv_color_hex(0xFFE080), NULL);
      lv_label_set_text(objects.risk_label, "Risiko Karies Gigi Sedang");
      lv_label_set_text(objects.risk_desc, "Terapkan langkah preventif");
    } else if (mlp_risk_label == 2) {
      lv_image_set_src(objects.risk_indicator, &img_high_risk);
      lv_obj_set_style_bg_color(objects.risk_bg, lv_color_hex(0xFFD9D9), NULL);
      lv_label_set_text(objects.risk_label, "Risiko Karies Gigi Tinggi");
      lv_label_set_text(objects.risk_desc, "Waspadalah Boloooo");
    } else {
      lv_scr_load_anim(objects.main_menu, LV_SCR_LOAD_ANIM_FADE_IN, 0, 200, false);
    }
  }
}

void splash_screen_done_timer(lv_timer_t* timer) {
  lv_scr_load_anim(objects.main_menu, LV_SCR_LOAD_ANIM_FADE_IN, 200, 0, false);

  lv_timer_del(timer);
}

void splash_screen_loaded(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_SCREEN_LOADED) {
    lv_obj_remove_flag(objects.process_init, LV_OBJ_FLAG_HIDDEN);

    saved_SSID = preferences.getString("saved_SSID", "");
    saved_Password = preferences.getString("saved_Password", "");

    Serial.print("Saved SSID : ");
    Serial.println(saved_SSID.c_str());

    if (saved_SSID != "") {
      lv_label_set_text(objects.process_init, "Connecting Saved SSID...");
      Serial.print("WiFi is Auto Connecting ");
      Serial.println(saved_SSID.c_str());
      WiFi.begin(saved_SSID.c_str(), saved_Password.c_str());
      wifi_state = WIFI_STATE_CONNECTING;
      wifi_connect_start_ms = millis();

      if (wifi_timer != NULL) {
        lv_timer_del(wifi_timer);
      }
      wifi_timer = lv_timer_create(WiFi_connect_tick, 300, NULL);
    }

    // if (WiFi.status() == WL_CONNECTED) {
    //   lv_label_set_text(objects.process_init, "Database Init...");
    //   Serial.println("Database Init...");
    //   database_init();
    //   local_server_init();
    //   set_device_active();
    // }
    // else{
    //   Serial.println("Not Connected Yet");
    // }

    // Init sensor pH
    // Init sensor viskositas
    lv_label_set_text(objects.process_init, "Prototype Initialized!");

    lv_timer_create(splash_screen_done_timer, 5000, NULL);
  }
}

void wifi_settings_heading_interface_update() {
  if (WiFi.status() != WL_CONNECTED) {
    lv_image_set_src(objects.wifi_indicator_settings, &img_no_connection);
    lv_label_set_text(objects.wifi_status_settings, "Tidak Terhubung ke");
    lv_label_set_text(objects.wifi_ssid_settings, "Wi-Fi");
  } else {
    lv_image_set_src(objects.wifi_indicator_settings, &img_connected);
    lv_label_set_text(objects.wifi_status_settings, "Terhubung ke");
    lv_label_set_text(objects.wifi_ssid_settings, WiFi.SSID().c_str());
  }
}

void wifi_settings_loaded(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_SCREEN_LOADED) {
    if (WiFi.status() != WL_CONNECTED) {
      lv_obj_set_style_bg_opa(objects.disconnect_wifi_btn, 100, NULL);
      lv_obj_remove_flag(objects.disconnect_wifi_btn, LV_OBJ_FLAG_CLICKABLE);
    } else {
      lv_obj_set_style_bg_opa(objects.disconnect_wifi_btn, 255, NULL);
      lv_obj_add_flag(objects.disconnect_wifi_btn, LV_OBJ_FLAG_CLICKABLE);
    }
  }
  wifi_settings_heading_interface_update();
}

void screening_loaded(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_SCREEN_LOADED) {
    if (WiFi.status() != WL_CONNECTED) {
      lv_image_set_src(objects.wifi_indicator_screening, &img_no_connection);
      lv_label_set_text(objects.wifi_status_screening, "Tidak Terhubung ke");
      lv_label_set_text(objects.wifi_ssid_screening, "Wi-Fi");
    } else {
      lv_image_set_src(objects.wifi_indicator_screening, &img_connected);
      lv_label_set_text(objects.wifi_status_screening, "Terhubung ke");
      lv_label_set_text(objects.wifi_ssid_screening, WiFi.SSID().c_str());
    }

    File file = LittleFS.open(CONFIG_PATH, "r");
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (WiFi.status() != WL_CONNECTED && doc["on_going"] != true) {  // && Ketika Tidak lagi On progress
      lv_obj_remove_flag(objects.container_peringatan_penggunaan, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(objects.screening_on_progress, LV_OBJ_FLAG_HIDDEN);

      lv_obj_remove_flag(objects.back_btn_screening, LV_OBJ_FLAG_HIDDEN);
      lv_obj_remove_flag(objects.arrow_start_screening, LV_OBJ_FLAG_HIDDEN);

      lv_label_set_text(objects.start_stop_screening_label, "Mulai Skrining");
      lv_obj_set_style_bg_color(objects.start_stop_screening_btn, lv_color_hex(0x2196f3), NULL);
    } else if (WiFi.status() == WL_CONNECTED && doc["on_going"] == true) {
      lv_obj_add_flag(objects.offline_indicator, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(objects.container_peringatan_penggunaan, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(objects.container_screening_btn, LV_OBJ_FLAG_HIDDEN);
      lv_obj_remove_flag(objects.screening_on_progress, LV_OBJ_FLAG_HIDDEN);

      lv_obj_add_flag(objects.back_btn_screening, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(objects.arrow_start_screening, LV_OBJ_FLAG_HIDDEN);

      lv_label_set_text(objects.start_stop_screening_label, "Hentikan Skrining");
      lv_obj_set_style_bg_color(objects.start_stop_screening_btn, lv_color_hex(0xEB5757), NULL);
    }
  }
}

void hide_password_btn(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_CLICKED) {
    if (is_hide) {
      lv_image_set_src(objects.hide_password_btn, &img_eye);
      lv_textarea_set_password_mode(objects.textarea_password, false);
      is_hide = false;
    } else {
      lv_image_set_src(objects.hide_password_btn, &img_eye_hide);
      lv_textarea_set_password_mode(objects.textarea_password, true);
      is_hide = true;
    }
  }
}

void disconnect_btn_interface_update() {
  if (WiFi.status() != WL_CONNECTED) {
    lv_obj_set_style_bg_opa(objects.disconnect_wifi_btn, 100, NULL);
    lv_obj_remove_flag(objects.disconnect_wifi_btn, LV_OBJ_FLAG_CLICKABLE);
  } else {
    lv_obj_set_style_bg_opa(objects.disconnect_wifi_btn, 255, NULL);
    lv_obj_add_flag(objects.disconnect_wifi_btn, LV_OBJ_FLAG_CLICKABLE);
  }
}


// bool checkbox_auto_connect(lv_event_t * e){
//   lv_event_code_t code = lv_event_get_code(e);

//   if(code == LV_EVENT_VALUE_CHANGED){
//     return lv_obj_has_state(objects.is_auto_connect, LV_STATE_CHECKED);
//   }
// }

void disconnect_status_timer(lv_timer_t* timer) {
  lv_obj_add_flag(objects.panel_status_connection, LV_OBJ_FLAG_HIDDEN);
  disconnect_btn_interface_update();
  wifi_settings_heading_interface_update();

  lv_timer_del(timer);
}

void disconnect_btn(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_CLICKED) {
    WiFi.disconnect();
    if (WiFi.status() != WL_CONNECTED) {
      lv_obj_remove_flag(objects.panel_status_connection, LV_OBJ_FLAG_HIDDEN);
      lv_obj_remove_flag(objects.wifi_success, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(objects.wifi_ssid_status_connection, LV_OBJ_FLAG_HIDDEN);
      lv_label_set_text(objects.detail_status_wifi_connection, "Berhasil Memutuskan Jaringan");
    } else {
      lv_obj_remove_flag(objects.panel_status_connection, LV_OBJ_FLAG_HIDDEN);
      lv_obj_remove_flag(objects.wifi_failed, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(objects.wifi_ssid_status_connection, LV_OBJ_FLAG_HIDDEN);
      lv_label_set_text(objects.detail_status_wifi_connection, "Gagal Memutuskan Jaringan");
    }
    lv_timer_create(disconnect_status_timer, 5000, NULL);
  }
}

void wifi_settings_btn(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_CLICKED) {
    lv_scr_load_anim(objects.wifi_settings, LV_SCR_LOAD_ANIM_FADE_IN, 200, 0, false);
    lv_obj_remove_flag(objects.panel_status_connection, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.wifi_spinner, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.wifi_success, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.wifi_failed, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.wifi_ssid_status_connection, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(objects.detail_status_wifi_connection, "Memindai Wi-Fi...");
    lv_label_set_text(objects.wifi_status, "Memindai Wi-Fi...");

    lv_timer_create(wifi_scan_timer, 500, NULL);
  }
}

void list_btn_wifi_network(lv_event_t* e) {
  //     String get_Selected_SSID;

  lv_event_code_t code = lv_event_get_code(e);  //--> Get the event code.
  lv_obj_t* obj = (lv_obj_t*)lv_event_get_target(e);

  if (code == LV_EVENT_CLICKED) {
    LV_UNUSED(obj);
    //     //LV_LOG_USER("Clicked: %s", lv_list_get_button_text(objects.list_wifi_network_ws, obj));

    //     get_Selected_SSID = "";
    //     get_Selected_SSID = String(lv_list_get_button_text(objects.wifi_list, obj));

    //     Serial.println();
    //     Serial.print(get_Selected_SSID);
    //     Serial.println(" is clicked/touched.");

    //     if (WiFi.status() == WL_CONNECTED) {
    //       if (get_Selected_SSID == saved_SSID) return;
    //     }

    //     // Show the "panel_connect_ws" panel.
    //     lv_obj_remove_flag(objects.panel_connect_ws, LV_OBJ_FLAG_HIDDEN);

    //     // Hide the "panel_main_ws" panel.
    //     lv_obj_add_flag(objects.panel_main_ws, LV_OBJ_FLAG_HIDDEN);

    //     if (get_Selected_SSID == saved_SSID) {
    //       get_Selected_SSID = saved_SSID;
    //       get_Entered_SSID_Password = saved_Password;

    //       lv_textarea_set_text(objects.textarea_password_ws, get_Entered_SSID_Password.c_str());
    //     } else {
    //       lv_textarea_set_text(objects.textarea_password_ws, "");
    //     }
    const char* ssid = lv_list_get_button_text(objects.wifi_list, obj);
    lv_label_set_text(objects.name_ssid, ssid);

    lv_obj_remove_flag(objects.keyboard_password, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(objects.panel_input_wifi_password, LV_OBJ_FLAG_HIDDEN);
    //     lv_obj_set_style_text_color(objects.label_connecting_process_info_ws, lv_color_hex(0xff2a2c3b), LV_PART_MAIN | LV_STATE_DEFAULT);
    //     lv_label_set_text(objects.label_connecting_process_info_ws, "");

    //     update_UI();
  }
}

void back_wifi_btn(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_CLICKED) {
    lv_scr_load_anim(objects.main_menu, LV_SCR_LOAD_ANIM_FADE_IN, 200, 0, false);
  }
}

void wifi_cancel_btn(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_CLICKED) {
    lv_textarea_set_text(objects.textarea_password, "");
    lv_obj_add_flag(objects.panel_input_wifi_password, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.keyboard_password, LV_OBJ_FLAG_HIDDEN);

    lv_obj_clear_state(objects.is_auto_connect, LV_STATE_CHECKED);
    lv_image_set_src(objects.hide_password_btn, &img_eye_hide);
    lv_textarea_set_password_mode(objects.textarea_password, true);
    is_hide = true;
  }
}

void mulai_btn(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_CLICKED) {
    lv_scr_load_anim(objects.screening, LV_SCR_LOAD_ANIM_FADE_IN, 200, 0, false);
  }
}

void back_btn_screening(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_CLICKED) {
    lv_scr_load_anim(objects.main_menu, LV_SCR_LOAD_ANIM_FADE_IN, 200, 0, false);
  }
}

void process_timer(lv_timer_t* timer) {
  process();
  lv_timer_del(timer);
}

void start_stop_screening(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);

  File file = LittleFS.open(CONFIG_PATH, "r");
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();
  if (code == LV_EVENT_CLICKED) {
    if (doc["on_going"] == false) {
      Serial.println("Skrining Berjalan!");
      lv_obj_add_flag(objects.back_btn_screening, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(objects.arrow_start_screening, LV_OBJ_FLAG_HIDDEN);

      lv_label_set_text(objects.start_stop_screening_label, "Hentikan Skrining");
      lv_obj_set_style_bg_color(objects.start_stop_screening_btn, lv_color_hex(0xEB5757), NULL);
      doc["on_going"] = true;

      lv_obj_remove_flag(objects.screening_on_progress, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(objects.container_peringatan_penggunaan, LV_OBJ_FLAG_HIDDEN);

      // Prosedur Mulai Skrining (Gerakin sensor, servo, dll.) Di dalamnya
      lv_timer_create(process_timer, 500, NULL);
    } else {
      Serial.println("Skrining Berhenti!");
      lv_obj_remove_flag(objects.back_btn_screening, LV_OBJ_FLAG_HIDDEN);
      lv_obj_remove_flag(objects.arrow_start_screening, LV_OBJ_FLAG_HIDDEN);

      lv_label_set_text(objects.start_stop_screening_label, "Mulai Skrining");
      lv_obj_set_style_bg_color(objects.start_stop_screening_btn, lv_color_hex(0x2196f3), NULL);
      doc["on_going"] = false;

      lv_obj_add_flag(objects.screening_on_progress, LV_OBJ_FLAG_HIDDEN);
      lv_obj_remove_flag(objects.container_peringatan_penggunaan, LV_OBJ_FLAG_HIDDEN);
    }
    File write_file = LittleFS.open(CONFIG_PATH, "w");
    serializeJson(doc, write_file);
    write_file.close();
  }
}

void done_result_btn(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_CLICKED) {
    mlp_risk_label = 3;
    ph_sensor = -1;
    visc_sensor = -1;
    lv_scr_load_anim(objects.main_menu, LV_SCR_LOAD_ANIM_FADE_IN, 0, 200, false);
  }
}

void wifi_scan_timer(lv_timer_t* timer) {
  WiFi_scan();
  lv_obj_add_flag(objects.panel_status_connection, LV_OBJ_FLAG_HIDDEN);
  lv_obj_remove_flag(objects.wifi_ssid_status_connection, LV_OBJ_FLAG_HIDDEN);
  // lv_label_set_text(objects.detail_status_wifi_connection, "Memindai Wi-Fi...");

  lv_timer_del(timer);
}
