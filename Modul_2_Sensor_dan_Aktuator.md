# MODUL 2
# SENSOR BERDASARKAN BASIS PENGUKURAN & AKTUATOR MOTOR

**Mata Kuliah:** Praktikum Mikrokontroler & Embedded System
**Alokasi Waktu:** 6 x Percobaan (@ 100–150 menit)
**Platform:** ESP32 (Framework Arduino)
**IDE:** VSCode + PlatformIO

> **Catatan:** Modul ini melanjutkan penggunaan ESP32 dengan framework Arduino seperti pada Percobaan 3–4 Modul 1. Langkah instalasi VSCode, PlatformIO, dan driver USB-to-Serial tidak diulang di sini — lihat kembali **Modul 1, Bagian D**.

---

## A. Capaian Pembelajaran

Setelah menyelesaikan Modul 2, praktikan mampu:
1. Menjelaskan prinsip kerja sensor berdasarkan basis transduksi (resistif, kapasitif, induktif, dan basis lain)
2. Mengimplementasikan pembacaan sensor resistif melalui ADC serta melakukan kalibrasi nilai
3. Mengimplementasikan pembacaan sensor kapasitif dan induktif
4. Mengimplementasikan pembacaan sensor berbasis akustik (ultrasonik) dan optik (inframerah)
5. Mengimplementasikan kontrol kecepatan dan arah motor DC menggunakan sinyal PWM
6. Mengimplementasikan kontrol motor stepper menggunakan sinyal step/direction
7. Mengimplementasikan kontrol posisi motor servo menggunakan sinyal PWM

---

## B. Alat dan Bahan

| No | Nama Komponen | Spesifikasi | Jumlah |
|---|---|---|---|
| 1 | Board ESP32 DevKit | ESP32 DevKit v1 | 1 |
| 2 | Kabel USB Micro/USB-C | untuk ESP32 ke PC | 1 |
| 3 | Breadboard | 830 titik | 1 |
| 4 | Kabel jumper male-male | — | secukupnya |
| 5 | LDR (Light Dependent Resistor) | — | 1 |
| 6 | Resistor | 10kΩ (pembagi tegangan LDR), 220Ω (LED) | secukupnya |
| 7 | Kabel/kawat sebagai probe capacitive touch | — | 1 |
| 8 | Hall effect sensor module | mis. A3144/KY-003 | 1 |
| 9 | Magnet kecil | untuk uji hall effect | 1 |
| 10 | Sensor ultrasonik | HC-SR04 | 1 |
| 11 | Sensor IR obstacle | — | 1 |
| 12 | Motor DC + driver | mis. L298N/L293D | 1 |
| 13 | Motor stepper | mis. NEMA17 atau sejenis | 1 |
| 14 | Driver motor stepper | mis. A4988/DRV8825 | 1 |
| 15 | Servo motor | SG90 | 1 |
| 16 | Catu daya eksternal | sesuai kebutuhan motor DC/stepper (jangan gunakan 5V dari USB langsung) | 1 |
| 17 | Laptop/PC | VSCode + PlatformIO terinstal | 1 |

---

## C. Dasar Teori

### C.1 ADC pada ESP32
ESP32 memiliki ADC (Analog-to-Digital Converter) internal dengan resolusi default **12-bit** (nilai 0–4095) dan referensi tegangan sekitar **0–3.3V**. Fungsi `analogRead(pin)` pada framework Arduino mengembalikan nilai digital yang merepresentasikan tegangan analog pada pin tersebut. Perlu diperhatikan bahwa ADC ESP32 memiliki karakteristik non-linear pada tegangan rendah dan tinggi, sehingga kalibrasi/pemetaan nilai (mapping) seringkali diperlukan agar hasil pembacaan sesuai dengan besaran fisik yang diukur.

### C.2 Sensor Resistif — Prinsip Pembagi Tegangan
Sensor resistif (LDR, thermistor, potensiometer) bekerja dengan mengubah nilai resistansinya terhadap besaran fisik (cahaya, suhu, posisi). Karena mikrokontroler hanya dapat membaca tegangan (bukan resistansi secara langsung), sensor resistif dirangkai sebagai **pembagi tegangan (voltage divider)** bersama resistor tetap, sehingga perubahan resistansi menghasilkan perubahan tegangan yang dapat dibaca oleh ADC.

