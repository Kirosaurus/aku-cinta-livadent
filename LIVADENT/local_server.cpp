#include "esp32-hal.h"
#include "Arduino.h"
#include "local_server.h"
#include "wifi_livadent.h"

//Konfigurasi Web Server & mDNS
const char* mdnsname = "esplivadent";
// char mdns_hostname[MDNN_NAME_BUFF_LEN] =
WebServer server(8888);

uint32_t last_milli = 0;

void init_json() {
  // String json = "{";
  // json += "\"mac\":\"" + get_mac() + "\",";
  // json += "\"webConnect\":false,";
  // json += "\"online\":true,";
  // json += "\"local_ip\":\"" + WiFi.localIP().toString() + "\"";
  // json += "}";
  JsonDocument doc;
  doc["mac"] = WiFi.macAddress();
  doc["local_ip"] = WiFi.localIP().toString();
  doc["is_ph"] = true;    // Ini nanti diganti sesuai dengan kondisi sensor
  doc["is_visc"] = true;  // Ini juga
  doc["on_going"] = false;
  doc["web_app"] = false;

  File file = LittleFS.open(CONFIG_PATH, "w");
  serializeJsonPretty(doc, file);
  file.close();
}

void get_json() {
  send_cors_headers();
  File file = LittleFS.open(CONFIG_PATH, "r");
  String json = file.readString();
  file.close();
  server.send(200, "application/json", json);
}

void update_json() {
  if (millis() - last_milli > TIMEOUT_WEB_APP_CONNECTION && !web_app_timeout) {
    File open_file = LittleFS.open(CONFIG_PATH, "r");
    JsonDocument doc;
    deserializeJson(doc, open_file);
    open_file.close();
    doc["web_app"] = false;
    File write_file = LittleFS.open(CONFIG_PATH, "w");
    serializeJsonPretty(doc, write_file);
    write_file.close();
    web_app_timeout = true;

    lv_image_set_src(objects.web_app_indicator_home, &img_no_web_app);
    lv_label_set_text(objects.web_app_status_home, "Tdk Terhubung ke");
    lv_obj_set_style_text_color(objects.web_app_status_home, lv_color_hex(0xEB5757), NULL);
    lv_obj_set_style_text_color(objects.web_app_home, lv_color_hex(0xEB5757), NULL);

    lv_image_set_src(objects.is_prototype_ready_indicator, &img_wifi_failed);
    lv_label_set_text(objects.is_prototype_ready_label, "Alat Belum Siap Digunakan");
    lv_obj_set_style_text_color(objects.is_prototype_ready_label, lv_color_hex(0xEB5757), NULL);

    Serial.println(millis() - last_milli);
  }
}

void web_app_connect() {
  JsonDocument doc;
  File file = LittleFS.open(CONFIG_PATH, "r");
  DeserializationError read_json_error = deserializeJson(doc, file);
  file.close();

    if(doc["web_app"] == true){
        server.send(200, "application/json", "{\"info\" : \"Alat sedang terkoneksi dengan web app lain\", \"is_success\" : false}");
        return;
    }

  JsonDocument new_doc;
  DeserializationError read_new_json_error = deserializeJson(new_doc, server.arg("plain"));

  JsonObject new_doc_obj = new_doc.as<JsonObject>();

  for (JsonPair i : new_doc_obj) {
    doc[i.key().c_str()] = i.value();
  }

  File write_file = LittleFS.open(CONFIG_PATH, "w");
  serializeJsonPretty(doc, write_file);
  write_file.close();

  lv_image_set_src(objects.web_app_indicator_home, &img_web_app);
  lv_label_set_text(objects.web_app_status_home, "Terhubung ke");
  lv_obj_set_style_text_color(objects.web_app_status_home, lv_color_hex(0x000000), NULL);
  lv_obj_set_style_text_color(objects.web_app_home, lv_color_hex(0x000000), NULL);

  lv_image_set_src(objects.is_prototype_ready_indicator, &img_wifi_success);
  lv_label_set_text(objects.is_prototype_ready_label, "Alat Siap Digunakan");
  lv_obj_set_style_text_color(objects.is_prototype_ready_label, lv_color_hex(0x27AE60), NULL);

  last_milli = millis();
  web_app_timeout = false;
  server.send(200, "application/json", "{\"info\" : \"Web App Berhasil Dihubungkan\", \"is_success\" : true}");
}

void web_app_interval() {
  JsonDocument doc;
  File file = LittleFS.open(CONFIG_PATH, "r");
  DeserializationError doc_error = deserializeJson(doc, file);

  if (doc["web_app"] == true) {
  }

  last_milli = millis();
  server.send(200, "application/json", "{\"info\" : \"Interval Updated!\"}");
}

void send_cors_headers() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  // server.sendHeader("Access-Control-Allow-Headers", "Content-Type, X-LivaDent-Token");
}

void local_server_init() {
  init_json();
  // if (!MDNS.begin(mdnsname))
  // {
  //     while (1)
  //     {
  //         Serial.println("Failed to Start MDNS! Please Reset...");
  //     }
  // }

  // MDNS.addService("http", "tcp", 1111);

  server.on("/", HTTP_GET, get_json);
  server.on("/status", HTTP_GET, get_json);
  server.on("/connect", HTTP_POST, web_app_connect);
  server.on("/interval", HTTP_GET, web_app_interval);
  server.begin();
  Serial.println("Local Web Server Initialized!");
}