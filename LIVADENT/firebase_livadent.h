#pragma once
 
#define ENABLE_USER_AUTH
#define ENABLE_DATABASE
 
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>
#include <ArduinoJson.h>

#include "firebase_config.h"
#include "local_server.h"
#include "wifi_livadent.h"
 
extern FirebaseApp      firebaseApp;
extern AsyncClientClass async_client;
extern RealtimeDatabase database;
 
extern void database_init();
extern void processData(AsyncResult &aResult);
extern void set_device_active();