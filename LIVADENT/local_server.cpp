#include <stdio.h>
#include "esp32-hal.h"
#include "Arduino.h"
#include "local_server.h"
#include "wifi_livadent.h"

uint32_t last_milli = 0;

bool session_timeout = false;
// bool web_app_timeout = false;

AsyncWebServer server(8888);
char session_key[41] = "";

void init_json() {
  global_variable["mac"] = WiFi.macAddress();
  global_variable["local_ip"] = WiFi.localIP().toString();
  global_variable["is_ph"] = true;    // Ini nanti diganti sesuai dengan kondisi sensor
  global_variable["is_visc"] = true;  // Ini juga
  global_variable["on_going"] = false;
  global_variable["web_app"] = false;
  global_variable["ready"] = false;
  global_variable["stop"] = false;
}

void update_json() {
  if (millis() - last_milli > TIMEOUT_WEB_APP_CONNECTION && !web_app_timeout) {
    global_variable["web_app"] = false;

    lv_image_set_src(objects.web_app_indicator_home, &img_no_web_app);
    lv_label_set_text(objects.web_app_status_home, "Tdk Terhubung ke");
    lv_obj_set_style_text_color(objects.web_app_status_home, lv_color_hex(0xEB5757), NULL);
    lv_obj_set_style_text_color(objects.web_app_home, lv_color_hex(0xEB5757), NULL);

    lv_image_set_src(objects.is_prototype_ready_indicator, &img_wifi_failed);
    lv_label_set_text(objects.is_prototype_ready_label, "Alat Belum Siap Digunakan");
    lv_obj_set_style_text_color(objects.is_prototype_ready_label, lv_color_hex(0xEB5757), NULL);

    session_timeout = true;
    web_app_timeout = true;
    strcpy(session_key, "");

    Serial.println("No Respond from Web App! (Session Deleted)");
  }
}

bool require_session_key(AsyncWebServerRequest *request, bool is_post) {
  if (!request->hasParam("session_key", is_post)) {
    request->send(403, "application/json", "{\"reason\":\"Session Key Not Available\"}");
    return false;
  }
  const AsyncWebParameter *param = request->getParam("session_key", is_post);
  if (strcmp(session_key, param->value().c_str()) != 0) {
    Serial.println("Session key tidak sama");
    request->send(403, "application/json", "{\"reason\":\"Session Key Mismatch\"}");
    return false;
  }
  return true;
}

void init_endpoint_webserver() {

  server.on("/connect", HTTP_POST, [](AsyncWebServerRequest *request) {
    const AsyncWebParameter *session_key_param = request->getParam("session_key", true);
    
    
    if (global_variable["web_app"] == true && strcmp(session_key, session_key_param->value().c_str()) != 0) {
      request->send(405, "application/json", "{\"reason\" : \"alat terhubung pada web app yang lain\"}");
      Serial.println("Being Used");
      return;
    }

    if (request->hasParam("session_key", true)) {
      strcpy(session_key, session_key_param->value().c_str());
    } else {
      request->send(405, "application/json", "{\"reason\" : \"Session Key Not Available\"}");
      Serial.println("No session key sent");
      return;
    }

    session_timeout = false;
    web_app_timeout = false;
    last_milli = millis();
    global_variable["web_app"] = true;

    lv_image_set_src(objects.web_app_indicator_home, &img_web_app);
    lv_label_set_text(objects.web_app_status_home, "Terhubung ke");
    lv_obj_set_style_text_color(objects.web_app_status_home, lv_color_hex(0x000000), NULL);
    lv_obj_set_style_text_color(objects.web_app_home, lv_color_hex(0x000000), NULL);

    lv_image_set_src(objects.is_prototype_ready_indicator, &img_wifi_success);
    lv_label_set_text(objects.is_prototype_ready_label, "Alat Siap Digunakan");
    lv_obj_set_style_text_color(objects.is_prototype_ready_label, lv_color_hex(0x27AE60), NULL);

    Serial.println("Device Connected to Web App");
    JsonDocument ret_val;
    String str_ret_val;
    ret_val["info"] = "success";
    ret_val["is_ph"] = global_variable["is_ph"];
    ret_val["is_visc"] = global_variable["is_visc"];
    serializeJson(ret_val, str_ret_val);
    request->send(200, "application/json", str_ret_val);
  });

  server.on("/interval", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (global_variable["web_app"] == false) {
      Serial.println("Warning, Web App Belum terkoneksi tapi ada yang pake endpoint interval");
      request->send(403);
      return;
    }

    if (!require_session_key(request, true)) return;

    last_milli = millis();
    request->send(200, "application/json", "{\"info\" : \"success\", \"message\" : \"Interval Updated!\"}");
  });

  server.on("/start", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (global_variable["web_app"] == false) {
      request->send(403);
      return;
    }

    if (!require_session_key(request, true)) return;

    if (global_variable["on_going"] == true) {
      JsonDocument ret_val_json;
      ret_val_json["info"] = "error";
      ret_val_json["reason"] = "already running";
      String ret_val;
      serializeJson(ret_val_json, ret_val);
      request->send(403, "application/json", ret_val);
      return;
    }

    if (!(request->hasParam("schedule_id", true) && request->hasParam("student_id", true) && request->hasParam("key", true))) {
      request->send(400, "application/json", "{\"reason\":\"Missing Parameters\"}");
      return;
    }

    const AsyncWebParameter *schedule_id_param = request->getParam("schedule_id", true);
    const AsyncWebParameter *student_id_param = request->getParam("student_id", true);
    const AsyncWebParameter *key_param = request->getParam("key", true);

    String schedule_key = schedule_id_param->value().c_str();
    String student_key = student_id_param->value().c_str();
    String key = key_param->value().c_str();

    global_variable["on_going"] = true;

    Serial.print("Ini key screening : ");
    Serial.println(key);

    lv_scr_load_anim(objects.screening, LV_SCR_LOAD_ANIM_FADE_IN, 200, 0, false);
    start_screening_process(schedule_key, student_key, key);

    request->send(200, "application/json", "{\"info\":\"success\"}");
  });

  server.on("/stop", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (global_variable["web_app"] == false) {
      request->send(403);
      return;
    }

    if (!require_session_key(request, true)) return;

    global_variable["stop"] = true;

    JsonDocument ret_val_json;
    ret_val_json["info"] = "screening_stopped";
    String ret_val;
    serializeJson(ret_val_json, ret_val);
    request->send(200, "application/json", ret_val);
  });
}