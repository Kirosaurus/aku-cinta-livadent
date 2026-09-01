#include "WiFiType.h"
#include "hardware_workflow.h"
#include "wifi_livadent.h"

float ph_dummy[4] = { 6.12, 7.21, 6.73, 6.48 };
float visc_dummy[4] = { 0.97, 1.38, 1.74, 2.38 };

const uint16_t samples = 100;
uint32_t last_progress = 0;

TaskHandle_t TaskMotor;
QueueHandle_t xQueue;

bool queued = false;
volatile bool abort_motor = false;

// All motor
TMC2209Stepper driver(&SERIAL_PORT, R_SENSE, DRIVER_ADDRESS);
AccelStepper stepperX(AccelStepper::DRIVER, STEP_PIN_X, DIR_PIN_X);
AccelStepper stepperY(AccelStepper::DRIVER, STEP_PIN_Y, DIR_PIN_Y);
AccelStepper stepperVisc(AccelStepper::DRIVER, STEP_VISC_PIN, DIR_VISC_PIN);

Adafruit_ADS1115 ads;

// STEPPER MOVE MOTOR CONFIGURATION
const float MOTOR_SPEED = 5000.0;
float DISTANCE_TO_GO = -20000;

// Visc Move Timeout
uint32_t visc_timeout = 0;
bool is_visc_active = false;

void task_visc_motor(void* pvParameters) {
  while (true) {
    if (visc_timeout > SCAN_TIMEOUT) {
      if (stepperVisc.distanceToGo() == 0) {
        stepperVisc.moveTo(stepperVisc.currentPosition() + 200000000);
      }
      stepperVisc.run();
    }
    taskYIELD();
  }
}

void motor_task(void* pv) {
  motor motor_cmd;
  while (true) {
    if (xQueueReceive(xQueue, &motor_cmd, portMAX_DELAY) == pdTRUE) {
      motor_cmd.motor->setMaxSpeed(MOTOR_SPEED);
      motor_cmd.motor->move(motor_cmd.distance);
      motor_cmd.motor->setSpeed(motor_cmd.distance > 0 ? MOTOR_SPEED : -MOTOR_SPEED);

      bool aborted = false;
      while (motor_cmd.motor->distanceToGo() != 0) {
        if (abort_motor) {
          motor_cmd.motor->move(0);
          aborted = true;
          break;
        }
        motor_cmd.motor->runSpeed();
        taskYIELD();
        // vTaskDelay(0.5);
      }

      queued = false;

      if (aborted) {
        Serial.println("Motor diabort (stop diminta) - tidak lanjut ke next_state");
        screening_state = SCREENING_STOPPING;
      } else {
        Serial.println("Motor Stoped");
        screening_state = motor_cmd.next_state;
      }
    }
  }
}

void create_queue_motor(AccelStepper* motor_identifier, long distance, screening_state_t next_state) {
  motor motor_cmd = { motor_identifier, distance, next_state };
  queued = true;
  xQueueSend(xQueue, &motor_cmd, 0);
}

bool init_ph() {
  if (!is_ads) {
    return false;
  }
  return true;
}

bool init_visc() {
  if (!is_ads) {
    return false;
  }

  return true;
}

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
  while (millis() - last_progress < SCAN_TIMEOUT) {
    adc1[i % samples] = read_adc_median(1);
    i++;
  }

  Serial.println("Selesai Scan ADC");

  int total_adc1 = 0;
  for (int index = 0; index < samples; index++) {
    total_adc1 += adc1[index];
  }
  Serial.println("Dapet Volt");

  int volt = ads.computeVolts(total_adc1 / samples);

  ph_sensor = -10.94 * get_digit(volt) + 45.22;  // Rumus dapetin pH
  Serial.println("Dapet Nilai pH");
}

void failed_handler_timeout(lv_timer_t* time) {

  lv_timer_del(time);
}

