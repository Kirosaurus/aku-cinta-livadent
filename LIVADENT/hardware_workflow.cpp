#include "hardware_workflow.h"

void process() {
  vector<float> ph, visc, risk_label;
  File file = LittleFS.open("/Data Testing.csv", "r");
  file.readStringUntil('\n');

  while (file.available()) {
    String line = file.readStringUntil('\n');
    line.trim();

    int c1 = line.indexOf(',');
    int c2 = line.indexOf(',', c1 + 1);

    ph.push_back(line.substring(0, c1).toFloat());
    visc.push_back(line.substring(c1 + 1, c2).toFloat());
    visc.push_back(line.substring(c2 + 1).toFloat());
  }

  file.close();


  for (int i = 0; i < 10; i++) {
    Serial.print("Ini Proses ");
    Serial.println(i);
    delay(1000);
  }

//   Serial.println("Output Array Test");
//   for (int i = 0; i < ph.size(); i++) {
//     Serial.println(ph[i]);
//   }

    uint8_t index = esp_random()%ph.size();
    ph_sensor = ph[index];
    visc_sensor = visc[index];
    mlp_risk_label = predict_test(ph_sensor, visc_sensor);

    lv_scr_load_anim(objects.result, LV_SCR_LOAD_ANIM_FADE_IN, 0, 200, false);
}