#include "websockets.h"

WebSocketsServer webSocket(81);

char session_key[41];
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

          Serial.println(
            "Device already in use. Rejecting client.");

          webSocket.sendTXT(
            clientNum,
            "{\"info\":\"error\",\"reason\":\"device_in_use\"}");

          webSocket.disconnect(
            clientNum);

          break;
        }

        // Client pertama diterima
        clientConnected = true;
        activeClient = clientNum;

        Serial.printf(
          "Client %u accepted\n",
          clientNum);

        webSocket.sendTXT(
          clientNum,
          "{\"info\":\"connection_accepted\"}");

        break;
      }

    case WStype_DISCONNECTED:
      {
        Serial.printf(
          "Client %u disconnected\n",
          clientNum);

        if (
          clientConnected && clientNum == activeClient) {

          clientConnected = false;
          activeClient = 255;

          Serial.println(
            "ESP32 is now available");
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
        if (strcmp(web_app_request["info"], "PING") == 0) {
          Serial.println("PINGING");
          last_millis = millis();
          session_timeout = false;
          JsonDocument doc;
          doc["info"] = "PONG";
          String ret_val;

          serializeJson(doc, ret_val);
          Serial.print("PINGING DATA : "); serializeJson(doc, Serial);
          webSocket.sendTXT(
            clientNum,
            ret_val);
          break;
        } else if (strcmp(web_app_request["info"], "session_key") == 0) {
          Serial.println("Session Key Given!");
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
        } else if (strcmp(web_app_request["info"], "connect_init") == 0) {
          Serial.println(session_key);
          JsonDocument doc;
          doc["info"] = "Sensor Initialization";
          doc["is_ph"] = 1;
          doc["is_visc"] = 1;
          String ret_val;
          serializeJson(doc, ret_val);
          serializeJson(doc, Serial);
          webSocket.sendTXT(clientNum, ret_val);
          Serial.println("Send Init");
        }
        break;
      }
    default:
      break;
  }
}

void updateSessionKey() {
  if (millis() - last_millis > 20000 && !session_timeout) {
    Serial.println("No Respond from Web App! (Session Deleted)");
    session_timeout = true;
    strcpy(session_key, "");
  }
}