```
3.3V ---[Sensor Resistif]---+---[Resistor Tetap]--- GND
                             |
                          ke ADC
```

### C.3 Sensor Kapasitif
Sensor kapasitif mendeteksi besaran fisik (sentuhan, kelembapan, jarak dekat) melalui **perubahan nilai kapasitansi**. Pada praktikum ini digunakan fitur **touch sensor bawaan ESP32**, tersedia pada beberapa pin GPIO tertentu (T0–T9), yang dapat langsung mendeteksi sentuhan tanpa komponen tambahan, memanfaatkan perubahan kapasitansi tubuh manusia terhadap pin tersebut. Sensor kapasitif lain (mis. capacitive soil moisture, capacitive proximity) bekerja dengan prinsip serupa — mengukur perubahan kapasitansi pada elektroda sensor — namun umumnya berbentuk modul terpisah dengan output analog.

### C.4 Sensor Induktif
Sensor induktif mendeteksi objek logam atau perubahan medan magnet melalui **perubahan induktansi/medan magnet**. Pada praktikum ini digunakan **hall effect sensor**, yang mendeteksi keberadaan/kekuatan medan magnet secara langsung — umum digunakan untuk mendeteksi posisi magnet atau kecepatan putar (bersama magnet pada objek berputar). Contoh sensor induktif lain adalah *inductive proximity sensor*, yang menghasilkan medan elektromagnetik osilasi dan mendeteksi benda logam melalui redaman (eddy current) pada medan tersebut.

### C.5 Sensor Basis Lain (Akustik & Optik)
- **Ultrasonik (akustik):** mengukur jarak berdasarkan waktu tempuh gelombang suara (pulsa dipancarkan, dipantulkan objek, lalu diterima kembali); jarak dihitung dari selisih waktu dan kecepatan suara di udara
- **Inframerah/optik:** mendeteksi objek berdasarkan pantulan cahaya inframerah; umum digunakan sebagai sensor jarak dekat/obstacle dengan output digital

### C.6 PWM (Pulse Width Modulation) — Kontrol Kecepatan Motor DC
PWM adalah teknik menghasilkan sinyal digital yang menyerupai sinyal analog dengan mengatur **duty cycle** (persentase waktu sinyal HIGH dalam satu periode). Semakin besar duty cycle, semakin besar "rata-rata" tegangan yang dirasakan oleh beban (mis. motor DC), sehingga kecepatan putarnya meningkat. Pada ESP32 framework Arduino, PWM diakses melalui API **LEDC**. Sejak **Arduino-ESP32 core versi 3.x**, API ini disederhanakan menjadi berbasis pin — `ledcAttach(pin, freq, resolution)` untuk mengonfigurasi sekaligus menghubungkan PWM ke suatu pin, dan `ledcWrite(pin, duty)` untuk mengatur duty cycle-nya — menggantikan API lama berbasis channel manual (`ledcSetup()`/`ledcAttachPin()`) pada core versi 2.x. Arah putar motor DC diatur secara terpisah melalui driver motor (mis. L298N) menggunakan dua pin digital (IN1/IN2).

> **Catatan versi:** Seluruh contoh kode PWM pada modul ini (dan modul-modul lain dalam rangkaian praktikum) mengasumsikan **Arduino-ESP32 core versi 3.x ke atas** (ESP-IDF 5.x), agar konsisten dengan driver ADC continuous/DMA yang digunakan pada Modul 3. Pastikan `platformio.ini` tidak mengunci platform `espressif32` ke versi lama — biarkan tanpa versi spesifik atau gunakan versi terbaru agar API LEDC baru tersedia.

