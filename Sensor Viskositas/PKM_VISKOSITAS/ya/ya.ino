#include <Arduino.h>
#include <Wire.h>               
#include <Adafruit_ADS1X15.h>   
#include <TMCStepper.h>
#include <AccelStepper.h>
#include <TFT_eSPI.h>

// --- Definisi Pin ---
#define EN_PIN           2  // Pin Enable (Active LOW)
#define DIR_PIN         13  // Pin Direction (Output)
#define STEP_PIN        12  // Pin Step (Output)

#define RXD2            16  // Pin RX2 ESP32 (ke TX TMC2209)
#define TXD2            17  // Pin TX2 ESP32 (ke RX TMC2209)

#define SERIAL_PORT Serial2 
#define DRIVER_ADDRESS 0b00 

#define R_SENSE       0.11f 
#define RMS_CURRENT    800  // Arus dinaikkan ke 800 mA agar ada torsi awal

// Inisialisasi ADS1115 & TFT
Adafruit_ADS1115 ads; 
TFT_eSPI tft = TFT_eSPI();

// Inisialisasi Driver TMC2209 (UART)
TMC2209Stepper driver(&SERIAL_PORT, R_SENSE, DRIVER_ADDRESS);

// Inisialisasi AccelStepper
AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

unsigned long lastAdcRead = 0;
const unsigned long adcInterval = 100; 

int16_t adc0 = 0;
float voltage = 0.0;

void setup() {
  Serial.begin(115200);
  delay(500); // Beri waktu Serial Monitor siap

  // 1. Inisialisasi UART TMC2209 dengan Pin RX/TX Eksplisit
  SERIAL_PORT.begin(115200, SERIAL_8N1, RXD2, TXD2); 

  // 2. Setup Display TFT
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLUE);
  tft.setTextColor(TFT_WHITE, TFT_BLUE); 
  tft.setTextSize(2);

  // 3. Setup ADS1115
  Wire.begin(); 
  if (!ads.begin()) {
    Serial.println("[ADS1115] Gagal terdeteksi!");
  } else {
    ads.setGain(GAIN_ONE); 
  }

  // 4. Config Hardware Pin Stepper
  pinMode(EN_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW); // LOW = Enable Driver

  // 5. Config TMC2209
  driver.begin();
  driver.toff(5);                   
  driver.rms_current(RMS_CURRENT);  
  driver.en_spreadCycle(true);      // Force SpreadCycle
  driver.pwm_autoscale(true);
  driver.microsteps(16);            
  driver.intpol(true);              

  // Cek Komunikasi UART TMC2209 via Serial
  uint8_t result = driver.test_connection();
  if (result == 0) {
    Serial.println(">>> SUCCESS: TMC2209 Terkoneksi lewat UART! <<<");
  } else {
    Serial.print(">>> ERROR: TMC2209 Tidak Merespons UART! Kode Error: ");
    Serial.println(result);
  }

  // 6. Config AccelStepper
  stepper.setMaxSpeed(4000);        // Kecepatan dites di 4000 dulu
  stepper.setAcceleration(500);     
  stepper.setEnablePin(EN_PIN);
  stepper.setPinsInverted(false, false, true); // Active LOW enable

  stepper.enableOutputs();
  
  // Beri target posisi tegas di awal setup
  stepper.moveTo(1000000);

  Serial.println("Setup Selesai - Motor Harus Berjalan Sekarang");
}

void loop() {
  // Wajib dipanggil setiap siklus CPU
  stepper.run(); 

  // Read ADC & TFT Display
  if (millis() - lastAdcRead >= adcInterval) {
    lastAdcRead = millis();
    
    adc0 = ads.readADC_SingleEnded(0);    
    voltage = ads.computeVolts(adc0);       

    // Tampilkan data ke Serial Monitor
    Serial.print("ADC: ");
    Serial.print(adc0);
    Serial.print(" | Volt: ");
    Serial.print(voltage, 4);
    Serial.print(" V | Stepper Pos: ");
    Serial.println(stepper.currentPosition());

    // Display TFT
    tft.setCursor(0, 0);
    tft.print("ADC : ");
    tft.print(adc0);
    tft.println("      "); 

    tft.print("Volt: ");
    tft.print(voltage, 4); 
    tft.println(" V    ");
  }

  // Logika Bolak-Balik Stepper
  if (stepper.distanceToGo() == 0) {
    long nextTarget = (stepper.currentPosition() == 0) ? 1000000 : 0;
    stepper.moveTo(nextTarget);
  }
}