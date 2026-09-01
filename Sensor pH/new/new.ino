#include <AccelStepper.h>
#include <TMCStepper.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();
volatile float viskositas_cP = 0.0;
volatile float finalVoltage = 0.0;
volatile float finalAdcRaw = 0.0;

Adafruit_ADS1115 ads;

// Definisi Pin TMC2209
const int STEP_PIN = 12;
const int DIR_PIN = 13;
const int EN_PIN = 26;

// Definisi Pin Hardware Serial2
const int RX_PIN = 17;
const int TX_PIN = 16;

// Definisi Touch Pin Reset (GPIO 4 = Touch0)
const int TOUCH_RESET_PIN = 4;
const int TOUCH_THRESHOLD = 200;

#define R_SENSE 0.11f
#define DRIVER_ADDRESS 0b00

TMC2209Stepper driver(&Serial2, R_SENSE, DRIVER_ADDRESS);
AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

// Set kecepatan target
const float TARGET_SPEED = 3000.0;
const float ACCEL_RATE = 700.0;

// --- PARAMETER KALIBRASI VISKOSITAS (cP) ---
const float ADC_BASELINE_AIR = 21300.0;
const float FAKTOR_SLOPE_M = 0.0125;
const float OFFSET_C = 0.0;

TaskHandle_t TaskSerialHandle;
volatile bool triggerReset = false;

void IRAM_ATTR gotTouched() {
  triggerReset = true;  // Perbaikan: ubah ke true agar memicu restart
}

float hitungCentipoise(float rawAdc) {
  // float deltaAdc = rawAdc - ADC_BASELINE_AIR;
  // if (deltaAdc < 0) deltaAdc = 0;
  // return (FAKTOR_SLOPE_M * deltaAdc) + OFFSET_C;

  float visc_ret_val = rawAdc * 0.023776224 -496.034965;
  return visc_ret_val;
}

void TaskSerial(void* pvParameters) {
  for (;;) {
    // 0. Cek Trigger Reset
    if (triggerReset) {
      Serial.println("\n[RESET] Touch terdeteksi! Mereset ESP32...");
      // tft.fillScreen(TFT_RED);
      // tft.setCursor(20, 50);
      // tft.setTextColor(TFT_WHITE, TFT_RED);
      // tft.println("RESETTING...");
      vTaskDelay(100 / portTICK_PERIOD_MS);
      ESP.restart();
    }

    int32_t activeSum = 0;
    int validCount = 0;

    // 1. Sampling cepat
    for (int i = 0; i < 20; i++) {
      int16_t val = ads.readADC_SingleEnded(0);
      if (val > 4000) {
        activeSum += val;
        validCount++;
      }
      vTaskDelay(2 / portTICK_PERIOD_MS);
    }

    // 2. Tampilkan hasil jika ada sinyal valid
    if (validCount > 0) {
      finalAdcRaw = (float)activeSum / validCount;
      finalVoltage = ads.computeVolts((int16_t)finalAdcRaw);
      viskositas_cP = hitungCentipoise(finalAdcRaw);

      // Print ke Serial Monitor
      Serial.print("[Sensor] ADC: ");
      Serial.print(finalAdcRaw, 1);
      Serial.print(" | Volt: ");
      Serial.print(finalVoltage, 4);
      Serial.print(" V | Viskositas: ");
      Serial.print(viskositas_cP, 2);
      Serial.print("Viskositas: ");
      Serial.print(validCount);
      Serial.print(" | Touch: ");
      Serial.println(touchRead(TOUCH_RESET_PIN));
      // tft.fillScreen(TFT_BLACK);
      // tft.setCursor(0, 0);
      // tft.print("Viskositas: ");
      // tft.print(viskositas_cP, 2);
      // tft.println(" cP");
    }

    vTaskDelay(150 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;

  Serial2.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);

  Wire.begin(21, 22);
  if (ads.begin(0x48)) {
    ads.setGain(GAIN_ONE);
    Serial.println("[OK] ADS1115 Terhubung!");
  } else {
    Serial.println("[ERROR] ADS1115 Tidak Ditemukan!");
  }

  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW);

  driver.begin();
  uint8_t result = driver.test_connection();
  if (result == 0) {
    Serial.println("[OK] TMC2209 UART Terhubung!");
  } else {
    Serial.print("[ERROR] TMC2209 UART Gagal: ");
    Serial.println(result);
  }

  driver.toff(5);
  driver.rms_current(400);
  driver.microsteps(8);
  driver.en_spreadCycle(false);
  driver.pwm_autoscale(true);

  stepper.setMaxSpeed(TARGET_SPEED);
  stepper.setAcceleration(ACCEL_RATE);
  stepper.moveTo(20000000);

  touchAttachInterrupt(TOUCH_RESET_PIN, gotTouched, TOUCH_THRESHOLD);

  // Inisialisasi TFT LCD
  // tft.begin();
  // tft.setRotation(3);
  // tft.fillScreen(TFT_BLACK);
  // tft.setTextColor(TFT_WHITE);
  // tft.setTextSize(2);

  // tft.setCursor(20, 20);
  // tft.println("TFT OK");

  // tft.setCursor(20, 50);
  // tft.println("ESP32 START");

  // Jalankan Task LCD & Sensor di Core 0
  xTaskCreatePinnedToCore(
    TaskSerial,
    "TaskSerial",
    4096,
    NULL,
    1,
    &TaskSerialHandle,
    0);
}

void loop() {
  Serial.println("huihi");
  stepper.run();
}