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


// void process(String schedule_key, String student_key, String screening_key) {
//   // Gerakin sensor ph
//   // get_ph();
//   // Balikin sensor ph

//   // Gerakin sensor viskositas
//   // int adc0 = read_adc_median(0);
//   // visc_sensor = ads.computeVolts(adc0);  // INI CUMA PLACEHOLDER
//   // Balikin sensor viskositas


//   /////////////////////////////////////////////// HARDCODE NILAI SENSOR PH DAN VISKOSITAS
//   ph_sensor = PH_INPUT_DUMMY;
//   visc_sensor = VISKOSITAS_INPUT_DUMMY;
//   ///////////////////////////////////////////////

//   // Validasi data skrining wajar atau egak
//   if (ph_sensor <= 0 || ph_sensor > 14) {
//     lv_obj_remove_flag(objects.screening_failed, LV_OBJ_FLAG_HIDDEN);
//     lv_obj_add_flag(objects.screening_on_progress, LV_OBJ_FLAG_HIDDEN);
//     lv_obj_add_flag(objects.start_stop_screening_btn, LV_OBJ_FLAG_HIDDEN);
//     lv_obj_remove_flag(objects.back_in_second, LV_OBJ_FLAG_HIDDEN);

//     // uint16_t failed_timeout = millis();
//     // char time_second[6];

//     // while(millis() - failed_timeout <= FAILED_TIMEOUT){
//     //   snprintf(time_second, sizeof(time_second), "%d", millis() - failed_timeout);
//     //   lv_label_set_text(objects.failed_timeout_second, time_second);
//     //   lv_timer_handler();
//     //   delay(1);
//     // }

//     // lv_timer_create(failed_handler_timeout, FAILED_TIMEOUT, NULL);
//     global_variable["ready"] = true;
//     global_variable["on_going"] = false;
//   }

//   if (WiFi.status() == WL_CONNECTED) {
//     JsonDocument ret_val;
//     ret_val["mode"] = "online";
//     ret_val["ph"] = ph_sensor;
//     ret_val["viscosity"] = visc_sensor;
//     ret_val["risk"] = -1;
//     ret_val["schedule_id"] = schedule_key;
//     ret_val["student_id"] = student_key;

//     String database_up;
//     serializeJson(ret_val, database_up);
// #ifdef DEBUG
//     Serial.print("Firebase is Active? = ");
//     Serial.println(firebaseApp.ready());
//     Serial.println(database_up);
// #endif
//     if (firebaseApp.ready()) {
// #ifdef DEBUG
//       Serial.println("Send pH & Viscosity Value to Database...");
// #endif
//       database.set<String>(async_client, "screenings/" + screening_key + "/mode", "online");
//       database.set<float>(async_client, "screenings/" + screening_key + "/ph", ph_sensor);
//       database.set<float>(async_client, "screenings/" + screening_key + "/viscosity", visc_sensor);
//       database.set<int>(async_client, "screenings/" + screening_key + "/risk", -1);
//       database.set<String>(async_client, "screenings/" + screening_key + "/schedule_id", schedule_key);
//       database.set<String>(async_client, "screenings/" + screening_key + "/student_id", student_key);

// #ifdef DEBUG
//       Serial.println(screening_key);
// #endif

//       uint32_t poll_start = millis();
//       const uint32_t POLL_TIMEOUT_MS = 30000;

//       do {
//         mlp_risk_label = database.get<int>(async_client, "screenings/" + screening_key + "/risk");
// #ifdef DEBUG
//         Serial.print("Waiting Web App Classification... ");
//         Serial.println(mlp_risk_label);
// #endif

//         if (millis() - poll_start > POLL_TIMEOUT_MS) {
//           Serial.println("Timeout menunggu klasifikasi dari web app!");
//           mlp_risk_label = 3;
//           break;
//         }
//         delay(500);
//       } while (mlp_risk_label == 255);
//     }

//   } else {
//     mlp_risk_label = predict_test(ph_sensor, visc_sensor);
//   }

//   lv_scr_load_anim(objects.result, LV_SCR_LOAD_ANIM_FADE_IN, 0, 200, false);
// }

const uint32_t POLL_INTERVAL_MS = 500;
const uint32_t POLL_TIMEOUT_MS = 30000;
uint32_t last_millis_scan = 0;

