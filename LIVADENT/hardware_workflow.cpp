#include "hardware_workflow.h"

// const int samples = 100;

const double v7 = 1.8;
const double v4 = 2.1;
const double p7 = 6.86;
const double p4 = 4.01;

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

void process() {
  //   vector<float> ph, visc, risk_label;
  //   File file = LittleFS.open("/Data Testing.csv", "r");
  //   file.readStringUntil('\n');

  //   while (file.available()) {
  //     String line = file.readStringUntil('\n');
  //     line.trim();

  //     int c1 = line.indexOf(',');
  //     int c2 = line.indexOf(',', c1 + 1);

  //     ph.push_back(line.substring(0, c1).toFloat());
  //     visc.push_back(line.substring(c1 + 1, c2).toFloat());
  //     visc.push_back(line.substring(c2 + 1).toFloat());
  //   }

  //   file.close();


  //   for (int i = 0; i < 10; i++) {
  //     Serial.print("Ini Proses ");
  //     Serial.println(i);
  //     delay(1000);
  //   }

  // //   Serial.println("Output Array Test");
  // //   for (int i = 0; i < ph.size(); i++) {
  // //     Serial.println(ph[i]);
  // //   }

  //     uint8_t index = esp_random()%ph.size();
  //     ph_sensor = ph[index];
  //     visc_sensor = visc[index];
  //     mlp_risk_label = predict_test(ph_sensor, visc_sensor);

  double totVolt = 0.00;
  double volt = 0.00;
  double totpH = 0.00;
  double pH = 0.00;
  
  // for (int i = 0; i < samples; i++) {
    // Serial.println("Masuk");
    int adc1 = read_adc_median(1);
    volt = ads.computeVolts(adc1);
    // pH = -35.625 * volt + 37.85375;
    pH =  -9.5 * volt + 23.96;
    // totpH += pH;
    // totVolt += volt;
    // i++;
  // }
    // Serial.println("Keluar");

  int adc0 = read_adc_median(0);

  // ph_sensor = totpH / samples;
  ph_sensor = pH;
  visc_sensor = ads.computeVolts(adc0); // INI CUMA PLACEHOLDER
  mlp_risk_label = predict_test(ph_sensor, visc_sensor);

  lv_scr_load_anim(objects.result, LV_SCR_LOAD_ANIM_FADE_IN, 0, 200, false);
}