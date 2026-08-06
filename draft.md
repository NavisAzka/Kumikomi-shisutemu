# Draf Modul Praktikum Mikrokontroler & Embedded System

**Platform utama:** ESP32 (dengan pengenalan perbandingan STM32 di Modul 1)
**Target peserta:** D3/S1 Teknik (tingkat dasar)
**Total:** 5 modul besar, 16 pertemuan

---

## Struktur Umum Setiap Modul

Setiap modul mengikuti format:
1. Tujuan Pembelajaran
2. Dasar Teori Singkat
3. Alat & Bahan
4. Langkah Praktikum (per pertemuan)
5. Tugas/Latihan Mandiri
6. Rubrik Penilaian

---

## MODUL 1 — Pengantar Mikrokontroler & Dasar Pemrograman
**Durasi:** 3 pertemuan
**Platform:** ESP32 (utama), STM32 (pengenalan/pembanding)

### Tujuan Pembelajaran
- Memahami arsitektur dasar mikrokontroler dan perbedaan antar keluarga (AVR/Arduino, ESP32, STM32)
- Mampu melakukan instalasi toolchain dan upload program pertama
- Mampu membuat program logika dasar untuk mengontrol I/O digital

### Dasar Teori
- Arsitektur mikrokontroler: CPU, memori (Flash/RAM), peripheral, GPIO
- Perbandingan keluarga: AVR (8-bit, sederhana), ESP32 (32-bit dual-core, WiFi/BT built-in), STM32 (ARM Cortex-M, register-level/HAL, industrial-grade)
- Konsep GPIO sebagai digital input/output, pull-up/pull-down resistor
- Debouncing pada pushbutton

### Alat & Bahan
- ESP32 DevKit v1
- STM32 Nucleo/Blue Pill (untuk demo pembanding)
- Breadboard, kabel jumper
- LED (beberapa warna), resistor 220Ω/10kΩ
- Pushbutton, buzzer aktif

### Langkah Praktikum

**Pertemuan 1: Arsitektur & Perbandingan Keluarga Mikrokontroler**
1. Penjelasan arsitektur CPU, memori, dan peripheral mikrokontroler secara umum
2. Perbandingan tabel: AVR vs ESP32 vs STM32 (clock speed, memori, peripheral, ekosistem IDE)
3. Instalasi Arduino IDE/PlatformIO untuk ESP32
4. Upload program `Blink` pertama ke ESP32
5. Demo singkat: instalasi STM32CubeIDE dan upload `Blink` ke STM32 sebagai perbandingan alur kerja (HAL vs Arduino framework)

**Pertemuan 2: Digital Output & Input**
1. Kontrol LED tunggal dan multiple LED (digitalWrite)
2. Membaca status pushbutton (digitalRead) dengan pull-up internal
3. Implementasi debouncing secara software
4. Kombinasi input-output: LED menyala saat button ditekan

**Pertemuan 3: Studi Kasus Logika Kontrol**
1. Rancang program traffic light sederhana (3 LED: merah-kuning-hijau) dengan timing
2. Rancang program running LED (LED berjalan) menggunakan array dan loop
3. Modifikasi: kombinasikan dengan pushbutton untuk mengubah mode/kecepatan

### Contoh Kode (Pertemuan 1 — Blink ESP32)
```cpp
#define LED_PIN 2

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  delay(500);
}
```

### Tugas/Latihan Mandiri
- Buat program traffic light dengan tambahan mode "kedip kuning" saat malam (disimulasikan dengan pushbutton)
- Tulis laporan singkat perbandingan AVR, ESP32, STM32 (min. 3 aspek)

### Rubrik Penilaian
| Aspek | Bobot |
|---|---|
| Ketepatan instalasi & upload program | 20% |
| Kebenaran logika program I/O | 30% |
| Implementasi debouncing | 20% |
| Laporan perbandingan mikrokontroler | 20% |
| Kerapian & dokumentasi kode | 10% |

---

## MODUL 2 — Sensor Berdasarkan Basis Pengukuran & Aktuator PWM
**Durasi:** 3 pertemuan

### Tujuan Pembelajaran
- Memahami prinsip kerja sensor berdasarkan basis transduksi (resistif, kapasitif, induktif, lainnya)
- Mampu melakukan pembacaan dan kalibrasi sensor melalui ADC
- Mampu mengontrol aktuator menggunakan sinyal PWM

### Dasar Teori
- ADC pada ESP32 (resolusi, referensi tegangan, mapping nilai)
- Prinsip sensor resistif: perubahan resistansi terhadap besaran fisik
- Prinsip sensor kapasitif: perubahan kapasitansi terhadap besaran fisik
- Prinsip sensor induktif: perubahan induktansi/medan magnet
- Konsep PWM: duty cycle, frekuensi, aplikasi pada kontrol kecerahan/kecepatan

