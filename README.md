PROGRAM SOFTWARE LIVADENT FOR ESP32

# Kalo mau run programnya wajib install yang ada di folder "WAJIB PASANG INI DULU" yh
## Folder Contain  :
--> UI_LCD.zip
--> lv_conf.h

# Langka 1 :
Ekstrak file "UI_LCD.zip" dulu

# Langkah 2 :
Copy kedua file (Folder "UI_LCD" dan File "lv_conf.h") ke "C:\Users\{Nama User di Windows kalian}\Documents\Arduino\libraries"

# Langkah 3 :
Buka Arduino IDE, terus buka Library Manager dan install beberapa library ini :
1. lvgl
2. TFT_eSPI
3. FirebaseClient
4. ArduinoJson
5. TensorFlowLite_ESP32
6. AccelStepper
7. Adafruit_ADS1X15
8. ESPAsycWebServer Bundle (Harus install manual)

Cara install libray nomer 8
8A. Ekstrak File ESPAsycWebServer Bundle (Ada 2 folder)
8B. Copy 2 filenya
8C. Paste di Document->Arduino->libraries

# Langkah 4 :
Buka folder "LIVADENT" di repo ini, terus buka file "LIVADENT.ino"

# Langkah 5 :
Tancepin esp32 ke laptop (Jangan lupa komponen-komponennya juga udah kepasang)
## DETAIL SIRKUIT COMING SOON YHHH

# Langkah 6 :
Pencet icon tanda panah di pojok kiri atas (upload) buat upload kode ke esp32

# Langkah 7 :
Tunggu sampe selesai dan esp32 bisa dioperasikan di lcd

## INGFOKAN KALO ADA BUG ATAU ERROR
