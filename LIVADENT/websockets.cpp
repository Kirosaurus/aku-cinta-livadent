#include "websockets.h"

WebSocketsServer webSocket(81);

char session_key[41] = "";
uint32_t last_millis = millis();
bool session_timeout = false;
uint8_t activeClient = 255;
bool clientConnected = false;

void webSocketEvent(
  uint8_t clientNum,
  WStype_t type,
  uint8_t* payload,
  size_t length) {
  switch (type) {

    case WStype_CONNECTED:
      {
        Serial.printf(
          "Connection request from client %u\n",
          clientNum);

        // Sudah ada client aktif
        if (clientConnected) {
          Serial.println("Device already in use. Rejecting client.");

          webSocket.sendTXT(clientNum, "{\"info\":\"error\",\"reason\":\"device_in_use\"}");
          webSocket.disconnect(clientNum);
          break;
        }

        // Client pertama diterima
        clientConnected = true;
        activeClient = clientNum;

        Serial.printf("Client %u accepted\n", clientNum);

        webSocket.sendTXT(clientNum, "{\"info\":\"connection_accepted\"}");
        lv_image_set_src(objects.web_app_indicator_home, &img_web_app);
        lv_label_set_text(objects.web_app_status_home, "Terhubung ke");
        lv_obj_set_style_text_color(objects.web_app_status_home, lv_color_hex(0x000000), NULL);
        lv_obj_set_style_text_color(objects.web_app_home, lv_color_hex(0x000000), NULL);

        lv_image_set_src(objects.is_prototype_ready_indicator, &img_wifi_success);
        lv_label_set_text(objects.is_prototype_ready_label, "Alat Siap Digunakan");
        lv_obj_set_style_text_color(objects.is_prototype_ready_label, lv_color_hex(0x27AE60), NULL);
        break;
      }

    case WStype_DISCONNECTED:
      {
        Serial.printf(
          "Client %u disconnected\n",
          clientNum);

        if (clientConnected && clientNum == activeClient) {

          clientConnected = false;
          activeClient = 255;

          Serial.println("ESP32 is now available");
          // UPDATE INTERFACE DI LCD BIAR WEB APP DISCONNECT
          // lv_image_set_src(objects.web_app_indicator_home, &img_no_web_app);
          // lv_label_set_text(objects.web_app_status_home, "Tdk Terhubung ke");
          // lv_obj_set_style_text_color(objects.web_app_status_home, lv_color_hex(0xEB5757), NULL);
          // lv_obj_set_style_text_color(objects.web_app_home, lv_color_hex(0xEB5757), NULL);

          // lv_image_set_src(objects.is_prototype_ready_indicator, &img_wifi_failed);
          // lv_label_set_text(objects.is_prototype_ready_label, "Alat Belum Siap Digunakan");
          // lv_obj_set_style_text_color(objects.is_prototype_ready_label, lv_color_hex(0xEB5757), NULL);
        }

        if(global_variable["on_progress"] == true){
          global_variable["stop"] = true;

          JsonDocument ret_val_json;
          ret_val_json["info"] = "screening_stopped";
          String ret_val;
          serializeJson(ret_val_json, ret_val);
          webSocket.sendTXT(clientNum, ret_val);
        }

        break;
      }

    case WStype_TEXT:
      {
        Serial.print("Current Session Key : ");
        Serial.println(session_key);
        String json_data;
        Serial.print("Received: ");
        Serial.println((char*)payload);
        JsonDocument web_app_request;
        DeserializationError error = deserializeJson(web_app_request, payload);
        if (error) {
          Serial.println("web_app_request deserialization error!");
        }

        Serial.print("INFO : ");
        Serial.println((const char*)web_app_request["info"]);

        // BUAT RESPOND PING DARI WEB APP
        if (strcmp(web_app_request["info"], "PING") == 0) {
          Serial.println("PINGING");
          last_millis = millis();
          session_timeout = false;
          JsonDocument doc;
          doc["info"] = "PONG";
          String ret_val;

          serializeJson(doc, ret_val);
          Serial.print("PINGING DATA : ");
          serializeJson(doc, Serial);
          webSocket.sendTXT(
            clientNum,
            ret_val);
          break;
        }
        // BUAT CEK SESSION KEY
        else if (strcmp(web_app_request["info"], "session_key") == 0) {
          Serial.println("Session Key Given!");
          // Kalo misal session key egak ada, langsung ganti yang baru, kalo ada dicek dulu apakah sama atau egak. Kalo beda reject
          if (strcmp(session_key, "") == 0) {
            strcpy(session_key, web_app_request["session_key"]);
          } else if (strcmp(session_key, web_app_request["session_key"]) != 0) {
            String ret_val;
            JsonDocument doc;
            doc["info"] = "error";
            doc["reason"] = "session key not valid";
            serializeJson(doc, ret_val);
            webSocket.sendTXT(clientNum, ret_val);
            // webSocket.close();
            break;
          }
        }
        // NGASIH KONDISI SENSOR WAKTU INISIALISASI
        else if (strcmp(web_app_request["info"], "connect_init") == 0) {

          JsonDocument ret_val_json;
          ret_val_json["info"] = "Sensor Initialization";
          ret_val_json["is_ph"] = global_variable["is_ph"];
          ret_val_json["is_visc"] = global_variable["is_visc"];
          String ret_val;
          serializeJson(ret_val_json, ret_val);
          serializeJson(ret_val_json, Serial);
          webSocket.sendTXT(clientNum, ret_val);
          Serial.println("Send Init");
          break;
        }
        // RESPOND START DARI WEB APP
        else if (strcmp(web_app_request["info"], "start_screening") == 0) {
          JsonDocument ret_val_json;

          if (global_variable["on_going"] == true) {
            ret_val_json["info"] = "error";
            ret_val_json["reason"] = "already running";
            String ret_val;
            serializeJson(ret_val_json, ret_val);
            webSocket.sendTXT(clientNum, ret_val);
            break;
          }

          webSocket.sendTXT(clientNum, "{\"info\" : \"started\"}");

          global_variable["on_going"] = true;

          String schedule_key = web_app_request["schedule_id"];
          String student_key = web_app_request["student_id"];
          String key = web_app_request["key"];
          Serial.print("Ini key screening : ");
          Serial.println(key);

          // lv_timer_create(screening_timer, 500, NULL);
          lv_scr_load_anim(objects.screening, LV_SCR_LOAD_ANIM_FADE_IN, 200, 0, false);
          start_screening_process(schedule_key, student_key, key);
          break;
        }
        // BUAT BERHENTI SKRINING
        else if (strcmp(web_app_request["info"], "stop_screening") == 0) {
          global_variable["stop"] = true;

          JsonDocument ret_val_json;
          ret_val_json["info"] = "screening_stopped";
          String ret_val;
          serializeJson(ret_val_json, ret_val);
          webSocket.sendTXT(clientNum, ret_val);
        }
      }
    default:
      break;
  }
}

void updateSessionKey() {
  if (millis() - last_millis > TIMEOUT_WEB_APP_CONNECTION && !session_timeout) {
    Serial.println("No Respond from Web App! (Session Deleted)");

    // UPDATE INTERFACE DI LCD BIAR WEB APP DISCONNECT
    lv_image_set_src(objects.web_app_indicator_home, &img_no_web_app);
    lv_label_set_text(objects.web_app_status_home, "Tdk Terhubung ke");
    lv_obj_set_style_text_color(objects.web_app_status_home, lv_color_hex(0xEB5757), NULL);
    lv_obj_set_style_text_color(objects.web_app_home, lv_color_hex(0xEB5757), NULL);

    lv_image_set_src(objects.is_prototype_ready_indicator, &img_wifi_failed);
    lv_label_set_text(objects.is_prototype_ready_label, "Alat Belum Siap Digunakan");
    lv_obj_set_style_text_color(objects.is_prototype_ready_label, lv_color_hex(0xEB5757), NULL);
    session_timeout = true;
    strcpy(session_key, "");
  }
}