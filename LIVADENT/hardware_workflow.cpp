#include "WiFiType.h"
#include "hardware_workflow.h"
#include "wifi_livadent.h"

const double v7 = 1.8;
const double v4 = 2.1;
const double p7 = 6.86;
const double p4 = 4.01;

const uint16_t samples = 100;
uint32_t last_progress = 0;

const float slope = (p7 - p4) / (v7 - v4);
const float offset = p4 - (slope * v4);

Adafruit_ADS1115 ads;

int read_adc_median(int channel) {
  int adc[samples];

  for (int i = 0; i < samples; i++) {
    adc[i] = ads.readADC_SingleEnded(channel);
  }

  for (int i = 0; i < samples - 1; i++) {
    for (int j = i + 1; j < samples; j++) {
      if (adc[i] > adc[j]) {
        int temp = adc[i];
        adc[i] = adc[j];
        adc[j] = temp;
      }
    }
  }

  return adc[samples / 2];
}

float get_digit(float v) {
  // float decimal = 0.01;
  // float temp = v%decimal;
  // return v - temp;

  return round(v * 1000.0) / 1000.0;
}

void get_ph() {
  last_progress = millis();
  int adc1[samples];
  int i = 0;
  Serial.println("Masuk");
  while (millis() - last_progress < SCAN_TIMEOUT) {
    adc1[i % samples] = read_adc_median(1);
    i++;
    Serial.println("Skrining...");
  }

  Serial.println("Selesai Scan ADC");

  int total_adc1 = 0;
  for (int index = 0; index < samples; index++) {
    total_adc1 += adc1[index];
  }
  Serial.println("Dapet Volt");

  int volt = ads.computeVolts(total_adc1 / samples);

  ph_sensor = -10.94 * get_digit(volt) + 45.22;
  Serial.println("Dapet Nilai pH");
}

void failed_handler_timeout(lv_timer_t* time) {


  lv_timer_del(time);
}


void process(String schedule_key, String student_key, String screening_key) {
  // Gerakin sensor ph
  // get_ph();
  // Balikin sensor ph

  // Gerakin sensor viskositas
  // int adc0 = read_adc_median(0);
  // visc_sensor = ads.computeVolts(adc0);  // INI CUMA PLACEHOLDER
  // Balikin sensor viskositas


  /////////////////////////////////////////////// HARDCODE NILAI SENSOR PH DAN VISKOSITAS
  ph_sensor = PH_INPUT_DUMMY;
  visc_sensor = VISKOSITAS_INPUT_DUMMY;
  ///////////////////////////////////////////////

  // Validasi data skrining wajar atau egak
  if (ph_sensor <= 0 || ph_sensor > 14) {
    lv_obj_remove_flag(objects.screening_failed, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.screening_on_progress, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.start_stop_screening_btn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(objects.back_in_second, LV_OBJ_FLAG_HIDDEN);

    // uint16_t failed_timeout = millis();
    // char time_second[6];

    // while(millis() - failed_timeout <= FAILED_TIMEOUT){
    //   snprintf(time_second, sizeof(time_second), "%d", millis() - failed_timeout);
    //   lv_label_set_text(objects.failed_timeout_second, time_second);
    //   lv_timer_handler();
    //   delay(1);
    // }

    // lv_timer_create(failed_handler_timeout, FAILED_TIMEOUT, NULL);
    File open_file = LittleFS.open(CONFIG_PATH, "r");
    JsonDocument doc;
    deserializeJson(doc, open_file);
    open_file.close();
    doc["ready"] = true;
    doc["on_going"] = false;
    File write_file = LittleFS.open(CONFIG_PATH, "w");
    serializeJsonPretty(doc, write_file);
    write_file.close();
  }

  if (WiFi.status() == WL_CONNECTED) {
    JsonDocument ret_val;
    ret_val["mode"] = "online";
    ret_val["ph"] = ph_sensor;
    ret_val["viscosity"] = visc_sensor;
    ret_val["risk"] = -1;
    ret_val["schedule_id"] = schedule_key;
    ret_val["student_id"] = student_key;

    String database_up;
    serializeJson(ret_val, database_up);
    Serial.print("Firebase is Active? = ");
    Serial.println(firebaseApp.ready());
    Serial.println(database_up);
    if (firebaseApp.ready()) {
      Serial.println("Send pH & Viscosity Value to Database...");
      database.set<String>(async_client, "screenings/" + screening_key + "/mode", "online");
      database.set<float>(async_client, "screenings/" + screening_key + "/ph", ph_sensor);
      database.set<float>(async_client, "screenings/" + screening_key + "/viscosity", visc_sensor);
      database.set<int>(async_client, "screenings/" + screening_key + "/risk", -1);
      database.set<String>(async_client, "screenings/" + screening_key + "/schedule_id", schedule_key);
      database.set<String>(async_client, "screenings/" + screening_key + "/student_id", student_key);

      Serial.println(screening_key);

      uint32_t poll_start = millis();
      const uint32_t POLL_TIMEOUT_MS = 30000;  

      do {
        mlp_risk_label = database.get<int>(async_client, "screenings/" + screening_key + "/risk");
        Serial.print("Waiting Web App Classification... ");
        Serial.println(mlp_risk_label);

        if (millis() - poll_start > POLL_TIMEOUT_MS) {
          Serial.println("Timeout menunggu klasifikasi dari web app!");
          mlp_risk_label = 3;  // fallback state kalau timeout
          break;
        }
        delay(500); 
      } while (mlp_risk_label == 255);
    }

  } else {
    mlp_risk_label = predict_test(ph_sensor, visc_sensor);
  }

  lv_scr_load_anim(objects.result, LV_SCR_LOAD_ANIM_FADE_IN, 0, 200, false);
}
