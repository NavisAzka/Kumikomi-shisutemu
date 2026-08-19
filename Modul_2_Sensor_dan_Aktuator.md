# MODUL 2
# SENSOR BERDASARKAN BASIS PENGUKURAN & AKTUATOR MOTOR

**Mata Kuliah:** Praktikum Mikrokontroler & Embedded System
**Alokasi Waktu:** 5 x Percobaan (@ 100–150 menit)
**Platform:** ESP32 (Framework Arduino)
**IDE:** VSCode + PlatformIO

> **Catatan:** Modul ini melanjutkan penggunaan ESP32 dengan framework Arduino seperti pada Percobaan 3–5 Modul 1. Langkah instalasi VSCode, PlatformIO, dan driver USB-to-Serial tidak diulang di sini — lihat **[setup_vscode_platformio.md](setup_vscode_platformio.md)**.

---

## Daftar Isi
- [A. Capaian Pembelajaran](#a-capaian-pembelajaran)
- [B. Alat dan Bahan](#b-alat-dan-bahan)
- [C. Dasar Teori](#c-dasar-teori)
  - [C.1 ADC pada ESP32](#c1-adc-pada-esp32)
  - [C.2 Sensor Resistif — Prinsip Pembagi Tegangan](#c2-sensor-resistif--prinsip-pembagi-tegangan)
  - [C.3 Sensor Kapasitif](#c3-sensor-kapasitif)
  - [C.4 Sensor Induktif](#c4-sensor-induktif)
  - [C.5 Sensor Basis Lain (Akustik & Optik)](#c5-sensor-basis-lain-akustik--optik)
  - [C.6 PWM (Pulse Width Modulation) — Kontrol Kecepatan Motor DC](#c6-pwm-pulse-width-modulation--kontrol-kecepatan-motor-dc)
  - [C.7 Kontrol Posisi Motor Servo](#c7-kontrol-posisi-motor-servo)
  - [C.8 Motor Stepper](#c8-motor-stepper)
- [D. Persiapan Sebelum Praktikum](#d-persiapan-sebelum-praktikum)
- [E. Kegiatan Praktikum](#e-kegiatan-praktikum)
  - [PERCOBAAN 1 — Sensor Resistif, Kapasitif, dan Induktif](#percobaan-1--sensor-resistif-kapasitif-dan-induktif)
  - [PERCOBAAN 2 — Sensor Basis Lain (Ultrasonik & IR Obstacle)](#percobaan-2--sensor-basis-lain-ultrasonik--ir-obstacle)
  - [PERCOBAAN 3 — Aktuator Motor DC](#percobaan-3--aktuator-motor-dc)
  - [PERCOBAAN 4 — Aktuator Motor Stepper](#percobaan-4--aktuator-motor-stepper)
  - [PERCOBAAN 5 — Aktuator Motor Servo](#percobaan-5--aktuator-motor-servo)
- [F. Tugas Pasca Praktikum (Simulasi Wokwi)](#f-tugas-pasca-praktikum-simulasi-wokwi)
- [G. Referensi](#g-referensi)

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
| 7 | Modul touch sensor TTP223 | Capacitive touch sensor, output digital | 1 |
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

![Gambar 1: Foto/diagram modul LDR beserta rangkaian pembagi tegangan pada breadboard](img/modul_ldr_pembagi_tegangan.png)

### C.3 Sensor Kapasitif
Sensor kapasitif mendeteksi besaran fisik (sentuhan, kelembapan, jarak dekat) melalui **perubahan nilai kapasitansi**. Pada praktikum ini digunakan modul **touch sensor TTP223** — modul berbasis IC TTP223 yang sudah mengintegrasikan rangkaian deteksi kapasitansi dan pembanding ambang batas (threshold) secara internal, sehingga cukup menghasilkan **output digital HIGH/LOW** siap pakai (umumnya aktif HIGH saat pad disentuh) tanpa perlu kalibrasi nilai analog secara manual — berbeda dengan fitur *touch* bawaan ESP32 (`touchRead()`) yang mengembalikan nilai mentah dan memerlukan penentuan threshold sendiri. Sensor kapasitif lain (mis. capacitive soil moisture, capacitive proximity) bekerja dengan prinsip serupa — mengukur perubahan kapasitansi pada elektroda sensor — namun umumnya berbentuk modul terpisah dengan output analog.

![Gambar 2: Foto modul touch sensor TTP223 beserta label pin VCC/GND/OUT](img/modul_touch_ttp223.png)

### C.4 Sensor Induktif
Sensor induktif mendeteksi objek logam atau perubahan medan magnet melalui **perubahan induktansi/medan magnet**. Pada praktikum ini digunakan **hall effect sensor**, yang mendeteksi keberadaan/kekuatan medan magnet secara langsung — umum digunakan untuk mendeteksi posisi magnet atau kecepatan putar (bersama magnet pada objek berputar). Contoh sensor induktif lain adalah *inductive proximity sensor*, yang menghasilkan medan elektromagnetik osilasi dan mendeteksi benda logam melalui redaman (eddy current) pada medan tersebut.

![Gambar 3: Foto modul hall effect sensor (mis. A3144/KY-003) beserta magnet uji](img/modul_hall_effect.png)

### C.5 Sensor Basis Lain (Akustik & Optik)
- **Ultrasonik (akustik):** mengukur jarak berdasarkan waktu tempuh gelombang suara (pulsa dipancarkan, dipantulkan objek, lalu diterima kembali); jarak dihitung dari selisih waktu dan kecepatan suara di udara
- **Inframerah/optik:** mendeteksi objek berdasarkan pantulan cahaya inframerah; umum digunakan sebagai sensor jarak dekat/obstacle dengan output digital

![Gambar 4: Foto modul sensor ultrasonik HC-SR04 (label pin Trig/Echo) berdampingan dengan modul sensor IR obstacle](img/sensor_ultrasonik_ir.png)

### C.6 PWM (Pulse Width Modulation) — Kontrol Kecepatan Motor DC
PWM adalah teknik menghasilkan sinyal digital yang menyerupai sinyal analog dengan mengatur **duty cycle** (persentase waktu sinyal HIGH dalam satu periode). Semakin besar duty cycle, semakin besar "rata-rata" tegangan yang dirasakan oleh beban (mis. motor DC), sehingga kecepatan putarnya meningkat. Pada ESP32 framework Arduino, PWM diakses melalui API **LEDC** berbasis **channel**: `ledcSetup(channel, freq, resolution)` untuk mengonfigurasi sebuah channel PWM (frekuensi & resolusi), `ledcAttachPin(pin, channel)` untuk menghubungkan channel tersebut ke pin fisik, dan `ledcWrite(channel, duty)` untuk mengatur duty cycle-nya berdasarkan nomor channel (bukan nomor pin). Arah putar motor DC diatur secara terpisah melalui driver motor (mis. L298N) menggunakan dua pin digital (IN1/IN2).

> **Catatan versi:** Seluruh contoh kode pada modul ini (dan modul-modul lain dalam rangkaian praktikum) menggunakan **platform PlatformIO resmi `espressif32`** tanpa mengunci versi khusus, yang secara default membawa **Arduino-ESP32 core versi 2.0.x**. API LEDC berbasis channel (`ledcSetup`/`ledcAttachPin`) adalah API yang tersedia pada core versi ini. Core versi 3.x (dengan API LEDC berbasis pin seperti `ledcAttach()`) memerlukan platform komunitas terpisah (mis. fork *pioarduino*) dan **tidak dibahas** pada praktikum ini agar tetap konsisten dan kompatibel dengan library lain (mis. ESP32Servo) yang digunakan di modul-modul ini.

![Gambar 5: Grafik sinyal PWM dengan beberapa nilai duty cycle berbeda (mis. 25%, 50%, 75%), menunjukkan hubungan duty cycle dengan tegangan rata-rata](img/grafik_pwm_duty_cycle.png)

### C.7 Kontrol Posisi Motor Servo
Motor servo juga dikendalikan menggunakan sinyal PWM, namun dengan prinsip yang berbeda dari kontrol kecepatan motor DC — pada servo, **lebar pulsa (pulse width)** itu sendiri yang menentukan posisi sudut, bukan rata-rata duty cycle. Umumnya sinyal kontrol servo memiliki periode 20ms (frekuensi 50Hz), dengan lebar pulsa sekitar 1ms merepresentasikan sudut 0° dan 2ms merepresentasikan sudut 180°. Pada framework Arduino, detail ini diabstraksi oleh library seperti **ESP32Servo**, sehingga cukup memanggil fungsi `.write(angle)` untuk menggerakkan servo ke sudut tertentu.

![Gambar 6: Diagram lebar pulsa sinyal servo (1ms–2ms dalam periode 20ms) beserta sudut yang dihasilkan (0°–180°)](img/diagram_pulsa_servo.png)

### C.8 Motor Stepper
Berbeda dengan motor DC yang berputar kontinu, motor stepper bergerak dalam **langkah-langkah diskret (step)** sesuai jumlah pulsa yang diberikan — misalnya 1.8° per step pada motor stepper standar (200 step per putaran penuh). Motor stepper dikendalikan melalui **driver motor stepper** (mis. A4988, DRV8825), yang menerima sinyal:
- **STEP:** setiap pulsa (transisi LOW→HIGH) menyebabkan motor berputar satu step
- **DIR:** menentukan arah putaran motor
- **EN (Enable):** mengaktifkan/menonaktifkan driver — saat dinonaktifkan, motor dapat diputar bebas secara manual
- **RESET/SLEEP:** beberapa driver memiliki pin ini untuk mereset atau menonaktifkan mode tidur driver, umumnya perlu ditarik HIGH agar driver aktif normal

![Gambar 7: Diagram wiring motor stepper NEMA17 ke driver A4988/DRV8825, beserta pin STEP/DIR/EN/RESET ke ESP32](img/wiring_motor_stepper.png)

---

## D. Persiapan Sebelum Praktikum

1. Pastikan PlatformIO sudah terinstal (lihat **[setup_vscode_platformio.md](setup_vscode_platformio.md)**)
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
4. Untuk Percobaan 5 (Motor Servo), tambahkan library **ESP32Servo** melalui PlatformIO Library Manager atau tambahkan pada `platformio.ini`:
   ```ini
   lib_deps = madhephaestus/ESP32Servo@^3.0.0
   ```

---

## E. Kegiatan Praktikum

### PERCOBAAN 1 — Sensor Resistif, Kapasitif, dan Induktif

**Tujuan:**
Mahasiswa mampu memahami dan mengimplementasikan pembacaan sensor resistif (LDR melalui ADC), kapasitif (touch sensor TTP223), dan induktif (hall effect sensor).

**Skema Rangkaian:**

| Komponen | Pin ESP32 | Keterangan |
|---|---|---|
| LDR (pembagi tegangan dengan resistor 10kΩ) | GPIO 34 | Titik tengah pembagi tegangan ke ADC |
| Modul TTP223 — OUT | GPIO 4 | Output digital, umumnya aktif HIGH saat pad disentuh |
| Modul TTP223 — VCC/GND | 3.3V, GND | Sesuai datasheet modul |
| Hall effect sensor module | GPIO 26 | Output digital |

**`platformio.ini`:**
```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
```

**Langkah Kerja:**
1. Rangkai LDR sebagai pembagi tegangan sesuai skema, hubungkan titik tengah ke GPIO 34
2. Rangkai modul TTP223 (VCC, GND, dan OUT ke GPIO 4)
3. Rangkai hall effect sensor module pada GPIO 26
4. Upload program gabungan di bawah, amati ketiga pembacaan sekaligus pada Serial Monitor
5. Uji LDR dengan menutup/menyinari sensor, uji TTP223 dengan menyentuh pad sensor, dan uji hall effect dengan mendekatkan magnet — catat rentang/status hasil masing-masing

**Kode Program (LDR, TTP223, & Hall Effect):**
```cpp
#define LDR_PIN 34
#define TOUCH_PIN 4  // OUT modul TTP223
#define HALL_PIN 26

void setup() {
  Serial.begin(115200);
  pinMode(TOUCH_PIN, INPUT);
  pinMode(HALL_PIN, INPUT);
}

void loop() {
  int ldrRaw = analogRead(LDR_PIN);
  float ldrVoltage = ldrRaw * (3.3 / 4095.0);

  bool touched = (digitalRead(TOUCH_PIN) == HIGH); // TTP223 umumnya aktif HIGH
  bool magnetDetected = (digitalRead(HALL_PIN) == LOW); // umumnya aktif LOW, cek datasheet modul

  Serial.printf("LDR: %d (%.2fV) | Touch (TTP223): %s | Hall Effect: %s\n",
                ldrRaw, ldrVoltage,
                touched ? "TERSENTUH" : "idle",
                magnetDetected ? "MAGNET TERDETEKSI" : "idle");
  delay(300);
}
```

**Penjelasan Kode:**
| Bagian | Penjelasan |
|---|---|
| `analogRead(LDR_PIN)` | Membaca nilai ADC 12-bit (0–4095) pada pin yang terhubung ke titik tengah pembagi tegangan LDR |
| `ldrVoltage = ldrRaw * (3.3 / 4095.0)` | Mengonversi nilai ADC menjadi tegangan (volt) berdasarkan referensi ADC 3.3V dan resolusi 12-bit |
| `digitalRead(TOUCH_PIN) == HIGH` | Modul TTP223 sudah mengeluarkan output digital siap pakai — tidak perlu `touchRead()` maupun threshold manual seperti fitur touch bawaan ESP32 |
| `digitalRead(HALL_PIN) == LOW` | Sebagian besar modul hall effect bersifat aktif LOW — perlu diverifikasi pada datasheet modul yang digunakan |

---

### PERCOBAAN 2 — Sensor Basis Lain (Ultrasonik & IR Obstacle)

**Tujuan:**
Mahasiswa mampu mengimplementasikan pembacaan sensor ultrasonik (basis akustik) dan sensor IR obstacle (basis optik).

**Skema Rangkaian:**

| Komponen | Pin ESP32 | Keterangan |
|---|---|---|
| HC-SR04 — Trig | GPIO 5 | Output dari ESP32 ke sensor |
| HC-SR04 — Echo | GPIO 18 | Input ke ESP32 (gunakan pembagi tegangan jika sensor 5V) |
| Sensor IR obstacle | GPIO 19 | Output digital |

**`platformio.ini`:**
```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
```

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

### PERCOBAAN 3 — Aktuator Motor DC

**Tujuan:**
Mahasiswa mampu mengimplementasikan kontrol kecepatan dan arah putar motor DC menggunakan sinyal PWM.

**Skema Rangkaian:**

| Komponen | Pin ESP32 | Keterangan |
|---|---|---|
| Driver motor (ENA) | GPIO 25 | Sinyal PWM kecepatan |
| Driver motor (IN1) | GPIO 26 | Arah putar motor |
| Driver motor (IN2) | GPIO 27 | Arah putar motor |

**`platformio.ini`:**
```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
```

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

const int pwmChannel = 0;
const int pwmFreq = 1000;
const int pwmResolution = 8;

int motorSpeed = 150;

//================================================

void motorForward(uint8_t speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  ledcWrite(pwmChannel, speed);
}

void motorBackward(uint8_t speed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  ledcWrite(pwmChannel, speed);
}

void motorStop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  ledcWrite(pwmChannel, 0);
}

//================================================

void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  ledcSetup(pwmChannel, pwmFreq, pwmResolution); // konfigurasi channel PWM (core 2.x)
  ledcAttachPin(ENA, pwmChannel);                // hubungkan channel ke pin ENA

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
| `ledcSetup(pwmChannel, pwmFreq, pwmResolution)` | Mengonfigurasi channel PWM 0 dengan frekuensi 1kHz dan resolusi 8-bit (nilai duty 0–255) |
| `ledcAttachPin(ENA, pwmChannel)` | Menghubungkan channel PWM yang telah dikonfigurasi ke pin fisik ENA |
| `ledcWrite(pwmChannel, speed)` | Mengatur nilai duty cycle PWM berdasarkan nomor **channel** (bukan nomor pin) — karakteristik API LEDC berbasis channel pada core 2.x |
| `motorForward()` / `motorBackward()` / `motorStop()` | Mengatur kombinasi IN1/IN2 untuk menentukan arah putar, sekaligus nilai PWM untuk kecepatan |
| `Serial.readStringUntil('\n')` | Membaca perintah teks dari Serial Monitor untuk mengendalikan motor secara interaktif |

---

### PERCOBAAN 4 — Aktuator Motor Stepper

**Tujuan:**
Mahasiswa mampu mengimplementasikan kontrol motor stepper menggunakan sinyal step melalui driver motor stepper.

**Skema Rangkaian:**

| Komponen | Pin ESP32 | Keterangan |
|---|---|---|
| Driver stepper — STEP | GPIO 18 | Setiap pulsa menggerakkan motor satu step |
| Driver stepper — EN | GPIO 19 | Enable driver (aktif LOW pada sebagian besar driver) |
| Driver stepper — RESET | GPIO 21 | Ditarik HIGH agar driver aktif normal |

**`platformio.ini`:**
```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
```

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

> **Catatan:** Kode dasar ini hanya menggerakkan motor pada satu arah. Sebagai pengembangan, tambahkan pin **DIR** pada driver untuk mengatur arah putaran (bandingkan dengan pendekatan IN1/IN2 pada motor DC di Percobaan 3).

---

### PERCOBAAN 5 — Aktuator Motor Servo

**Tujuan:**
Mahasiswa mampu mengimplementasikan kontrol posisi sudut motor servo menggunakan sinyal PWM melalui library ESP32Servo.

**Skema Rangkaian:**

| Komponen | Pin ESP32 | Keterangan |
|---|---|---|
| Servo motor (sinyal) | GPIO 13 | Sinyal PWM servo (50Hz) |

**`platformio.ini`:**
```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps = madhephaestus/ESP32Servo@^3.0.0
```

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

## F. Tugas Pasca Praktikum (Simulasi Wokwi)

[Wokwi](https://wokwi.com) menyediakan part siap pakai untuk ESP32 beserta LDR, potensiometer, HC-SR04, servo motor, dan motor DC — cukup lengkap untuk mensimulasikan sebagian besar rangkaian pada modul ini tanpa hardware fisik. Kerjakan tugas berikut **setelah** kegiatan praktikum selesai.

> **Catatan:** Modul touch sensor TTP223 dan hall effect sensor kemungkinan belum tersedia sebagai part bawaan Wokwi. Sebagai gantinya, gunakan **potensiometer** atau **slide switch virtual** untuk mensimulasikan sinyal digital HIGH/LOW pengganti kedua sensor tersebut pada bagian yang membutuhkannya.

**Tugas 1 — Indikator Jarak Otomatis (Wokwi):**
1. Buat project Wokwi baru dengan board **ESP32**, rangkai sensor **HC-SR04** dan **servo motor**
2. Implementasikan program yang membaca jarak dari HC-SR04 (Percobaan 2), lalu memetakan (`map()`) nilai jarak tersebut menjadi sudut servo (0°–180°) — semakin dekat objek, semakin besar sudut servo (meniru jarum penunjuk pada meter analog)
3. Uji dengan menggeser posisi objek virtual di depan sensor HC-SR04 pada simulator, amati apakah pergerakan servo responsif dan sesuai ekspektasi
4. Sebagai pengembangan, tambahkan LDR yang mengatur kecepatan "kedipan" LED indikator (semakin gelap, semakin cepat berkedip) berjalan bersamaan dengan sistem indikator jarak di atas

**Tugas 2 — Eksplorasi Mandiri:**
Ganti servo pada Tugas 1 dengan motor DC (via driver, kendalikan menggunakan PWM) sehingga kecepatan putar motor merepresentasikan jarak objek, alih-alih posisi sudut. Bandingkan kelebihan/kekurangan representasi jarak melalui sudut (servo) vs kecepatan (motor DC).

**Pengumpulan:** Sertakan link project Wokwi (mode *share*, pastikan visibility public/unlisted) beserta laporan singkat pada berkas terpisah.

---

## G. Referensi
1. Espressif Systems, *ESP32 Technical Reference Manual*
2. Espressif Systems, *ESP32 Arduino Core Documentation — Analog, Touch, LEDC*, https://docs.espressif.com/projects/arduino-esp32/
3. Datasheet HC-SR04 Ultrasonic Sensor
4. Datasheet Hall Effect Sensor Module (mis. A3144/KY-003)
5. Datasheet Driver Motor Stepper A4988/DRV8825
6. ESP32Servo Library Documentation, https://github.com/madhephaestus/ESP32Servo
7. PlatformIO Documentation, https://docs.platformio.org/
