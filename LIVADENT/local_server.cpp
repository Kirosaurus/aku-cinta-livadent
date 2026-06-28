#include "Arduino.h"
#include "local_server.h"
#include "wifi_livadent.h"

//Konfigurasi Web Server & mDNS
const char* mdnsname = "esplivadent";
// char mdns_hostname[MDNN_NAME_BUFF_LEN] = 
WebServer server(8888);

void handle_root()
{
    String json = "{";
    json += "\"mac\":\"" + get_mac() + "\",";
    json += "\"webConnect\":false,";
    json += "\"online\":true,";
    json += "\"local_ip\":\"" + WiFi.localIP().toString() + "\"";
    json += "}";

    send_cors_headers();
    server.send(200, "application/json", json);
}

void send_cors_headers()
{
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    // server.sendHeader("Access-Control-Allow-Headers", "Content-Type, X-LivaDent-Token");
}

void local_server_init()
{
    // if (!MDNS.begin(mdnsname))
    // {
    //     while (1)
    //     {
    //         Serial.println("Failed to Start MDNS! Please Reset...");
    //     }
    // }

    // MDNS.addService("http", "tcp", 1111);

    server.on("/", HTTP_GET, handle_root);
    server.on("/status", HTTP_GET, handle_root);

    server.begin();
    Serial.println("Local Web Server Initialized!");
}