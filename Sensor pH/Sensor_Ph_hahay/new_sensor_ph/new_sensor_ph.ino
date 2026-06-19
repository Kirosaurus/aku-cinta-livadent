#include <Wire.h>
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads;

const int jumlahSampel = 100;

float pH7 = 6.86;
float v7 = 2.5961;
float pH4 = 4.01;
float v4 = 2.8569;


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

  // Untuk ADS1115 GAIN_ONE:
  // 1 bit = 0.125 mV = 0.000125 V
  float voltage = adcAvg * 0.000125;

  return voltage;
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

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


  delay(100);
}