### Alat & Bahan
- LDR, resistor pembagi tegangan
- Thermistor NTC/PTC
- Potensiometer
- Capacitive touch sensor, capacitive soil moisture sensor
- Inductive proximity sensor, sensor hall effect
- Sensor ultrasonik HC-SR04, sensor IR obstacle
- Motor DC + driver (L298N), servo motor

### Langkah Praktikum

**Pertemuan 1: Sensor Resistif**
1. Penjelasan prinsip pembagi tegangan (voltage divider) untuk sensor resistif
2. Praktik pembacaan LDR (cahaya) via ADC, kalibrasi nilai lux relatif
3. Praktik pembacaan thermistor NTC (suhu), konversi nilai ADC ke °C
4. Praktik potensiometer sebagai input posisi/sudut

**Pertemuan 2: Sensor Kapasitif & Induktif**
1. Penjelasan prinsip perubahan kapasitansi (capacitive sensing)
2. Praktik capacitive touch sensor dan capacitive soil moisture sensor
3. Penjelasan prinsip sensor induktif dan efek Hall
4. Praktik inductive proximity sensor dan hall effect sensor (deteksi logam/medan magnet)
5. Diskusi: kapan memilih sensor resistif vs kapasitif vs induktif untuk aplikasi tertentu

**Pertemuan 3: Sensor Basis Lain & Aktuator PWM**
1. Praktik sensor ultrasonik HC-SR04 (basis akustik/waktu tempuh gelombang)
2. Praktik sensor IR obstacle (basis optik)
3. Penjelasan PWM dan praktik kontrol kecerahan LED
4. Praktik kontrol kecepatan motor DC dan posisi servo menggunakan PWM

### Contoh Kode (Pertemuan 1 — Pembacaan LDR)
```cpp
#define LDR_PIN 34

void setup() {
  Serial.begin(115200);
}

void loop() {
  int raw = analogRead(LDR_PIN);
  float voltage = raw * (3.3 / 4095.0);
  Serial.printf("Raw: %d, Voltage: %.2fV\n", raw, voltage);
  delay(200);
}
```

### Tugas/Latihan Mandiri
- Buat tabel klasifikasi sensor yang dipraktikkan berdasarkan basis pengukurannya
- Rancang sistem sederhana yang menggabungkan minimal 2 basis sensor berbeda (mis. resistif + induktif) untuk satu aplikasi

### Rubrik Penilaian
| Aspek | Bobot |
|---|---|
| Ketepatan pembacaan & kalibrasi sensor | 30% |
| Pemahaman klasifikasi basis sensor | 25% |
| Implementasi kontrol PWM aktuator | 25% |
| Laporan & analisis | 20% |

---

## MODUL 3 — Komunikasi Serial & Interfacing Modul (UART/I2C/SPI/DMA)
**Durasi:** 4 pertemuan

### Tujuan Pembelajaran
- Memahami dan mengimplementasikan protokol komunikasi UART, I2C, dan SPI
- Mampu melakukan interfacing dengan modul eksternal (LCD, sensor, penyimpanan)
- Memahami konsep DMA untuk transfer data efisien

### Dasar Teori
- UART: komunikasi asinkron, baud rate, frame data
- I2C: komunikasi sinkron dua kabel (SDA/SCL), alamat perangkat, master-slave
- SPI: komunikasi sinkron empat kabel (MOSI/MISO/SCK/CS), full-duplex
- DMA: transfer data antara peripheral dan memori tanpa keterlibatan CPU langsung

### Alat & Bahan
- 2x ESP32 (untuk komunikasi UART antar board)
- LCD 16x2 I2C
- Sensor I2C (BMP280 atau MPU6050)
- Modul SPI: SD card module / OLED SPI (ST7735)

### Langkah Praktikum

**Pertemuan 1: UART**
1. Penjelasan frame data UART dan konfigurasi baud rate
2. Praktik komunikasi serial ESP32 ke PC (Serial Monitor)
3. Praktik komunikasi UART antar dua board ESP32 (kirim-terima data sensor)

**Pertemuan 2: I2C**
1. Penjelasan protokol I2C dan scanning alamat perangkat (I2C scanner)
2. Praktik interfacing LCD 16x2 I2C — menampilkan data sensor
3. Praktik interfacing sensor BMP280/MPU6050 via I2C

