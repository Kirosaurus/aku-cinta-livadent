#include <Arduino.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

int pHSense = 34;
int samples = 5;
float adc_resolution = 4095.0;

void setup() {
  Serial.begin(9600);
  delay(100);
  tft.begin();
  tft.setRotation(1);
  tft.setTextSize(2);
  Serial.println("CimpleO pH Sense");
}

float ph(float voltage) {
  return 7 + ((2.5 - voltage) / 0.18);
}

void loop() {
  int measurings = 0;
  for (int i = 0; i < samples; i++) {
    measurings += analogRead(pHSense);
    delay(10);
  }
  float voltage = 5 / adc_resolution * measurings / samples;

  tft.setCursor(0, 0);
  tft.fillScreen(ILI9341_BLUE);

  Serial.print("pH= ");
  Serial.println(ph(voltage));
  tft.print("pH= ");
  tft.println(ph(voltage));
  tft.print("Voltage= ");
  tft.println(voltage);

  delay(3000);
}