### C.7 Kontrol Posisi Motor Servo
Motor servo juga dikendalikan menggunakan sinyal PWM, namun dengan prinsip yang berbeda dari kontrol kecepatan motor DC — pada servo, **lebar pulsa (pulse width)** itu sendiri yang menentukan posisi sudut, bukan rata-rata duty cycle. Umumnya sinyal kontrol servo memiliki periode 20ms (frekuensi 50Hz), dengan lebar pulsa sekitar 1ms merepresentasikan sudut 0° dan 2ms merepresentasikan sudut 180°. Pada framework Arduino, detail ini diabstraksi oleh library seperti **ESP32Servo**, sehingga cukup memanggil fungsi `.write(angle)` untuk menggerakkan servo ke sudut tertentu.

### C.8 Motor Stepper
Berbeda dengan motor DC yang berputar kontinu, motor stepper bergerak dalam **langkah-langkah diskret (step)** sesuai jumlah pulsa yang diberikan — misalnya 1.8° per step pada motor stepper standar (200 step per putaran penuh). Motor stepper dikendalikan melalui **driver motor stepper** (mis. A4988, DRV8825), yang menerima sinyal:
- **STEP:** setiap pulsa (transisi LOW→HIGH) menyebabkan motor berputar satu step
- **DIR:** menentukan arah putaran motor
- **EN (Enable):** mengaktifkan/menonaktifkan driver — saat dinonaktifkan, motor dapat diputar bebas secara manual
- **RESET/SLEEP:** beberapa driver memiliki pin ini untuk mereset atau menonaktifkan mode tidur driver, umumnya perlu ditarik HIGH agar driver aktif normal

---

## D. Persiapan Sebelum Praktikum

1. Pastikan PlatformIO sudah terinstal (lihat **Modul 1, Bagian D.1–D.2**)
2. Buat project baru untuk Modul 2:
   - Name: `modul2-sensor-aktuator`
   - Board: **"Espressif ESP32 Dev Module"**
   - Framework: **Arduino**
3. Pastikan `platformio.ini` berisi:
   ```ini
   [env:esp32dev]
   platform = espressif32
   board = esp32dev
   framework = arduino
   ```
4. Untuk Percobaan 6 (Motor Servo), tambahkan library **ESP32Servo** melalui PlatformIO Library Manager atau tambahkan pada `platformio.ini`:
   ```ini
   lib_deps = madhephaestus/ESP32Servo@^3.0.0
   ```

---

## E. Kegiatan Praktikum

### PERCOBAAN 1 — Sensor Resistif

**Tujuan:**
Mahasiswa mampu memahami dan mengimplementasikan pembacaan sensor resistif (LDR) melalui ADC ESP32.

**Skema Rangkaian:**

| Komponen | Pin ESP32 | Keterangan |
|---|---|---|
| LDR (pembagi tegangan dengan resistor 10kΩ) | GPIO 34 | Titik tengah pembagi tegangan ke ADC |

**Langkah Kerja:**
1. Rangkai LDR sebagai pembagi tegangan sesuai skema, hubungkan titik tengah ke GPIO 34
2. Tulis program pembacaan nilai ADC dan tegangan LDR
3. Amati perubahan nilai saat sensor ditutup (gelap) dan disinari cahaya (terang), catat rentang nilai minimum-maksimum yang terbaca

**Kode Program (LDR):**
```cpp
#define LDR_PIN 34

void setup() {
  Serial.begin(115200);
}

void loop() {
  int ldrRaw = analogRead(LDR_PIN);
  float ldrVoltage = ldrRaw * (3.3 / 4095.0);

  Serial.printf("LDR raw: %d | Tegangan: %.2fV\n", ldrRaw, ldrVoltage);
  delay(300);
}
```

**Penjelasan Kode:**
| Bagian | Penjelasan |
|---|---|
| `analogRead(LDR_PIN)` | Membaca nilai ADC 12-bit (0–4095) pada pin yang terhubung ke titik tengah pembagi tegangan LDR |
| `ldrVoltage = ldrRaw * (3.3 / 4095.0)` | Mengonversi nilai ADC menjadi tegangan (volt) berdasarkan referensi ADC 3.3V dan resolusi 12-bit |

---

### PERCOBAAN 2 — Sensor Kapasitif & Induktif

**Tujuan:**
Mahasiswa mampu memahami dan mengimplementasikan pembacaan sensor kapasitif (touch sensor bawaan ESP32) dan sensor induktif (hall effect sensor).