**Pertemuan 3: SPI**
1. Penjelasan protokol SPI dan perbedaannya dengan I2C
2. Praktik interfacing modul SD card (baca/tulis file)
3. Praktik interfacing display OLED/TFT via SPI

**Pertemuan 4: DMA**
1. Penjelasan konsep DMA dan manfaatnya (mengurangi beban CPU)
2. Praktik ADC sampling kontinu menggunakan DMA, dibandingkan dengan ADC blocking biasa
3. Analisis perbandingan penggunaan CPU dan konsistensi sampling rate (blocking vs DMA)

### Contoh Kode (Pertemuan 2 — I2C Scanner)
```cpp
#include <Wire.h>

void setup() {
  Wire.begin();
  Serial.begin(115200);
  Serial.println("I2C Scanner");
}

void loop() {
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.printf("Ditemukan perangkat di alamat 0x%02X\n", addr);
    }
  }
  delay(5000);
}
```

### Tugas/Latihan Mandiri
- Buat sistem logging: baca sensor I2C, tampilkan di LCD, simpan ke SD card via SPI
- Bandingkan penggunaan CPU antara ADC sampling dengan dan tanpa DMA (buat grafik/tabel sederhana)

### Rubrik Penilaian
| Aspek | Bobot |
|---|---|
| Implementasi UART | 20% |
| Implementasi I2C | 25% |
| Implementasi SPI | 25% |
| Implementasi & analisis DMA | 20% |
| Laporan | 10% |

---

## MODUL 4 — Interrupt, Timer, Watchdog, & Multitasking
**Durasi:** 3 pertemuan

### Tujuan Pembelajaran
- Memahami dan mengimplementasikan interrupt (eksternal dan timer)
- Memahami fungsi watchdog timer untuk keandalan sistem
- Memahami dasar multitasking menggunakan FreeRTOS

### Dasar Teori
- External interrupt: konsep event-driven programming, ISR (Interrupt Service Routine)
- Timer interrupt: periodic task tanpa blocking delay()
- Watchdog Timer (WDT): mencegah sistem hang, auto-reset otomatis
- FreeRTOS: task, scheduler, semaphore, queue pada arsitektur dual-core ESP32

### Alat & Bahan
- ESP32 DevKit
- Rotary encoder atau sensor IR untuk trigger interrupt
- LED indikator (untuk visualisasi task paralel)

### Langkah Praktikum

**Pertemuan 1: Interrupt**
1. Penjelasan external interrupt dan attachInterrupt()
2. Praktik penghitung pulsa menggunakan rotary encoder/sensor IR
3. Praktik timer interrupt untuk task periodik (mis. blink tanpa delay())

**Pertemuan 2: Watchdog Timer**
1. Penjelasan konsep sistem hang/freeze dan pentingnya WDT
2. Praktik konfigurasi Watchdog Timer pada ESP32
3. Simulasi program "macet" (infinite loop) dan observasi auto-reset oleh WDT
4. Diskusi studi kasus penerapan WDT pada sistem embedded di industri

**Pertemuan 3: Multitasking Dasar (FreeRTOS)**
1. Penjelasan konsep task dan scheduler pada FreeRTOS
2. Praktik membuat multiple task (mis. task baca sensor + task update LED berjalan paralel)
3. Praktik komunikasi antar task menggunakan queue/semaphore sederhana

### Contoh Kode (Pertemuan 2 — Watchdog Timer)
```cpp
#include <esp_task_wdt.h>

#define WDT_TIMEOUT 3 // detik

void setup() {
  Serial.begin(115200);
  esp_task_wdt_init(WDT_TIMEOUT, true);
  esp_task_wdt_add(NULL);
}

void loop() {
  Serial.println("Sistem berjalan normal");
  esp_task_wdt_reset(); // feed the watchdog
  delay(1000);

  // Uncomment baris berikut untuk simulasi hang:
  // while(true) {}
}
```

### Tugas/Latihan Mandiri
- Rancang sistem dengan 2 task paralel (FreeRTOS) yang salah satunya disimulasikan hang, amati bagaimana watchdog per-task bereaksi
- Buat laporan analisis kapan sebaiknya menggunakan interrupt vs polling

### Rubrik Penilaian
| Aspek | Bobot |
|---|---|
| Implementasi interrupt | 25% |
| Implementasi watchdog timer | 30% |
| Implementasi multitasking FreeRTOS | 30% |
| Laporan analisis | 15% |

---

## MODUL 5 — Pemrosesan Data Sensor & Dasar Sistem Kontrol (Filtering & PID)
**Durasi:** 3 pertemuan

