#include "esp32-hal.h"
#include "Arduino.h"
#include "local_server.h"
#include "wifi_livadent.h"

const char* CONFIG_PATH = "/web_app_config.json";


//Konfigurasi Web Server & mDNS
const char* mdnsname = "esplivadent";
// char mdns_hostname[MDNN_NAME_BUFF_LEN] = 
WebServer server(8888);

uint32_t last_milli = 0;

void init_json(){
    // String json = "{";
    // json += "\"mac\":\"" + get_mac() + "\",";
    // json += "\"webConnect\":false,";
    // json += "\"online\":true,";
    // json += "\"local_ip\":\"" + WiFi.localIP().toString() + "\"";
    // json += "}";
    JsonDocument doc;
    doc["mac"] = WiFi.macAddress();
    doc["local_ip"] = WiFi.localIP().toString();
    doc["is_ph"] = true; // Ini nanti diganti sesuai dengan kondisi sensor
    doc["is_visc"] = true; // Ini juga
    doc["on_going"] = false;
    doc["web_app"] = false;

    File file = LittleFS.open(CONFIG_PATH, "w");
    serializeJsonPretty(doc, file);
    file.close();
}

void get_json()
{
    send_cors_headers();
    File file = LittleFS.open(CONFIG_PATH, "r");
    String json = file.readString();
    file.close();
    server.send(200, "application/json", json);
}

void update_json(){
    
}

void web_app_connection(){
    last_milli = millis();
    server.send(200, "application/interval", "{\"info\" : \"Interval Updated!\"");
}

void send_cors_headers()
{
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    // server.sendHeader("Access-Control-Allow-Headers", "Content-Type, X-LivaDent-Token");
}

void local_server_init()
{
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
    server.on("/interval", HTTP_GET, web_app_connection);
    server.begin();
    Serial.println("Local Web Server Initialized!");
}