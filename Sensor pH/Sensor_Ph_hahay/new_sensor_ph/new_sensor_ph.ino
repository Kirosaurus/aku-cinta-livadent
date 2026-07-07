#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <TFT_eSPI.h>

Adafruit_ADS1115 ads;

const int jumlahSampel = 1;

float pH7 = 6.86;
// float v7 = 2.5961;
float v7 = 3.4675;
float pH4 = 4.01;
float v4 = 3.4770;

TFT_eSPI tft = TFT_eSPI();

float voltageToPH(float voltage) {
  return pH7 + ((voltage - v7) * (pH4 - pH7) / (v4 - v7));
}

float readAverageVoltage() {
  int32_t adcTotal = 0;

  for (int i = 0; i < jumlahSampel; i++) {
    adcTotal += ads.readADC_SingleEnded(0);
    delay(5);
  }

  float adcAvg = (float)adcTotal / jumlahSampel;

  // Untuk/ DS1115 GAIN_ONE:
  // 1 bit = 0.125 mV = 0.000125 V
  float voltage = adcAvg * 0.000125;

  return voltage;
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  tft.begin();
  tft.setRotation(1);
  tft.setTextSize(2);

  if (!ads.begin()) {
    Serial.println("ADS1115 gagal terdeteksi.");
    while (1);
  }

  ads.setGain(GAIN_ONE);

  Serial.println("=== MODE KALIBRASI pH 7 ===");
  Serial.println("Celupkan probe ke larutan buffer pH 7.00.");
  Serial.println("Tunggu sampai tegangannya stabil.");
}

void loop() {
  float voltage = readAverageVoltage();
  float pH = voltageToPH(voltage);

  Serial.print("Voltage rata-rata saat ini: ");
  Serial.print(voltage, 4);
  Serial.println(" V");
  Serial.print("pH nya : ");
  Serial.println(pH, 2);

  tft.setCursor(0, 0);
  tft.fillScreen(ILI9341_BLUE);
  tft.print("Voltage rata-rata saat ini: ");
  tft.print(voltage, 4);
  tft.println(" V");
  tft.print("pH nya : ");
  tft.println(pH, 2);

  delay(100);
}