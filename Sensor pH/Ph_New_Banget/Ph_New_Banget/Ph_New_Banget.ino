#include <Adafruit_ADS1X15.h>
#include <TFT_eSPI.h>

Adafruit_ADS1115 ads;
TFT_eSPI tft = TFT_eSPI();

int samples = 100;

#define SCAN_TIMEOUT 60000

// const double v7 = 1.9;
// const double v4 = 2.1;
// const double p7 = 6.86;
// const double p4 = 4.01;

// const float slope = (p7 - p4) / (v7 - v4);
// const float offset = p4 - (slope * v4);

float get_digit(float v){
  // float decimal = 0.01;
  // float temp = v%decimal;
  // return v - temp;
   
  return round(v * 1000.0) / 1000.0;
}

void setup() {
  Serial.begin(115200);

  tft.begin();
  tft.setRotation(1);
  tft.setTextSize(2);
  tft.fillScreen(ILI9341_BLUE);

  ads.setGain(GAIN_ONE);

  if (!ads.begin()) {
    Serial.println("ADS DMSAODSAPADSJ");
    while (1)
      ;
  }
  // put your setup code here, to run once:
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

bool is_freeze = false;
int adc1 = 0;
float volt = 0;
float ph = 0;

void loop() {
  while (millis() <= SCAN_TIMEOUT) {

    adc1 = read_adc_median(1);
    // int adc0 = read_adc_median(0);
    volt = ads.computeVolts(adc1);
    ph = -10.94 * get_digit(volt) + 45.22;



    // int adc0 = 0;
    // float volt = 0;
    // float pH = 0;

    // float total_volt = 0;
    // float total_pH = 0;

    // for (int i = 0; i < samples; i++) {
    //   adc1 = ads.readADC_SingleEnded(1);
    //   volt = ((adc1 * 0.125) / 1000.0) * 1.22;
    //   pH = -13.90026881 * volt + 44.09822356;

    //   total_volt += volt;
    //   total_pH += pH;
    // }

    // tft.setCursor(0, 0);
    // tft.print("ADC : ");
    // tft.println(adc1);
    // tft.print("Volt : ");
    // tft.println(total_volt / samples, 3);
    // tft.print("pH : ");
    // tft.println(total_pH / samples, 3);

    tft.setCursor(0, 0);
    tft.print("ADC : ");
    tft.println(adc1);
    tft.print("Volt : ");
    tft.println(volt);
    tft.print("pH : ");
    tft.println(ph);
    // tft.print("pH : ");
    // tft.println(total_pH / samples, 3);

    // Serial.print("ADC : ");
    // Serial.println(adc1);
    // Serial.print("Volt : ");
    // Serial.println(total_volt / samples, 3);
    // Serial.print("pH : ");
    // Serial.println(total_pH / samples, 3);
  }

  if (millis() > SCAN_TIMEOUT && !is_freeze) {
    is_freeze = true;
    tft.print("ADC : ");
    tft.println(adc1);
    tft.print("Volt : ");
    tft.println(volt);
    tft.print("pH : ");
    tft.println(ph);
    tft.println("Done!");
  }



  while (1) {
    adc1 = read_adc_median(1);
    volt = ads.computeVolts(adc1);
    ph = -10.94 * get_digit(volt) + 45.22;

    tft.setCursor(0, 0);
    tft.print("ADC : ");
    tft.println(adc1);
    tft.print("Volt : ");
    tft.println(volt);
    tft.print("pH : ");
    tft.println(ph);
  }
  // tft.print("ADC : ");
  // tft.println(adc0);
  // tft.print("Volt : ");
  // tft.println(ads.computeVolts(adc0));
  // put your main code here, to run repeatedly:
}