**Skema Rangkaian:**

| Komponen | Pin ESP32 | Keterangan |
|---|---|---|
| Capacitive touch (probe kabel) | GPIO 4 (T0) | Menggunakan fitur touch sensor bawaan ESP32 |
| Hall effect sensor module | GPIO 26 | Output digital |

**Langkah Kerja:**
1. Sambungkan kabel probe ke GPIO 4 (pin touch T0), uji fitur capacitive touch bawaan ESP32
2. Amati perubahan nilai `touchRead()` saat probe disentuh dibanding saat tidak disentuh, tentukan nilai threshold yang sesuai
3. Rangkai hall effect sensor pada GPIO 26, uji deteksi dengan mendekatkan magnet

**Kode Program (Capacitive Touch & Hall Effect):**
```cpp
#define TOUCH_PIN 4       // GPIO 4 = Touch0 (T0)
#define HALL_PIN 26
#define TOUCH_THRESHOLD 30 // sesuaikan berdasarkan hasil pengamatan

void setup() {
  Serial.begin(115200);
  pinMode(HALL_PIN, INPUT);
}

void loop() {
  int touchValue = touchRead(TOUCH_PIN);
  bool touched = touchValue < TOUCH_THRESHOLD;

  bool magnetDetected = (digitalRead(HALL_PIN) == LOW); // umumnya aktif LOW, cek datasheet modul

  Serial.printf("Touch: %d (%s) | Hall Effect: %s\n",
                touchValue, touched ? "TERSENTUH" : "idle",
                magnetDetected ? "MAGNET TERDETEKSI" : "idle");
  delay(300);
}
```

**Penjelasan Kode:**
| Bagian | Penjelasan |
|---|---|
| `touchRead(TOUCH_PIN)` | Fungsi bawaan ESP32 untuk membaca nilai kapasitansi pada pin touch — nilai semakin kecil saat disentuh (bukan digitalRead biasa) |
| `TOUCH_THRESHOLD` | Nilai ambang batas untuk menentukan status tersentuh/tidak, ditentukan berdasarkan hasil kalibrasi/pengamatan langsung |
| `digitalRead(HALL_PIN) == LOW` | Sebagian besar modul hall effect bersifat aktif LOW — perlu diverifikasi pada datasheet modul yang digunakan |

---

### PERCOBAAN 3 — Sensor Basis Lain (Ultrasonik & IR Obstacle)

**Tujuan:**
Mahasiswa mampu mengimplementasikan pembacaan sensor ultrasonik (basis akustik) dan sensor IR obstacle (basis optik).

**Skema Rangkaian:**

| Komponen | Pin ESP32 | Keterangan |
|---|---|---|
| HC-SR04 — Trig | GPIO 5 | Output dari ESP32 ke sensor |
| HC-SR04 — Echo | GPIO 18 | Input ke ESP32 (gunakan pembagi tegangan jika sensor 5V) |
| Sensor IR obstacle | GPIO 19 | Output digital |

**Langkah Kerja:**
1. Rangkai HC-SR04 sesuai skema, tulis program pengukuran jarak berbasis `pulseIn()`
2. Rangkai sensor IR obstacle, uji pembacaan status halangan
3. Amati kedua pembacaan secara bersamaan pada Serial Monitor, uji dengan berbagai jarak dan objek

**Kode Program (Ultrasonik HC-SR04 & IR Obstacle):**
```cpp
#define TRIG_PIN 5
#define ECHO_PIN 18
#define IR_PIN 19

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(IR_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);
}

float readDistanceCM() {
  // Pastikan TRIG LOW
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // Kirim pulsa 10 us
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Baca durasi pantulan
  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // timeout 30 ms

  if (duration == 0)
    return -1.0; // tidak ada objek dalam jangkauan

  // Konversi ke cm
  return duration * 0.0343 / 2.0;
}

void loop() {
  float distance = readDistanceCM();
  bool obstacle = (digitalRead(IR_PIN) == LOW); // umumnya aktif LOW

  if (distance < 0) {
    Serial.print("Jarak: tidak ada objek");
  } else {
    Serial.printf("Jarak: %.1f cm", distance);
  }
  Serial.printf(" | IR Obstacle: %s\n", obstacle ? "TERDETEKSI" : "idle");
  delay(100);
}
```

