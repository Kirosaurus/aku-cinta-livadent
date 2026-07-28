#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <AccelStepper.h>

#define pin_step 18
#define pin_dir 19
#define pin_enable 17 

AccelStepper stepper(AccelStepper::DRIVER, pin_step, pin_dir);
Adafruit_ADS1115 ads;

// Task Khusus di Core 0 untuk membaca ADC & Print Serial
void TaskSensor(void * pvParameters) {
  // Inisialisasi I2C dan ADS1115 di Core 0
  Wire.begin(21, 22);
  
  if (!ads.begin(0x48)) {
    Serial.println("ADS1115 tidak terdeteksi di Core 0!");
  } else {
    ads.setGain(GAIN_ONE);
  }

  for (;;) {
    int16_t adc0 = ads.readADC_SingleEnded(0);
    float voltage = ads.computeVolts(adc0);

    Serial.print("ADC 0: ");
    Serial.print(adc0);
    Serial.print(" | Tegangan: ");
    Serial.print(voltage, 10);
    Serial.println(" V");

    // Jeda pembacaan sensor 200ms (tidak menggangu motor)
    vTaskDelay(200 / portTICK_PERIOD_MS); 
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(pin_step, OUTPUT);
  pinMode(pin_dir, OUTPUT);
  pinMode(pin_enable, OUTPUT);
  digitalWrite(pin_enable, LOW); // Enable driver stepper

  stepper.setMaxSpeed(18000);
  stepper.setAcceleration(1000);
  stepper.moveTo(2000000000L);

  // Buat Task yang berjalan khusus di Core 0
  xTaskCreatePinnedToCore(
    TaskSensor,   /* Fungsi Task */
    "TaskSensor", /* Nama Task */
    4096,         /* Ukuran Stack */
    NULL,         /* Parameter */
    1,            /* Prioritas Task */
    NULL,         /* Task Handle */
    0             /* Running di Core 0 */
  );
}

void loop() {
  // Loop Utama berjalan di Core 1 — Khusus 100% untuk Pulsa Motor Stepper
  stepper.run();
}