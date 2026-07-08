const int samples = 1;

#include <TFT_eSPI.h>

#define SECOND_OFFSET 30000

const double v7 = 0.85;
const double v4  = 0.94;
const double p7 = 6.86;
const double p4 = 4.01;

const float slope = (p7-p4) / (v7-v4);
const float offset = p4 - (slope*v4);

TFT_eSPI tft = TFT_eSPI();

// void get_ph() {
//   float totpH = 0;
//   long long i = 0;
//   while (millis() <= SECOND_OFFSET) {
//     Serial.println(".");
//     float volt = 3.3 / 4095 * analogRead(34);
//     float pH = -57 * volt + 56.45;
//     totpH += pH;
//     i++;
//   }


//   Serial.print("Volt : ");
//   Serial.println(3.3 / 4095 * analogRead(34));
//   Serial.print("pH : ");
//   Serial.println(totpH / i);

//   tft.setCursor(0, 0);
//   tft.fillScreen(ILI9341_BLUE);
//   tft.print("Volt : ");
//   tft.println(3.3 / 4095 * analogRead(34));
//   tft.print("pH : ");
//   tft.println(totpH / i);
// }

void setup() {
  Serial.begin(115200);
  pinMode(34, INPUT);
  tft.begin();
  tft.setRotation(1);
  tft.setTextSize(2);

  // get_ph();
}

void loop() {
  while (millis() <= SECOND_OFFSET) {
    double volt = 0.00;
    double totpH = 0.00;
    double pH = 0.00;
    for (int i = 0; i < samples; i++) {
      volt = 3.3 / 4095 * analogRead(34);
      // pH = -35.625 * volt + 37.85375;
      pH = (slope * volt + offset);
      totpH += pH;
      // i++;
    }

    Serial.print("Volt : ");
    Serial.println(volt);
    Serial.print("pH : ");
    Serial.println(totpH / samples);

    tft.setCursor(0, 0);
    tft.fillScreen(ILI9341_BLUE);
    tft.print("Volt : ");
    tft.println(volt);
    tft.print("pH : ");
    tft.println(totpH / samples);
    delay(100);
  }
}