**Penjelasan Kode:**
| Bagian | Penjelasan |
|---|---|
| `pulseIn(ECHO_PIN, HIGH, 30000)` | Mengukur durasi (mikrodetik) pin ECHO berada pada kondisi HIGH, dengan batas timeout 30ms |
| `duration == 0` | Menandakan tidak ada pantulan yang diterima dalam batas timeout, sehingga dianggap tidak ada objek pada jangkauan sensor |
| `digitalRead(IR_PIN) == LOW` | Sebagian besar modul IR obstacle bersifat aktif LOW saat mendeteksi halangan |

---

### PERCOBAAN 4 — Aktuator Motor DC

**Tujuan:**
Mahasiswa mampu mengimplementasikan kontrol kecepatan dan arah putar motor DC menggunakan sinyal PWM.

**Skema Rangkaian:**

| Komponen | Pin ESP32 | Keterangan |
|---|---|---|
| Driver motor (ENA) | GPIO 25 | Sinyal PWM kecepatan |
| Driver motor (IN1) | GPIO 26 | Arah putar motor |
| Driver motor (IN2) | GPIO 27 | Arah putar motor |

**Langkah Kerja:**
1. Rangkai motor DC melalui driver (mis. L298N) sesuai skema, gunakan catu daya eksternal untuk motor (bukan 5V dari USB langsung)
2. Implementasikan kontrol PWM pada pin ENA untuk mengatur kecepatan, serta IN1/IN2 untuk mengatur arah (maju/mundur/berhenti)
3. Uji kendali motor melalui perintah teks pada Serial Monitor (`F` = maju, `B` = mundur, `S` = berhenti, angka 0–255 = atur PWM)
4. Amati perubahan kecepatan putar motor saat nilai PWM diubah

**Kode Program (Kontrol Motor DC via Serial):**
```cpp
#include <Arduino.h>

//==================== MOTOR ====================
#define ENA 25
#define IN1 26
#define IN2 27

const int pwmFreq = 1000;
const int pwmResolution = 8;

int motorSpeed = 150;

//================================================

void motorForward(uint8_t speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  ledcWrite(ENA, speed);
}

void motorBackward(uint8_t speed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  ledcWrite(ENA, speed);
}

void motorStop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  ledcWrite(ENA, 0);
}

//================================================

void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  ledcAttach(ENA, pwmFreq, pwmResolution); // LEDC API baru (Arduino-ESP32 core 3.x) — channel dikelola otomatis

  Serial.println("--------------------------------");
  Serial.println("ESP32 Motor DC Test");
  Serial.println("Perintah: F=Forward, B=Backward, S=Stop, 0-255=PWM");
  Serial.println("--------------------------------");

  motorForward(motorSpeed);
}

//================================================

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd == "F") {
      motorForward(motorSpeed);
      Serial.println("Forward");
    } else if (cmd == "B") {
      motorBackward(motorSpeed);
      Serial.println("Backward");
    } else if (cmd == "S") {
      motorStop();
      Serial.println("Stop");
    } else {
      int pwm = cmd.toInt();
      if (pwm >= 0 && pwm <= 255) {
        motorSpeed = pwm;
        motorForward(motorSpeed);
        Serial.print("PWM = ");
        Serial.println(motorSpeed);
      }
    }
  }
}
```

**Penjelasan Kode:**
| Bagian | Penjelasan |
|---|---|
| `ledcAttach(ENA, pwmFreq, pwmResolution)` | Mengonfigurasi sekaligus menghubungkan PWM (frekuensi 1kHz, resolusi 8-bit) langsung ke pin ENA — pada API LEDC baru, channel dialokasikan otomatis sehingga tidak perlu dikelola manual seperti `ledcSetup()`/`ledcAttachPin()` pada API lama |
| `ledcWrite(ENA, speed)` | Mengatur nilai duty cycle PWM langsung berdasarkan nomor pin (bukan nomor channel seperti pada API lama) |
| `motorForward()` / `motorBackward()` / `motorStop()` | Mengatur kombinasi IN1/IN2 untuk menentukan arah putar, sekaligus nilai PWM untuk kecepatan |
| `Serial.readStringUntil('\n')` | Membaca perintah teks dari Serial Monitor untuk mengendalikan motor secara interaktif |

