#include <Wire.h>
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads; // Membuat objek untuk modul ADS1115

// === KONFIGURASI KALIBRASI ===
const float vNeutral = 2.55; // Tegangan titik tengah (pH 7) hasil short circuit di 3.3V
const float phSlope = 0.125;  // Karakteristik penurunan voltase per unit pH (180mV per pH)

void setup() {
  Serial.begin(115200);
  
  // Menginisialisasi komunikasi I2C (Default ESP32: SDA=21, SCL=22)
  Wire.begin(21, 22);
  
  // Mengecek apakah ADS1115 terhubung dengan benar
  if (!ads.begin()) {
    Serial.println("Koneksi ADS1115 Gagal! Periksa kabel SDA/SCL Anda.");
    while (1);
  }
  
  // GAIN_ONE mengatur rentang pembacaan ADS1115 hingga max +/- 4.096V
  // Nilai 1 bit ADC setara dengan 0.125 miliVolt (sangat presisi untuk sistem 3.3V)
  ads.setGain(GAIN_ONE); 
  
  Serial.println("Sistem Sensor pH Siap...");
}

void loop() {
  int32_t adcTotal = 0;
  int jumlahSampel = 50; // Mengambil 50 sampel data untuk membuang noise/riak tegangan

  // Proses pengambilan sampel (Oversampling)
  for (int i = 0; i < jumlahSampel; i++) {
    adcTotal += ads.readADC_SingleEnded(0); // Membaca pin A0 pada ADS1115
    delay(10); // Jeda kecil antar pembacaan
  }
  
  // Menghitung rata-rata nilai ADC mentah
  float rataRataAdc = (float)adcTotal / jumlahSampel;
  
  // KONVERSI ADC KE VOLTASE (Rumus Spesifik ADS1115 dengan GAIN_ONE)
  // Rentang 16-bit (positif saja) = 32768 tingkat pembacaan untuk 4.096V
  float voltage = ((rataRataAdc * 4.096) / 32768.0) + 0.44;

  // KONVERSI VOLTASE KE NILAI pH
  // Semakin kecil voltase dari 1.65V = Larutan bersifat Basa (pH > 7)
  // Semakin besar voltase dari 1.65V = Larutan bersifat Asam (pH < 7)
  float pHtot = 0;
  float phValue = 7.00 + ((vNeutral - voltage) / phSlope);
  for (int i=0;i<jumlahSampel;i++){
    pHtot += phValue;
  }

  phValue = pHtot / jumlahSampel;

  // === MENAMPILKAN DATA KE SERIAL MONITOR ===
  Serial.print("[ADC Mentah: ");
  Serial.print(rataRataAdc, 0);
  Serial.print("] | [Tegangan Po: ");
  Serial.print(voltage, 3); // 3 angka di belakang koma karena sangat sensitif
  Serial.print(" V] -> KUALITAS AIR (pH): ");
  Serial.println(phValue, 2);

  // delay(1000); // Update data setiap 1 detik
  Serial.print("Voltage = ");
  Serial.println(voltage);
  delay(200);
}