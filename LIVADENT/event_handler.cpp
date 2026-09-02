#include "WiFiType.h"
#include "event_handler.h"

bool is_hide = true;
// EVENT HANDLER SECTION!!

void my_delay(lv_timer_t* time) {
  delay(10000);

  lv_timer_del(time);
}

void main_menu_loaded(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_add_flag(objects.offline_warning, LV_OBJ_FLAG_HIDDEN);

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

    if (global_variable["ready"] == false) {
      global_variable["ready"] = true;
    }
  }
}

void result_loaded(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);

  global_variable["on_going"] = false;

  if (code == LV_EVENT_SCREEN_LOADED) {
    if (WiFi.status() != WL_CONNECTED) {
      lv_obj_add_flag(objects.wifi_detail_status_result, LV_OBJ_FLAG_HIDDEN);
      lv_obj_remove_flag(objects.offline_indicator_result, LV_OBJ_FLAG_HIDDEN);
      lv_image_set_src(objects.wifi_indicator_result, &img_no_connection);
      lv_label_set_text(objects.wifi_status_result, "Tidak Terhubung ke");
      lv_label_set_text(objects.wifi_ssid_result, "Wi-Fi");
    } else {
      lv_obj_add_flag(objects.offline_indicator_result, LV_OBJ_FLAG_HIDDEN);
      lv_obj_remove_flag(objects.wifi_detail_status_result, LV_OBJ_FLAG_HIDDEN);
      lv_image_set_src(objects.wifi_indicator_result, &img_connected);
      lv_label_set_text(objects.wifi_status_result, "Terhubung ke");
      lv_label_set_text(objects.wifi_ssid_result, WiFi.SSID().c_str());
    }

    if (ph_sensor < 0 || ph_sensor > 14 || visc_sensor < 0 || visc_sensor > 5) {  // Aku menganggap kalo viskositas lebih dari 5 udah egak normal
      lv_scr_load_anim(objects.screening, LV_SCR_LOAD_ANIM_FADE_IN, 0, 200, false);
      lv_obj_remove_flag(objects.screening_failed, LV_OBJ_FLAG_HIDDEN);
      lv_obj_remove_flag(objects.back_in_second, LV_OBJ_FLAG_HIDDEN);
      return;
    }

    char ph_value[10];
    char visc_value[10];
    snprintf(ph_value, sizeof(ph_value), "%.2f", ph_sensor);
    snprintf(visc_value, sizeof(visc_value), "%.2f cP", visc_sensor);
    lv_label_set_text(objects.ph_value, ph_value);
    lv_label_set_text(objects.visc_value, visc_value);

    Serial.print("Result :"); Serial.println(mlp_risk_label);

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
      lv_label_set_text(objects.risk_desc, "Lakukan pemeriksaan ke dokter gigi");
    } else {
      lv_image_set_src(objects.risk_indicator, &img_high_risk);
      lv_obj_set_style_bg_color(objects.risk_bg, lv_color_hex(0xBDBDBD), NULL);
      lv_label_set_text(objects.risk_label, "Terjadi Kesalahan");
      lv_label_set_text(objects.risk_desc, "Harap hubungi pihak teknis");
      // lv_scr_load_anim(objects.main_menu, LV_SCR_LOAD_ANIM_FADE_IN, 0, 200, false);
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

    if (saved_SSID != "") {
      lv_label_set_text(objects.process_init, "Connecting Saved SSID...");
      WiFi.begin(saved_SSID.c_str(), saved_Password.c_str());
      wifi_state = WIFI_STATE_CONNECTING;
      wifi_connect_start_ms = millis();
      if (wifi_timer != NULL) lv_timer_del(wifi_timer);
      wifi_timer = lv_timer_create(WiFi_connect_tick, 300, NULL);
    }

    if (!ads.begin()) {
      lv_label_set_text(objects.process_init, "ADS Gagal Inisialisasi! Harap mulai ulang!");
      Serial.println("ADS DSADJAPDAJ");
    } else {
      Serial.println("Anjay ADS");
      is_ads = true;
      init_json();
    }

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
    global_variable["ready"] = false;

    if (WiFi.status() != WL_CONNECTED) {
      lv_obj_set_style_bg_opa(objects.disconnect_wifi_btn, 100, NULL);
      lv_obj_remove_flag(objects.disconnect_wifi_btn, LV_OBJ_FLAG_CLICKABLE);
    } else {
      lv_obj_set_style_bg_opa(objects.disconnect_wifi_btn, 255, NULL);
      lv_obj_add_flag(objects.disconnect_wifi_btn, LV_OBJ_FLAG_CLICKABLE);
    }

    lv_label_set_text(objects.current_page_pagination, "Page 1");
    lv_obj_set_style_img_opa(objects.wifi_page_prev_btn, 20, NULL);
    lv_obj_set_style_img_opa(objects.wifi_page_next_btn, 255, NULL);
  }
  wifi_settings_heading_interface_update();
}