---

### PERCOBAAN 5 — Aktuator Motor Stepper

**Tujuan:**
Mahasiswa mampu mengimplementasikan kontrol motor stepper menggunakan sinyal step melalui driver motor stepper.

**Skema Rangkaian:**

| Komponen | Pin ESP32 | Keterangan |
|---|---|---|
| Driver stepper — STEP | GPIO 18 | Setiap pulsa menggerakkan motor satu step |
| Driver stepper — EN | GPIO 19 | Enable driver (aktif LOW pada sebagian besar driver) |
| Driver stepper — RESET | GPIO 21 | Ditarik HIGH agar driver aktif normal |

**Langkah Kerja:**
1. Rangkai motor stepper melalui driver (mis. A4988/DRV8825) sesuai skema, gunakan catu daya eksternal sesuai spesifikasi motor
2. Implementasikan fungsi untuk menggerakkan motor sejumlah step tertentu (mis. setara 90°)
3. Uji program dan amati pergerakan motor stepper, hitung apakah jumlah step yang diberikan sesuai dengan sudut putar yang diharapkan
4. Sebagai latihan tambahan, ukur waktu antar pulsa (`delayMicroseconds`) minimum yang masih membuat motor berputar dengan lancar (tanpa "kehilangan step")

**Kode Program (Kontrol Motor Stepper):**
```cpp
#define STEP_PIN 18
#define EN_PIN   19
#define RESET_PIN 21

void move90() {
  for (int i = 0; i < 50; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(1000);

    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(1000);
  }
}

void setup() {
  pinMode(STEP_PIN, OUTPUT);
  pinMode(EN_PIN, OUTPUT);
  pinMode(RESET_PIN, OUTPUT);

  digitalWrite(EN_PIN, HIGH);
  digitalWrite(RESET_PIN, HIGH);
}

void loop() {
  move90();
  delay(2000);
}
```

**Penjelasan Kode:**
| Bagian | Penjelasan |
|---|---|
| `move90()` | Mengirim 50 pulsa STEP berturut-turut — jumlah step untuk mencapai sudut tertentu tergantung spesifikasi motor (step/putaran) dan mode microstepping driver |
| `delayMicroseconds(1000)` | Jeda antar pulsa STEP; semakin kecil nilainya, semakin cepat motor berputar, namun terlalu kecil dapat menyebabkan motor kehilangan step |
| `digitalWrite(EN_PIN, HIGH)` | Perlu diverifikasi pada datasheet driver — sebagian driver justru aktif LOW pada pin EN, sehingga logikanya perlu disesuaikan |
| `digitalWrite(RESET_PIN, HIGH)` | Menonaktifkan mode reset agar driver beroperasi normal |

> **Catatan:** Kode dasar ini hanya menggerakkan motor pada satu arah. Sebagai pengembangan, tambahkan pin **DIR** pada driver untuk mengatur arah putaran (bandingkan dengan pendekatan IN1/IN2 pada motor DC di Percobaan 4).

---

### PERCOBAAN 6 — Aktuator Motor Servo

**Tujuan:**
Mahasiswa mampu mengimplementasikan kontrol posisi sudut motor servo menggunakan sinyal PWM melalui library ESP32Servo.

**Skema Rangkaian:**

| Komponen | Pin ESP32 | Keterangan |
|---|---|---|
| Servo motor (sinyal) | GPIO 13 | Sinyal PWM servo (50Hz) |

**Langkah Kerja:**
1. Rangkai servo motor sesuai skema (sinyal ke GPIO 13, VCC dan GND ke catu daya yang sesuai)
2. Implementasikan program untuk menggerakkan servo secara bertahap dari 0° hingga 180°
3. Amati kehalusan gerakan servo, bandingkan pergerakan bertahap (step kecil) dengan perpindahan langsung (mis. `myServo.write(180)` langsung dari posisi 0°)

