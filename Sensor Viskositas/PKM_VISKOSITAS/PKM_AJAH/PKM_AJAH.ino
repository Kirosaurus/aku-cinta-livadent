#include <Arduino.h>
#include <Wire.h>               // Wajib untuk komunikasi I2C
#include <Adafruit_ADS1X15.h>   // Library ADS1115
#include <TMCStepper.h>
#include <AccelStepper.h>
#include <TFT_eSPI.h>

// --- Definisi Pin ---
#define EN_PIN           2 // Pin Enable
#define DIR_PIN          13 // Pin Direction
#define STEP_PIN         12 // Pin Step
#define SERIAL_PORT Serial2 // Gunakan HardwareSerial untuk UART TMC2209
#define DRIVER_ADDRESS 0b00 // Alamat default TMC2209

#define R_SENSE       0.11f // Nilai resistor sense standar TMC2209
#define RMS_CURRENT    380 // Arus motor (mA)

// --- Inisialisasi Objek ---
Adafruit_ADS1115 ads; 
TFT_eSPI tft = TFT_eSPI();

// Driver TMC2209 & AccelStepper
TMC2209Stepper driver(&SERIAL_PORT, R_SENSE, DRIVER_ADDRESS);
AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

// Task Handle untuk Dual Core
TaskHandle_t TaskMotor;

// --- TASK CORE 1: khusus untuk menggerakkan stepper motor ---
void codeTaskMotor(void * pvParameters) {
  for (;;) {
    // Jika motor sudah sampai di target posisi, balikkan arah target
    if (stepper.distanceToGo() == 0) {
      stepper.moveTo(200000000);
    }

    // Eksekusi pulsa motor secepat mungkin tanpa terganggu I2C/TFT
    stepper.run(); 
  }
}

void setup() {
  Serial.begin(115200);
  SERIAL_PORT.begin(115200); // Baudrate UART driver

  // --- Inisialisasi TFT ---
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLUE);
  tft.setTextColor(TFT_WHITE, TFT_BLUE); 
  tft.setTextSize(2);

  // --- Inisialisasi ADS1115 ---
  Wire.begin(); // Pin SDA & SCL default (GPIO 21 & 22)
  if (!ads.begin()) {
    Serial.println("Gagal mendeteksi ADS1115! Periksa wiring I2C.");
  } else {
    ads.setGain(GAIN_ONE); // GAIN_ONE = rentang ±4.096V
  }

  // --- Inisialisasi Pin Stepper ---
  pinMode(EN_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW); // Enable driver (Active LOW)

  // --- Konfigurasi TMC2209 ---
  driver.begin();
  driver.toff(5);                   
  driver.rms_current(RMS_CURRENT);  
  
  driver.en_spreadCycle(true);      // SpreadCycle untuk Torsi Tinggi
  driver.pwm_autoscale(true);
  
  driver.microsteps(8);            
  driver.intpol(true);              // Interpolasi 256

  // --- Konfigurasi AccelStepper ---
  stepper.setMaxSpeed(8500);        
  stepper.setAcceleration(250);     
  stepper.setEnablePin(EN_PIN);
  stepper.setPinsInverted(false, false, true); 
  stepper.enableOutputs();
  stepper.moveTo(200000000);

  Serial.println("Setup TMC2209 & AccelStepper Selesai.");

  // --- Membuat Task Khusus Motor di Core 1 ---
  xTaskCreatePinnedToCore(
    codeTaskMotor,   // Fungsi task
    "TaskMotor",     // Nama task
    10000,           // Ukuran stack (byte)
    NULL,            // Parameter task
    1,               // Prioritas task (Tinggi)
    &TaskMotor,      // Task handle
    1                // Dipasang pada CORE 1
  );

  delay(500); // Beri waktu sebentar agar Core 1 siap
}

// --- LOOP UTAMA (Secara otomatis berjalan di CORE 0) ---
void loop() {
  static unsigned long lastAdcRead = 0;
  const unsigned long adcInterval = 100; // Pembacaan lancar & responsif tiap 100 ms

  if (millis() - lastAdcRead >= adcInterval) {
    lastAdcRead = millis();
    
    // Read ADC & Voltage
    int16_t adc0 = ads.readADC_SingleEnded(0); 
    float voltage = ads.computeVolts(adc0);    

    // Output Serial
    Serial.print("Raw ADS1115 (A0): ");
    Serial.print(adc0);
    Serial.print(" | Tegangan: ");
    Serial.print(voltage, 6);
    Serial.println(" V");

    // Output Display TFT
    tft.setCursor(0, 0);
    tft.print("ADC : ");
    tft.print(adc0);
    tft.println("        "); // Space filler untuk menghapus karakter sisa

    tft.print("Volt: ");
    tft.print(voltage, 4);    // Ditampilkan 4 desimal agar rapi di TFT
    tft.println(" V      ");
  }
}