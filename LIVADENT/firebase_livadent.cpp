#include "firebase_livadent.h"

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASS);
FirebaseApp firebaseApp;
WiFiClientSecure ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient async_client(ssl_client);

RealtimeDatabase database;
AsyncResult dbResult;

void database_init() {
  ssl_client.setInsecure();  // testing saja

  initializeApp(async_client, firebaseApp, getAuth(user_auth), processData, "authTask");

  firebaseApp.getApp<RealtimeDatabase>(database);
  database.url(DATABASE_URL);
}

void set_device_active() {
  database.set<String>(async_client,
                       "saved_devices/" + WiFi.macAddress() + "/local_ip",
                       WiFi.localIP().toString());
}

void processData(AsyncResult &aResult) {
  if (!aResult.isResult()) {
    return;
  }

  if (aResult.isEvent()) {
    Serial.print("Event: ");
    Serial.println(aResult.eventLog().message());
  }

  if (aResult.isDebug()) {
    Serial.print("Debug: ");
    Serial.println(aResult.debug());
  }

  if (aResult.isError()) {
    Serial.print("Error: ");
    Serial.println(aResult.error().message());
  }

  if (aResult.available()) {
    if (aResult.eventLog().message() == "auth response received" || aResult.eventLog().message() == "ready") {
      return;
    }

    JsonDocument doc;
    do {
      deserializeJson(doc, aResult.c_str());

      Serial.print("pH : ");
      Serial.println(String(doc["ph"]));
      Serial.print("Viskositas : ");
      Serial.print(String(doc["viscosity"]));
      Serial.println(" cP");
      Serial.println(String(doc["risk"]));

    //   const char *risiko_labels[] = { "Rendah", "Sedang", "Tinggi" };
    //   int risiko = doc["risiko"] | -1;
    //   if (risiko >= 0 && risiko <= 2) {
    //     Serial.print("Risiko : ");
    //     Serial.println(risiko_labels[risiko]);
    //   }

      Serial.print("Payload: ");
      Serial.println(aResult.c_str());
    } while (doc["risk"] == -1);
  }
}