**Kode Program (Kontrol Motor Servo):**
```cpp
#include <ESP32Servo.h>

#define SERVO_PIN 13
Servo myServo;

void setup() {
  Serial.begin(115200);
  myServo.attach(SERVO_PIN);
}

void loop() {
  // Kontrol posisi servo 0 - 180 derajat secara bertahap
  for (int angle = 0; angle <= 180; angle += 10) {
    myServo.write(angle);
    Serial.printf("Sudut: %d\n", angle);
    delay(150);
  }

  delay(1000);

  for (int angle = 180; angle >= 0; angle -= 10) {
    myServo.write(angle);
    Serial.printf("Sudut: %d\n", angle);
    delay(150);
  }

  delay(1000);
}
```

**Penjelasan Kode:**
| Bagian | Penjelasan |
|---|---|
| `myServo.attach(SERVO_PIN)` | Menghubungkan objek `Servo` ke pin PWM yang digunakan, library akan mengatur frekuensi 50Hz secara otomatis |
| `myServo.write(angle)` | Mengatur posisi sudut servo (0–180 derajat) — library mengonversi nilai sudut menjadi lebar pulsa yang sesuai |

**Tugas Akhir Modul 2:**
Rancang sistem yang menggabungkan minimal satu sensor dari tiap basis pengukuran (resistif, kapasitif/induktif, dan basis lain) dengan salah satu aktuator (motor DC, stepper, atau servo) sebagai respons — misalnya indikator jarak menggunakan servo sebagai penunjuk (mirip jarum meter) berdasarkan pembacaan sensor ultrasonik, atau sistem buka-tutup otomatis menggunakan motor stepper yang dipicu oleh sensor hall effect/touch.

---

## F. Format Laporan Praktikum

1. **Cover** — judul modul, nama, NIM, kelas
2. **Tujuan Praktikum**
3. **Dasar Teori Singkat** (parafrase, bukan salinan modul)
4. **Alat dan Bahan**
5. **Langkah Kerja & Skema Rangkaian** (sertakan foto rangkaian nyata untuk tiap percobaan)
6. **Kode Program** (lengkap, dengan komentar)
7. **Hasil dan Pembahasan** (data pengamatan/kalibrasi tiap sensor, jawaban pertanyaan analisis, hasil tugas akhir modul)
8. **Kesimpulan**
9. **Lampiran** (foto/video demo, dokumentasi tambahan)

---

## G. Rubrik Penilaian

| Aspek | Bobot | Kriteria |
|---|---|---|
| Implementasi sensor resistif (LDR) | 15% | Pembacaan ADC dan konversi tegangan berjalan benar |
| Implementasi sensor kapasitif & induktif (touch, hall effect) | 15% | Touch sensor dan hall effect terbaca dengan benar |
| Implementasi sensor basis lain (ultrasonik, IR) | 10% | Pengukuran jarak dan deteksi obstacle berjalan benar |
| Implementasi aktuator motor DC | 15% | Kontrol kecepatan dan arah motor DC berjalan sesuai spesifikasi |
| Implementasi aktuator motor stepper | 15% | Motor stepper bergerak sejumlah step sesuai program |
| Implementasi aktuator motor servo | 10% | Kontrol posisi sudut servo berjalan sesuai spesifikasi |
| Tugas akhir modul (integrasi sensor + aktuator) | 10% | Sistem gabungan berfungsi sesuai rancangan |
| Laporan & analisis | 10% | Kelengkapan, kedalaman analisis, kerapian dokumentasi |

---

## H. Referensi
1. Espressif Systems, *ESP32 Technical Reference Manual*
2. Espressif Systems, *ESP32 Arduino Core Documentation — Analog, Touch, LEDC*, https://docs.espressif.com/projects/arduino-esp32/
3. Datasheet HC-SR04 Ultrasonic Sensor
4. Datasheet Hall Effect Sensor Module (mis. A3144/KY-003)
5. Datasheet Driver Motor Stepper A4988/DRV8825
6. ESP32Servo Library Documentation, https://github.com/madhephaestus/ESP32Servo
7. PlatformIO Documentation, https://docs.platformio.org/

