#include <Wire.h>
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads;

const int jumlahSampel = 1000;

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

  Serial.println("=== MODE KALIBRASI pH ===");

}

void loop() {
  float voltage = readAverageVoltage();

  Serial.print("Voltage : ");
  Serial.print(voltage, 4);
  Serial.println(" V");
  
  delay(100);
}