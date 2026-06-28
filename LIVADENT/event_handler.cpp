// #include "WiFiType.h"
#include "event_handler.h"

// EVENT HANDLER SECTION!!

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

void splash_screen_done_timer(lv_timer_t * timer){
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
    }

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

void wifi_scan_timer(lv_timer_t* timer) {
  WiFi_scan();
  lv_obj_add_flag(objects.panel_status_connection, LV_OBJ_FLAG_HIDDEN);
  lv_obj_remove_flag(objects.wifi_ssid_status_connection, LV_OBJ_FLAG_HIDDEN);
  // lv_label_set_text(objects.detail_status_wifi_connection, "Memindai Wi-Fi...");

  lv_timer_del(timer);
}