const uint32_t POLL_INTERVAL_MS = 500;
const uint32_t POLL_TIMEOUT_MS = 60000;
uint32_t last_millis_scan = 0;

void start_screening_process(String schedule_key, String student_key, String screening_key) {
  screening_schedule_key = schedule_key;
  screening_student_key = student_key;
  screening_key_active = screening_key;
  screening_state = SCREENING_SENSOR_MOVE_Y_PH;

  // is_visc_active = false;
  global_variable["stop"] = false;

  if (screening_timer != NULL) lv_timer_del(screening_timer);
  last_millis_scan = millis();
  lv_scr_load(objects.screening);
  screening_timer = lv_timer_create(screening_process_tick, 1000, NULL);
}

bool first_pos = true;
bool stopping_in_progress = false;

void stop_screening_process() {
  if (screening_timer != NULL) {
    lv_timer_del(screening_timer);
    screening_timer = NULL;
  }
  screening_state = SCREENING_IDLE;
  global_variable["on_going"] = false;
  global_variable["ready"] = true;
  global_variable["stop"] = false;
  first_pos = true;
  stopping_in_progress = false;

  lv_scr_load_anim(objects.main_menu, LV_SCR_LOAD_ANIM_FADE_IN, 0, 200, false);
}

void screening_process_tick(lv_timer_t* timer) {
  if (global_variable["stop"] == true && !stopping_in_progress) {
    Serial.println("Skrining dihentikan dari web app");
    if (!first_pos) {
      stopping_in_progress = true;

      if (queued) {
        abort_motor = true;
        return;
      } else {
        screening_state = SCREENING_STOPPING;
      }
    } else {
      stop_screening_process();
      return;
    }
  }

  switch (screening_state) {
    case SCREENING_SENSOR_MOVE_X_PH:
      {
        if (!queued) {
          create_queue_motor(&stepperX, 20000, SCREENING_SENSOR_MOVE_Y_PH);
        }
        break;
      }
      break;
    case SCREENING_SENSOR_MOVE_Y_PH:
      {
        if (!queued) {
          create_queue_motor(&stepperY, -10000, SCREENING_SENSOR_READ_PH);
          first_pos = false;
        }
        break;
      }
    case SCREENING_SENSOR_READ_PH:
      {

        ph_sensor = ph_dummy[esp_random() % 4];
        Serial.print("Got pH Sensor : ");
        Serial.println(ph_sensor);
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
        // get_ph();
        Serial.println("Scanning pH...");

        // INI HANYA DUMMY PERCOBAAN BIAR AGAK LAMA WAKTU SCAN
        if (millis() - last_millis_scan > DUMMY_TIMEOUT_SCAN) {
          Serial.println("DONE!");
          screening_state = SCREENING_SENSOR_MOVE_Y_RETURN_PH;
        }
        break;
      }

    case SCREENING_SENSOR_MOVE_Y_RETURN_PH:
      {
        if (!queued) {
          create_queue_motor(&stepperY, 10000, SCREENING_SENSOR_MOVE_X_VISC);
        }
        break;
      }

    case SCREENING_SENSOR_MOVE_X_VISC:
      {
        if (!queued) {
          create_queue_motor(&stepperX, 22000, SCREENING_SENSOR_MOVE_Y_VISC);
        }
        break;
      }


    case SCREENING_SENSOR_MOVE_Y_VISC:
      {
        if (!queued) {
          create_queue_motor(&stepperY, -10000, SCREENING_SENSOR_READ_VISC);
        }
        break;
      }

    case SCREENING_SENSOR_READ_VISC:
      {
        visc_sensor = visc_dummy[esp_random() % 4];
        Serial.print("Got Viscosity Sensor : ");
        Serial.println(visc_sensor);
        if (visc_sensor <= 0) {
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

        if (!is_visc_active) {
          xTaskCreatePinnedToCore(task_visc_motor, "Run Viscometer Motor", 10000, NULL, 1, &TaskMotor, 0);
          is_visc_active = true;
        }

        if (millis() - last_millis_scan > DUMMY_TIMEOUT_SCAN) {
          Serial.println("DONE!");
          screening_state = SCREENING_SENSOR_MOVE_Y_FINISH;
        }
        break;
      }

    case SCREENING_SENSOR_MOVE_Y_FINISH:
      {
        if (!queued) {
          create_queue_motor(&stepperY, 10000, SCREENING_SENSOR_MOVE_X_FINISH);
        }
        break;
      }

    case SCREENING_SENSOR_MOVE_X_FINISH:
      {
        if (!queued) {
          create_queue_motor(&stepperX, -22000, SCREENING_SEND_TO_FIREBASE);
        }
        break;
      }

    case SCREENING_SEND_TO_FIREBASE:
      {
        first_pos = true;
        if (global_variable["stop"] == true) {
          stopping_in_progress = false;
          screening_state = SCREENING_IDLE;
          global_variable["on_going"] = false;
          global_variable["ready"] = true;
          global_variable["stop"] = false;
          lv_scr_load_anim(objects.main_menu, LV_SCR_LOAD_ANIM_FADE_IN, 0, 200, false);
          break;
        }
        if (WiFi.status() == WL_CONNECTED && firebaseApp.ready()) {
          Serial.printf("Free heap sebelum Firebase write: %u\n", ESP.getFreeHeap());
          Serial.printf("Largest free block: %u\n", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
          // database.set<String>(async_client, "screenings/" + screening_key_active + "/mode", "online");
          database.set<float>(async_client, "screenings/" + screening_key_active + "/ph", ph_sensor);
          database.set<float>(async_client, "screenings/" + screening_key_active + "/viscosity", visc_sensor);
          // database.set<int>(async_client, "screenings/" + screening_key_active + "/risk", -1);
          // database.set<String>(async_client, "screenings/" + screening_key_active + "/schedule_id", screening_schedule_key);
          // database.set<String>(async_client, "screenings/" + screening_key_active + "/student_id", screening_student_key);
          Serial.printf("Free heap sesudah Firebase write: %u\n", ESP.getFreeHeap());

          screening_poll_start_ms = millis();
          screening_state = SCREENING_WAIT_CLASSIFICATION;
        } else {
          mlp_risk_label = predict_test(ph_sensor, visc_sensor);
          screening_state = SCREENING_WAIT_CLASSIFICATION;
        }
        // lv_scr_load_anim(objects.result, LV_SCR_LOAD_ANIM_FADE_IN, 0, 200, false);
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

    case SCREENING_STOPPING:
      {
        abort_motor = false;

        if (!queued) {
          if (stepperY.currentPosition() != 0) {
            create_queue_motor(&stepperY, -stepperY.currentPosition(), SCREENING_STOPPING);
          } else if (stepperX.currentPosition() != 0) {
            create_queue_motor(&stepperX, -stepperX.currentPosition(), SCREENING_STOPPING);
          } else {
            stop_screening_process();
          }
        }
        break;
      }

    case SCREENING_DONE:
      {
        lv_timer_del(timer);
        screening_timer = NULL;
        screening_state = SCREENING_IDLE;
        global_variable["web_app"] = false;
        lv_scr_load_anim(objects.result, LV_SCR_LOAD_ANIM_FADE_IN, 0, 200, false);
        break;
      }

    default: break;
  }
}

bool main_menu_loaded_emer = true;

void check_emergency_btn() {
  if (digitalRead(EMERGENCY_BTN_PIN) != 0) {
    lv_scr_load(objects.emergency_stop);
    main_menu_loaded_emer = false;
    digitalWrite(EN_MOVE_PIN, HIGH);
  } else if (digitalRead(EMERGENCY_BTN_PIN) == 0 && !main_menu_loaded_emer) {
    lv_scr_load(objects.main_menu);
    digitalWrite(EN_MOVE_PIN, LOW);
    // stop_screening_process();
    main_menu_loaded_emer = true;
  }
}