---

## I. Kumpulan Pertanyaan

### Pertanyaan Pra-Praktikum
1. Mengapa sensor resistif seperti LDR perlu dirangkai sebagai pembagi tegangan sebelum dapat dibaca oleh ADC mikrokontroler?
2. Jelaskan perbedaan prinsip kerja antara sensor kapasitif dan sensor induktif dalam mendeteksi suatu objek/besaran fisik.
3. Apa yang dimaksud dengan duty cycle pada sinyal PWM, dan bagaimana pengaruhnya terhadap kecepatan putar motor DC?
4. Jelaskan perbedaan mendasar antara cara kerja motor DC, motor stepper, dan motor servo dalam merespons sinyal kontrol dari mikrokontroler

### Pertanyaan/Analisis Percobaan 1 — Sensor Resistif
1. Jelaskan mengapa nilai ADC dari LDR perlu dikonversi menjadi tegangan agar lebih bermakna secara fisik
2. Bagaimana pengaruh nilai resistor tetap pada rangkaian pembagi tegangan terhadap rentang nilai ADC yang terbaca dari LDR?
3. Apa yang akan terjadi pada rentang nilai ADC yang terbaca jika posisi LDR dan resistor tetap pada rangkaian pembagi tegangan ditukar?

### Pertanyaan/Analisis Percobaan 2 — Sensor Kapasitif & Induktif
1. Mengapa nilai `touchRead()` pada ESP32 justru semakin *kecil* saat pin disentuh, alih-alih semakin besar?
2. Jelaskan mengapa hall effect sensor module yang digunakan bersifat aktif LOW, dan bagaimana cara memastikan hal ini pada modul yang digunakan
3. Sebutkan satu aplikasi nyata yang lebih cocok menggunakan sensor induktif (hall effect) dibandingkan sensor kapasitif (touch), dan jelaskan alasannya

### Pertanyaan/Analisis Percobaan 3 — Sensor Basis Lain
1. Jelaskan mengapa perhitungan jarak pada sensor ultrasonik perlu dibagi dua (`/2.0`) pada rumus konversi waktu ke jarak
2. Apa yang terjadi pada hasil pembacaan `pulseIn()` jika objek berada di luar jangkauan maksimum sensor ultrasonik? Jelaskan peran parameter timeout dan nilai `duration == 0` pada fungsi `readDistanceCM()`
3. Sebutkan satu kelebihan dan satu keterbatasan sensor ultrasonik dibandingkan sensor IR obstacle untuk aplikasi deteksi jarak

### Pertanyaan/Analisis Percobaan 4 — Aktuator Motor DC
1. Jelaskan bagaimana kombinasi nilai IN1 dan IN2 menentukan arah putar motor DC pada program yang diimplementasikan
2. Mengapa nilai PWM (ENA) tetap diperlukan meskipun arah putar sudah ditentukan oleh IN1/IN2?
3. Mengapa motor DC pada praktikum ini disarankan menggunakan catu daya eksternal, bukan langsung dari pin 5V/3.3V ESP32?

### Pertanyaan/Analisis Percobaan 5 — Aktuator Motor Stepper
1. Jelaskan perbedaan mendasar antara cara kerja motor DC (Percobaan 4) dan motor stepper dalam merespons sinyal kontrol dari mikrokontroler
2. Apa yang dimaksud dengan "kehilangan step" (missed step) pada motor stepper, dan faktor apa pada program yang dapat menyebabkannya?
3. Jelaskan fungsi pin EN dan RESET pada driver motor stepper yang digunakan

### Pertanyaan/Analisis Percobaan 6 — Aktuator Motor Servo
1. Jelaskan bagaimana lebar pulsa (pulse width) pada sinyal kontrol servo menentukan posisi sudutnya
2. Bandingkan hasil pengamatan pergerakan servo secara bertahap dengan perpindahan langsung — jelaskan penyebab perbedaan yang teramati
3. Mengapa kontrol servo tidak memerlukan pin arah terpisah seperti pada motor DC atau motor stepper?