void start_screening_process(String schedule_key, String student_key, String screening_key) {
  screening_schedule_key = schedule_key;
  screening_student_key = student_key;
  screening_key_active = screening_key;
  screening_state = SCREENING_SENSOR_READ;

  if (screening_timer != NULL) lv_timer_del(screening_timer);
  last_millis_scan = millis();
  lv_scr_load(objects.screening);
  screening_timer = lv_timer_create(screening_process_tick, 1000, NULL);
}

void stop_screening_process() {
  if (screening_timer != NULL) {
    lv_timer_del(screening_timer);
    screening_timer = NULL;
  }
  screening_state = SCREENING_IDLE;
  global_variable["on_going"] = false;
  global_variable["ready"] = true;
  global_variable["stop"] = false;

  lv_scr_load_anim(objects.main_menu, LV_SCR_LOAD_ANIM_FADE_IN, 0, 200, false);
}

void screening_process_tick(lv_timer_t* timer) {
  if (global_variable["stop"] == true) {
    Serial.println("Skrining dihentikan dari web app");
    stop_screening_process();
    return;
  }

  switch (screening_state) {
    case SCREENING_SENSOR_READ:
      {
        ph_sensor = PH_INPUT_DUMMY;
        visc_sensor = VISKOSITAS_INPUT_DUMMY;

        if (ph_sensor <= 0 || ph_sensor > 14) {
          lv_obj_remove_flag(objects.screening_failed, LV_OBJ_FLAG_HIDDEN);
          lv_obj_add_flag(objects.screening_on_progress, LV_OBJ_FLAG_HIDDEN);
          lv_obj_add_flag(objects.start_stop_screening_btn, LV_OBJ_FLAG_HIDDEN);
          lv_obj_remove_flag(objects.back_in_second, LV_OBJ_FLAG_HIDDEN);

          global_variable["ready"] = true;
          global_variable["on_going"] = false;

          lv_timer_del(timer);
          screening_timer = NULL;
          screening_state = SCREENING_IDLE;
          return;
        }
        Serial.println("Scan Finished");

        // INI HANYA DUMMY PERCOBAAN BIAR AGAK LAMA WAKTU SCAN
        if (millis() - last_millis_scan > DUMMY_TIMEOUT_SCAN) {
          Serial.println("DONE!");
          screening_state = SCREENING_SEND_TO_FIREBASE;
        }
        break;
      }

    case SCREENING_SEND_TO_FIREBASE:
      {
        if (WiFi.status() == WL_CONNECTED && firebaseApp.ready()) {
          database.set<String>(async_client, "screenings/" + screening_key_active + "/mode", "online");
          database.set<float>(async_client, "screenings/" + screening_key_active + "/ph", ph_sensor);
          database.set<float>(async_client, "screenings/" + screening_key_active + "/viscosity", visc_sensor);
          database.set<int>(async_client, "screenings/" + screening_key_active + "/risk", -1);
          database.set<String>(async_client, "screenings/" + screening_key_active + "/schedule_id", screening_schedule_key);
          database.set<String>(async_client, "screenings/" + screening_key_active + "/student_id", screening_student_key);

          screening_poll_start_ms = millis();
          screening_state = SCREENING_WAIT_CLASSIFICATION;
        } else {
          mlp_risk_label = predict_test(ph_sensor, visc_sensor);
          screening_state = SCREENING_DONE;
        }
        break;
      }

    case SCREENING_WAIT_CLASSIFICATION:
      {
        static uint32_t last_poll = 0;
        if (millis() - last_poll >= POLL_INTERVAL_MS) {
          last_poll = millis();
          mlp_risk_label = database.get<int>(async_client, "screenings/" + screening_key_active + "/risk");
        }

        if (mlp_risk_label != 255) {
          screening_state = SCREENING_DONE;
        } else if (millis() - screening_poll_start_ms > POLL_TIMEOUT_MS) {
          Serial.println("Timeout menunggu klasifikasi dari web app!");
          mlp_risk_label = 3;
          screening_state = SCREENING_DONE;
        }
        break;
      }

    case SCREENING_DONE:
      {
        lv_timer_del(timer);
        screening_timer = NULL;
        screening_state = SCREENING_IDLE;
        lv_scr_load_anim(objects.result, LV_SCR_LOAD_ANIM_FADE_IN, 0, 200, false);
        break;
      }

    default: break;
  }
}