### Tujuan Pembelajaran
- Mampu menerapkan teknik filtering untuk mengurangi noise pada data sensor
- Memahami konsep sistem kontrol closed-loop
- Mampu mengimplementasikan dan melakukan tuning kontrol PID

### Dasar Teori
- Karakteristik noise pada sinyal sensor analog
- Moving average filter, low-pass filter (digital RC), median filter
- Konsep closed-loop control: setpoint, error, feedback
- Kontrol on-off (bang-bang) vs Proportional (P) vs PID
- Parameter tuning PID (Kp, Ki, Kd) dan metode dasar Ziegler-Nichols

### Alat & Bahan
- Sensor dari Modul 2 (LDR/thermistor/ultrasonik)
- Motor DC + driver, atau heater kecil + relay/MOSFET
- Rotary encoder (opsional, untuk feedback kecepatan/posisi)

### Langkah Praktikum

**Pertemuan 1: Filtering Sinyal Sensor**
1. Observasi noise pada pembacaan sensor mentah (raw) menggunakan Serial Plotter
2. Implementasi moving average filter
3. Implementasi low-pass filter digital
4. Implementasi median filter dan perbandingan ketiga metode

**Pertemuan 2: Dasar Sistem Kontrol Closed-Loop**
1. Penjelasan konsep setpoint, error, dan feedback loop
2. Implementasi kontrol on-off sederhana (mis. kontrol suhu dengan heater)
3. Implementasi kontrol Proportional (P) dan observasi perbedaan respons dengan on-off

**Pertemuan 3: PID Lengkap & Tuning**
1. Penjelasan komponen Integral dan Derivative
2. Implementasi kontrol PID penuh
3. Tuning manual (trial-error) parameter Kp, Ki, Kd
4. Studi kasus akhir: gabungan sensor terfilter (Modul 5 P1) + kontrol PID pada sistem nyata (mis. kontrol suhu atau kecepatan motor)
5. Analisis performa: overshoot, settling time, steady-state error (bandingkan on-off vs P vs PID)

### Contoh Kode (Pertemuan 1 — Moving Average Filter)
```cpp
#define WINDOW_SIZE 10
float buffer[WINDOW_SIZE];
int index = 0;

float movingAverage(float newValue) {
  buffer[index] = newValue;
  index = (index + 1) % WINDOW_SIZE;
  float sum = 0;
  for (int i = 0; i < WINDOW_SIZE; i++) sum += buffer[i];
  return sum / WINDOW_SIZE;
}

void setup() {
  Serial.begin(115200);
}

void loop() {
  int raw = analogRead(34);
  float filtered = movingAverage(raw);
  Serial.printf("Raw:%d\tFiltered:%.2f\n", raw, filtered);
  delay(50);
}
```

### Contoh Kode (Pertemuan 3 — PID Sederhana)
```cpp
float Kp = 2.0, Ki = 0.5, Kd = 1.0;
float setpoint = 25.0; // contoh: target suhu
float error, lastError = 0, integral = 0;
unsigned long lastTime = 0;

float computePID(float currentValue) {
  unsigned long now = millis();
  float dt = (now - lastTime) / 1000.0;
  if (dt <= 0) dt = 0.001;

  error = setpoint - currentValue;
  integral += error * dt;
  float derivative = (error - lastError) / dt;

  float output = (Kp * error) + (Ki * integral) + (Kd * derivative);

  lastError = error;
  lastTime = now;
  return output;
}
```

### Tugas/Latihan Mandiri
- Bandingkan hasil filtering (moving average vs low-pass vs median) pada sensor yang sama, sajikan dalam grafik
- Lakukan tuning PID pada sistem yang dipilih, dokumentasikan proses trial-error dan hasil akhirnya

### Rubrik Penilaian
| Aspek | Bobot |
|---|---|
| Implementasi filtering | 25% |
| Implementasi kontrol on-off & P | 20% |
| Implementasi PID lengkap | 30% |
| Analisis performa (overshoot, settling time, dll) | 25% |

---

## Ringkasan Struktur Keseluruhan

| Modul | Judul | Pertemuan |
|---|---|---|
| 1 | Pengantar Mikrokontroler (ESP32 + perbandingan STM32) & Dasar Pemrograman | 3 |
| 2 | Sensor Berdasarkan Basis Pengukuran & Aktuator PWM | 3 |
| 3 | Komunikasi Serial & Interfacing Modul (UART/I2C/SPI/DMA) | 4 |
| 4 | Interrupt, Timer, Watchdog, & Multitasking | 3 |
| 5 | Pemrosesan Data Sensor & Dasar Sistem Kontrol (Filtering & PID) | 3 |
| **Total** | | **16 pertemuan** |
