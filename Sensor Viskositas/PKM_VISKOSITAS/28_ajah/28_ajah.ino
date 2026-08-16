#include <Arduino.h>
#include <Wire.h>               // Wajib untuk komunikasi I2C
#include <Adafruit_ADS1X15.h>   // Library ADS1115
#include <TMCStepper.h>
#include <AccelStepper.h>
#include <TFT_eSPI.h>

// --- Definisi Pin ---
#define EN_PIN           2  // Pin Enable
#define DIR_PIN          13 // Pin Direction
#define STEP_PIN         12 // Pin Step
#define SERIAL_PORT Serial2 // HardwareSerial2 ESP32 (RX2=16, TX2=17)
#define DRIVER_ADDRESS 0b00 // Alamat default TMC2209

#define R_SENSE       0.11f // Nilai resistor sense standar TMC2209
#define RMS_CURRENT    367 // Arus dinaikkan ke 800 mA agar kuat di RPM tinggi!

// --- Inisialisasi Objek ---
Adafruit_ADS1115 ads; 
TFT_eSPI tft = TFT_eSPI();

TMC2209Stepper driver(&SERIAL_PORT, R_SENSE, DRIVER_ADDRESS);
AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

TaskHandle_t TaskMotor;

// --- TASK CORE 1: Khusus Eksekusi Pulsa Motor ---
void codeTaskMotor(void * pvParameters) {
  for (;;) {
    if (stepper.distanceToGo() == 0) {
      stepper.moveTo(stepper.currentPosition() + 200000000);
    }
    stepper.run(); 
    taskYIELD(); // Mencegah Watchdog Reset
  }
}

// --- FUNGSI MEAN / MOVING AVERAGE UNTUK LOAD ---
#define SAMPLE_SIZE 80  // Ambil rata-rata dari 15 sampel pembacaan
uint16_t getFilteredLoad() {
  long total = 0;
  for (int i = 0; i < SAMPLE_SIZE; i++) {
    total += driver.SG_RESULT();
    delayMicroseconds(10); // Jeda singkat antar pembacaan
  }
  return total / SAMPLE_SIZE;
}

float getVolt(uint16_t single){
  float ret_val = 0;
  for(int i=0;i<SAMPLE_SIZE;i++){
    int16_t adc = ads.readADC_SingleEnded(single);
    float voltage = ads.computeVolts(adc);  
    ret_val += voltage;
  }

  return ret_val / SAMPLE_SIZE;
}

void setup() {
  Serial.begin(115200);
  SERIAL_PORT.begin(115200, SERIAL_8N1, 16, 17); 

  // --- Inisialisasi TFT ---
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK); 
  tft.setTextSize(2);

  // --- Inisialisasi ADS1115 ---
  Wire.begin(21, 22);
  if (!ads.begin()) {
    Serial.println("Gagal mendeteksi ADS1115!");
  } else {
    ads.setGain(GAIN_SIXTEEN);
  }

  // --- Inisialisasi Pin Stepper ---
  pinMode(EN_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW); 

  // --- Konfigurasi TMC2209 untuk HIGH SPEED ---
  driver.begin();
  driver.toff(4);                   
  driver.blank_time(24);
  driver.rms_current(RMS_CURRENT);  // 800 mA agar bertenaga
  
  // SPREADCYCLE AKTIF: Mencegah stall di RPM tinggi
  driver.en_spreadCycle(true);      
  driver.pwm_autoscale(false);
    
  driver.microsteps(8);            
  driver.intpol(true);              

  // --- SETTING KECEPATAN TINGGI ---
  stepper.setMaxSpeed(4867);        // Kecepatan tinggi (Silakan sesuaikan 3000 - 6000)
  stepper.setAcceleration(200);    // Ramp-up bertahap agar tidak kaget/stall saat awal start
  stepper.setEnablePin(EN_PIN);
  stepper.setPinsInverted(false, false, true); 
  stepper.enableOutputs();
  stepper.moveTo(200000000);

  // --- Task khusus Motor di Core 1 ---
  xTaskCreatePinnedToCore(
    codeTaskMotor, "TaskMotor", 10000, NULL, 1, &TaskMotor, 1
  );

  delay(500);
}

void loop() {
  static unsigned long lastUpdate = 0;
  const unsigned long updateInterval = 50; // Update cepat tiap 50ms

  if (millis() - lastUpdate >= updateInterval) {
    lastUpdate = millis();
    
    // 1. Baca ADC ADS1115
    float voltage = getVolt(0);  

    // 2. Baca SG_RESULT yang SUDAH DI-RATA-RATA (MEAN)
    uint16_t sgFiltered = getFilteredLoad(); 

    // Output Serial untuk Monitoring
    Serial.print("Volt:"); Serial.print(voltage, 8);
    Serial.print("V | Load_Mean:"); Serial.println(sgFiltered);

    // Display TFT
    tft.setCursor(0, 10);
    tft.print("Volt    : ");
    tft.print(voltage, 8);
    tft.println(" V   "); 

    tft.setCursor(0, 45);
    tft.print("SG Mean : ");
    tft.print(sgFiltered);
    tft.println("    "); 

    tft.setCursor(0, 80);
    if (sgFiltered < 50) {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.print("STATUS  : TERHAMBAT!   ");
    } else {
      tft.setTextColor(TFT_GREEN, TFT_BLACK);
      tft.print("STATUS  : PUTAR LANCAR ");
    }
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
  }
}