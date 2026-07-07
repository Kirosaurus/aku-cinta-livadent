const int samples = 100;

#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(1);
  tft.setTextSize(2);
}

void loop() {
  float totpH = 0;
  for (int i = 0; i < samples; i++) {
    float volt = 3.3 / 4095 * analogRead(34);
    float pH = -28.5 * volt + 28.235;
    totpH += pH;
  }

  Serial.print("Volt : ");
  Serial.println(3.3 / 4095 * analogRead(34));
  Serial.print("pH : ");
  Serial.println(totpH / samples);

  tft.setCursor(0,0);
  tft.fillScreen(ILI9341_BLUE);
  tft.print("Volt : ");
  tft.println(3.3 / 4095 * analogRead(34));
  tft.print("pH : ");
  tft.println(totpH / samples);
  delay(100);
}