void screening_loaded(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_SCREEN_LOADED) {
    if (WiFi.status() != WL_CONNECTED) {
      lv_obj_add_flag(objects.wifi_detail_status_screening, LV_OBJ_FLAG_HIDDEN);
      lv_obj_remove_flag(objects.offline_indicator_screening, LV_OBJ_FLAG_HIDDEN);
      lv_image_set_src(objects.wifi_indicator_screening, &img_no_connection);
      lv_label_set_text(objects.wifi_status_screening, "Tidak Terhubung ke");
      lv_label_set_text(objects.wifi_ssid_screening, "Wi-Fi");
    } else {
      lv_obj_add_flag(objects.offline_indicator_screening, LV_OBJ_FLAG_HIDDEN);
      lv_obj_remove_flag(objects.wifi_detail_status_screening, LV_OBJ_FLAG_HIDDEN);
      lv_image_set_src(objects.wifi_indicator_screening, &img_connected);
      lv_label_set_text(objects.wifi_status_screening, "Terhubung ke");
      lv_label_set_text(objects.wifi_ssid_screening, WiFi.SSID().c_str());
    }

    lv_obj_remove_flag(objects.start_stop_screening_btn, LV_OBJ_FLAG_HIDDEN);
    if (WiFi.status() != WL_CONNECTED && global_variable["on_going"] != true) {  // && Ketika Tidak lagi On progress
      lv_obj_remove_flag(objects.container_screening_btn, LV_OBJ_FLAG_HIDDEN);
      lv_obj_remove_flag(objects.container_peringatan_penggunaan, LV_OBJ_FLAG_HIDDEN);
      lv_obj_remove_flag(objects.back_btn_screening, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(objects.screening_on_progress, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(objects.screening_failed, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(objects.back_in_second, LV_OBJ_FLAG_HIDDEN);

      lv_obj_remove_flag(objects.back_btn_screening, LV_OBJ_FLAG_HIDDEN);
      lv_obj_remove_flag(objects.arrow_start_screening, LV_OBJ_FLAG_HIDDEN);

      lv_label_set_text(objects.start_stop_screening_label, "Mulai Skrining");
      lv_obj_set_style_bg_color(objects.start_stop_screening_btn, lv_color_hex(0x2196f3), NULL);
    } else if (WiFi.status() == WL_CONNECTED && global_variable["on_going"] == true) {
      Serial.println("GOKILL");
      lv_obj_add_flag(objects.offline_indicator_screening, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(objects.container_peringatan_penggunaan, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(objects.container_screening_btn, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(objects.screening_failed, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(objects.back_in_second, LV_OBJ_FLAG_HIDDEN);
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
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t* obj = (lv_obj_t*)lv_event_get_target(e);

  if (code == LV_EVENT_CLICKED) {
    LV_UNUSED(obj);
    const char* ssid = lv_list_get_button_text(objects.wifi_list, obj);
    lv_label_set_text(objects.name_ssid, ssid);

    lv_obj_remove_flag(objects.keyboard_password, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(objects.panel_input_wifi_password, LV_OBJ_FLAG_HIDDEN);
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
    lv_obj_remove_flag(objects.offline_warning, LV_OBJ_FLAG_HIDDEN);
  }
}

void yes_screening_btn(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_CLICKED) {
    lv_scr_load_anim(objects.screening, LV_SCR_LOAD_ANIM_FADE_IN, 200, 0, false);
  }
}

void no_screening_btn(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_CLICKED) {
    lv_obj_add_flag(objects.offline_warning, LV_OBJ_FLAG_HIDDEN);
  }
}

// void back_btn_screening(lv_event_t* e) {
//   lv_event_code_t code = lv_event_get_code(e);

//   if (code == LV_EVENT_CLICKED) {
//     lv_scr_load_anim(objects.main_menu, LV_SCR_LOAD_ANIM_FADE_IN, 200, 0, false);
//   }
// }

void back_btn(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_CLICKED) {
    lv_scr_load_anim(objects.main_menu, LV_SCR_LOAD_ANIM_FADE_IN, 200, 0, false);
  }
}

void back_btn_failed(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_CLICKED) {
    lv_obj_add_flag(objects.back_in_second, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.screening_failed, LV_OBJ_FLAG_HIDDEN);
    lv_scr_load_anim(objects.main_menu, LV_SCR_LOAD_ANIM_FADE_IN, 200, 0, false);
  }
}

void cancel_offline_screening(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_CLICKED) {
    lv_obj_add_flag(objects.offline_warning, LV_OBJ_FLAG_HIDDEN);
  }
}

// void process_timer(lv_timer_t* timer) {
//   process("", "", "");
//   lv_timer_del(timer);
// }

void start_stop_screening(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_CLICKED) {
    if (global_variable["on_going"] == false) {
      Serial.println("Skrining Berjalan!");
      lv_obj_add_flag(objects.back_btn_screening, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(objects.arrow_start_screening, LV_OBJ_FLAG_HIDDEN);

      lv_label_set_text(objects.start_stop_screening_label, "Hentikan Skrining");
      lv_obj_set_style_bg_color(objects.start_stop_screening_btn, lv_color_hex(0xEB5757), NULL);
      global_variable["on_going"] = true;
      global_variable["ready"] = false;

      lv_obj_remove_flag(objects.screening_on_progress, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(objects.container_peringatan_penggunaan, LV_OBJ_FLAG_HIDDEN);

      // Prosedur Mulai Skrining (Gerakin sensor, servo, dll.) Di dalamnya
      start_screening_process("", "", "");
      // lv_timer_create(process_timer, 500, NULL);
    } else {
      Serial.println("Skrining Berhenti!");
      lv_obj_remove_flag(objects.back_btn_screening, LV_OBJ_FLAG_HIDDEN);
      lv_obj_remove_flag(objects.arrow_start_screening, LV_OBJ_FLAG_HIDDEN);

      lv_label_set_text(objects.start_stop_screening_label, "Mulai Skrining");
      lv_obj_set_style_bg_color(objects.start_stop_screening_btn, lv_color_hex(0x2196f3), NULL);
      lv_obj_add_flag(objects.screening_on_progress, LV_OBJ_FLAG_HIDDEN);
      lv_obj_remove_flag(objects.container_peringatan_penggunaan, LV_OBJ_FLAG_HIDDEN);
      stop_screening_process();

    }
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

  lv_timer_del(timer);
}

void update_next_prev_wifi_btn() {
  int total_pages = (wifi_ssid_count + WIFI_ITEMS_PER_PAGE - 1) / WIFI_ITEMS_PER_PAGE;
  if (wifi_current_page <= 0) {
    lv_obj_set_style_img_opa(objects.wifi_page_prev_btn, 20, NULL);
  } else {
    lv_obj_set_style_img_opa(objects.wifi_page_prev_btn, 255, NULL);
  }

  if (wifi_current_page + 1 >= total_pages) {
    lv_obj_set_style_img_opa(objects.wifi_page_next_btn, 20, NULL);
  } else {
    lv_obj_set_style_img_opa(objects.wifi_page_next_btn, 255, NULL);
  }
}

void wifi_page_prev_btn(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_CLICKED) {
    if (wifi_current_page > 0) {
      wifi_current_page--;
      render_wifi_page();
      char page_text[13];
      snprintf(page_text, sizeof(page_text), "%Page %d", wifi_current_page + 1);
      lv_label_set_text(objects.current_page_pagination, page_text);
      update_next_prev_wifi_btn();
    }
  }
}

void wifi_page_next_btn(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_CLICKED) {
    int total_pages = (wifi_ssid_count + WIFI_ITEMS_PER_PAGE - 1) / WIFI_ITEMS_PER_PAGE;
    if (wifi_current_page < total_pages - 1) {
      wifi_current_page++;
      render_wifi_page();
      char page_text[13];
      if (wifi_current_page + 1 >= total_pages) {
        snprintf(page_text, sizeof(page_text), "%Page %d (MAX)", wifi_current_page + 1);
      } else {
        snprintf(page_text, sizeof(page_text), "%Page %d", wifi_current_page + 1);
      }
      lv_label_set_text(objects.current_page_pagination, page_text);
      update_next_prev_wifi_btn();
    }
  }
}
