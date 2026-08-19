# MODUL 5
# PEMROSESAN DATA SENSOR & DASAR SISTEM KONTROL (FILTERING & PID)

**Mata Kuliah:** Praktikum Mikrokontroler & Embedded System
**Alokasi Waktu:** 5 x Percobaan (@ 100–150 menit)
**Platform:** ESP32 (Framework Arduino)
**IDE:** VSCode + PlatformIO

> **Catatan:** Modul ini merupakan modul kapstone yang menggabungkan **Modul 2** (motor DC + PWM) dan **Modul 4 Percobaan 2** (quadrature encoder via external interrupt). Pastikan rangkaian motor+encoder+driver dari kedua modul tersebut sudah berfungsi sebelum memulai modul ini. Seluruh contoh kode menggunakan platform PlatformIO resmi `espressif32` (Arduino-ESP32 **core versi 2.0.x**), konsisten dengan Modul 2–4, termasuk API LEDC berbasis channel (`ledcSetup`/`ledcAttachPin`/`ledcWrite`) untuk kontrol PWM motor DC.

---

## Daftar Isi
- [A. Capaian Pembelajaran](#a-capaian-pembelajaran)
- [B. Alat dan Bahan](#b-alat-dan-bahan)
- [C. Dasar Teori](#c-dasar-teori)
  - [C.1 Dari Pulsa Encoder ke RPM](#c1-dari-pulsa-encoder-ke-rpm)
  - [C.2 Filtering Sederhana — Low-Pass Filter Alpha](#c2-filtering-sederhana--low-pass-filter-alpha)
  - [C.3 Filtering Lanjut — Kalman Filter](#c3-filtering-lanjut--kalman-filter)
  - [C.4 Sistem Kontrol Closed-Loop](#c4-sistem-kontrol-closed-loop)
  - [C.5 Kontrol PID](#c5-kontrol-pid)
- [D. Persiapan Sebelum Praktikum](#d-persiapan-sebelum-praktikum)
- [E. Kegiatan Praktikum](#e-kegiatan-praktikum)
  - [PERCOBAAN 1 — Dari Pulsa ke RPM (Konversi & Kalibrasi)](#percobaan-1--dari-pulsa-ke-rpm-konversi--kalibrasi)
  - [PERCOBAAN 2 — Filtering Sederhana dengan Alpha (Low-Pass Filter)](#percobaan-2--filtering-sederhana-dengan-alpha-low-pass-filter)
  - [PERCOBAAN 3 — Filtering dengan Kalman Filter](#percobaan-3--filtering-dengan-kalman-filter)
  - [PERCOBAAN 4 — Kontrol Closed-Loop: On-Off vs Proportional (P)](#percobaan-4--kontrol-closed-loop-on-off-vs-proportional-p)
  - [PERCOBAAN 5 — Kontrol PID Lengkap & Tuning](#percobaan-5--kontrol-pid-lengkap--tuning)
- [F. Tugas Pasca Praktikum (Simulasi Wokwi)](#f-tugas-pasca-praktikum-simulasi-wokwi)
- [G. Referensi](#g-referensi)

---

## A. Capaian Pembelajaran

Setelah menyelesaikan Modul 5, praktikan mampu:
1. Mengonversi data pulsa mentah dari encoder menjadi kecepatan putar (RPM), termasuk kalibrasi pulsa per putaran
2. Menerapkan filtering sederhana (low-pass filter berbasis alpha) untuk mengurangi noise pada sinyal RPM
3. Menerapkan Kalman filter sebagai metode filtering alternatif, serta membandingkannya dengan filter alpha
4. Menjelaskan konsep sistem kontrol closed-loop, serta mengimplementasikan kontrol on-off dan Proportional (P)
5. Mengimplementasikan kontrol PID lengkap untuk mengatur kecepatan motor DC berdasarkan target RPM, serta melakukan tuning parameter

---

## B. Alat dan Bahan

| No | Nama Komponen | Spesifikasi | Jumlah |
|---|---|---|---|
| 1 | Board ESP32 DevKit | ESP32 DevKit v1 | 1 |
| 2 | Motor DC + encoder magnetik quadrature | modul dengan konektor JST 6-pin, mis. JGB37-520 (sama seperti Modul 4) | 1 |
| 3 | Driver motor | mis. L298N/L293D (sama seperti Modul 2) | 1 |
| 4 | Catu daya eksternal | sesuai kebutuhan motor (jangan gunakan 5V dari USB langsung) | 1 |
| 5 | Pushbutton (tactile) | 2 buah — tombol naik/turun target RPM | 2 |
| 6 | Breadboard | 830 titik | 1 |
| 7 | Kabel jumper male-male | — | secukupnya |
| 8 | Laptop/PC | VSCode + PlatformIO terinstal, Serial Plotter | 1 |

---

## C. Dasar Teori

### C.1 Dari Pulsa Encoder ke RPM
Encoder menghasilkan sejumlah pulsa untuk setiap putaran porosnya. Untuk motor gearbox (seperti JGB37-520), jumlah pulsa per putaran yang dirasakan pada **poros output** bukan hanya resolusi encoder mentah (CPR — *Counts Per Revolution*), melainkan sudah dikalikan dengan rasio reduksi gearbox:

```
CPR_TOTAL = CPR_encoder × Rasio_Gearbox
```

Kecepatan putar (RPM) kemudian dihitung dari jumlah pulsa yang terhitung dalam suatu interval waktu:

```
RPM = (ΔPulsa / ΔT) × (60 / CPR_TOTAL)
```

di mana `ΔPulsa` adalah jumlah pulsa yang terhitung selama interval `ΔT` (dalam detik). Karena nilai CPR dan rasio gearbox tidak selalu tercantum akurat pada datasheet/penjual, **kalibrasi langsung** (memutar poros output sejumlah putaran penuh yang diketahui, lalu mencatat total pulsa yang terhitung) umumnya menghasilkan nilai `CPR_TOTAL` yang lebih akurat.

![Gambar 1: Diagram sinyal quadrature encoder Channel A dan Channel B beserta pulsa yang terhitung per putaran poros](img/diagram_quadrature_encoder.png)

### C.2 Filtering Sederhana — Low-Pass Filter Alpha
Hasil pengukuran RPM dari encoder secara mentah cenderung **berisik (noisy)** — nilainya dapat melonjak-lonjak antar sampel akibat variasi singkat pada interval antar pulsa, terutama pada kecepatan rendah atau saat beban motor berubah. Metode filtering paling sederhana adalah **low-pass filter orde-1 (exponential filter)**, dengan persamaan diskret:

```
y[n] = α·y[n-1] + (1-α)·x[n]
```

di mana `y[n]` adalah nilai hasil filter, `x[n]` nilai mentah saat ini, dan `α` (0–1) menentukan seberapa besar pengaruh nilai sebelumnya — semakin besar `α`, sinyal semakin halus namun semakin lambat merespons perubahan (lag lebih besar). Filter ini hanya memiliki **satu parameter** (`α`) yang perlu ditentukan, sehingga sangat mudah diimplementasikan, namun nilainya bersifat tetap (tidak menyesuaikan diri terhadap kondisi sinyal).

![Gambar 2: Grafik perbandingan sinyal RPM mentah (noisy) vs hasil filter alpha pada beberapa nilai α berbeda, menunjukkan trade-off kehalusan vs lag](img/grafik_filter_alpha.png)

### C.3 Filtering Lanjut — Kalman Filter
**Kalman filter** adalah metode estimasi rekursif yang menggabungkan **prediksi** (berdasarkan model sistem) dengan **pengukuran baru**, masing-masing diberi bobot sesuai tingkat kepercayaan (ketidakpastian) terhadapnya. Berbeda dengan low-pass filter alpha yang bobotnya tetap, Kalman filter menghitung ulang bobot optimalnya (**Kalman gain**, `K`) pada setiap sampel, berdasarkan dua parameter:
- **Q (process noise):** seberapa besar nilai sebenarnya diperkirakan berubah dari waktu ke waktu — Q besar berarti sistem dianggap lebih dinamis/cepat berubah
- **R (measurement noise):** seberapa "berisik" hasil pengukuran sensor — R besar berarti sensor dianggap kurang dapat dipercaya

Versi sederhana Kalman filter 1-dimensi (skalar, tanpa model gerak eksplisit) bekerja dalam dua tahap setiap siklus:
```
// Predict
P = P + Q

// Update
K = P / (P + R)
X = X + K × (measurement - X)
P = (1 - K) × P
```
di mana `X` adalah estimasi nilai saat ini dan `P` adalah estimasi ketidakpastian (error covariance). Karena `K` dihitung ulang tiap siklus berdasarkan rasio `P` terhadap `R`, Kalman filter secara otomatis lebih "percaya" pada pengukuran saat estimasi belum yakin, dan lebih "percaya" pada model saat estimasi sudah stabil — perilaku adaptif yang tidak dimiliki filter alpha dengan bobot tetap.

![Gambar 3: Diagram blok siklus predict-update Kalman filter, beserta grafik perbandingan hasil Kalman filter vs filter alpha pada sinyal RPM yang sama](img/diagram_kalman_filter.png)

### C.4 Sistem Kontrol Closed-Loop
Sistem kontrol closed-loop (loop tertutup) membandingkan nilai yang diinginkan (**setpoint**) dengan nilai terukur sebenarnya (**feedback**), menghasilkan **error** yang digunakan untuk menentukan aksi kontrol berikutnya:

```
error = setpoint - nilai_terukur
```

Dua pendekatan dasar:
- **Kontrol On-Off (bang-bang):** aktuator diberi output penuh jika nilai terukur di bawah setpoint, dan dimatikan total jika sudah mencapai/melebihi setpoint. Sederhana, namun menghasilkan osilasi di sekitar setpoint (tidak pernah benar-benar stabil)
- **Kontrol Proportional (P):** output aktuator sebanding dengan besar error (`output = Kp × error`) — semakin besar error, semakin besar aksi koreksi. Lebih halus dari on-off, namun umumnya masih menyisakan **steady-state error** (nilai akhir tidak pernah persis mencapai setpoint)

![Gambar 4: Diagram blok sistem kontrol closed-loop (setpoint → kontroler → aktuator → plant → sensor → feedback ke pembanding), beserta grafik respons on-off (osilasi) vs P (steady-state error)](img/diagram_closed_loop.png)

### C.5 Kontrol PID
PID (Proportional-Integral-Derivative) menyempurnakan kontrol P dengan menambahkan dua komponen lain:
- **Integral (I):** mengakumulasikan error dari waktu ke waktu, menghilangkan steady-state error yang tersisa pada kontrol P
- **Derivative (D):** merespons **laju perubahan** error, membantu meredam overshoot dan mempercepat kestabilan

Dalam bentuk diskret (sesuai untuk implementasi pada mikrokontroler dengan periode sampling tetap):

```
u[n] = Kp·e[n] + Ki·Σ(e[n]·ΔT) + Kd·((e[n]-e[n-1])/ΔT)
```

Beberapa pertimbangan praktis dalam implementasi PID pada sistem nyata:
- **Integral windup:** akumulasi error pada komponen integral perlu dibatasi (*clamping*) agar tidak membesar tanpa batas saat sistem tidak dapat mengejar setpoint dalam waktu lama
- **PWM minimum:** motor DC seringkali membutuhkan PWM minimum tertentu agar benar-benar mulai berputar (mengatasi gesekan/deadzone) — output PID yang terlalu kecil namun bukan nol perlu dinaikkan ke ambang minimum ini
- **Tuning:** nilai Kp, Ki, Kd yang tepat umumnya dicari melalui trial-and-error terarah (amati respons, sesuaikan satu parameter setiap kali), atau metode yang lebih sistematis seperti **Ziegler-Nichols**

![Gambar 5: Diagram blok kontroler PID (jalur Proportional, Integral, Derivative dijumlahkan menjadi output), beserta grafik respons sistem sebelum dan sesudah tuning](img/diagram_blok_pid.png)

---

## D. Persiapan Sebelum Praktikum

1. Pastikan PlatformIO sudah terinstal (lihat **[setup_vscode_platformio.md](setup_vscode_platformio.md)**)
2. Instal ekstensi **Serial Plotter** pada VSCode — dibutuhkan untuk memvisualisasikan data pada seluruh Percobaan modul ini, karena VSCode (berbeda dari Arduino IDE) tidak memiliki Serial Plotter bawaan:
   - Buka **Extensions** pada VSCode (`Ctrl+Shift+X`), cari **"Serial Plotter"** (oleh badlogic), klik **Install**
   - Repository resmi: https://github.com/badlogic/serial-plotter
   - Setelah terinstal, buka melalui **Command Palette** (`Ctrl+Shift+P`) → ketik **"Serial Plotter: Open pane"**
   - Pada panel yang terbuka: pilih **port** serial ESP32 dan **baud rate** (115200, sesuai `Serial.begin()` pada kode), lalu klik **Start**
   - Ekstensi ini membaca format baris `>nama_variabel:nilai,nama_variabel2:nilai2` — persis format yang digunakan pada seluruh contoh kode `Serial.print()` di modul ini
   - **Penting:** port serial hanya dapat diakses satu proses dalam satu waktu — klik **Stop** pada Serial Plotter sebelum melakukan Upload program baru, agar proses upload tidak gagal karena port sedang terpakai
3. Pastikan rangkaian motor DC + driver (Modul 2) dan encoder quadrature (Modul 4 Percobaan 2) sudah tergabung dalam satu rangkaian — motor dan encoder berasal dari modul fisik yang sama
4. Buat project baru untuk Modul 5:
   - Name: `modul5-filtering-pid`
   - Board: **"Espressif ESP32 Dev Module"**
   - Framework: **Arduino**
5. Pastikan `platformio.ini` berisi:
   ```ini
   [env:esp32dev]
   platform = espressif32
   board = esp32dev
   framework = arduino
   ```
6. Tidak ada library eksternal tambahan yang dibutuhkan — seluruh fitur (interrupt, LEDC PWM) sudah tersedia dalam Arduino-ESP32 core, tidak perlu menambahkan baris `lib_deps` apa pun pada `platformio.ini`
7. **Alur kerja tiap Percobaan:** seluruh Percobaan pada modul ini (1–5) menggunakan **satu project PlatformIO yang sama** dari langkah 4–5 di atas. Setiap kode program pada modul ini bersifat **lengkap dan berdiri sendiri (self-contained)** — untuk berpindah ke Percobaan berikutnya, cukup **hapus seluruh isi** `src/main.cpp` dan **ganti dengan kode program Percobaan yang bersangkutan**, lalu **Build & Upload** ulang. Tidak perlu menggabungkan potongan kode dari beberapa Percobaan secara manual.
8. Sebelum upload, pastikan nilai `PULSES_PER_REV` pada tiap kode program sudah disesuaikan dengan hasil kalibrasi Anda sendiri pada Percobaan 1 (bukan nilai `2124.0` pada contoh, kecuali kalibrasi Anda kebetulan menghasilkan nilai yang sama)

---

## E. Kegiatan Praktikum

### PERCOBAAN 1 — Dari Pulsa ke RPM (Konversi & Kalibrasi)

**Tujuan:**
Mahasiswa mampu mengonversi data pulsa mentah dari encoder (Modul 4 Percobaan 2) menjadi nilai RPM, serta melakukan kalibrasi `CPR_TOTAL` secara langsung pada motor yang digunakan.

**Skema Rangkaian:**
Gunakan rangkaian encoder yang sama dengan **Modul 4 Percobaan 2** (Channel A = GPIO 32, Channel B = GPIO 33).

**`platformio.ini`:**
```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
```

**Langkah Kerja:**
1. Gunakan kembali kode decoding quadrature dari Modul 4 Percobaan 2 sebagai basis
2. Putar poros output motor sejumlah putaran penuh yang diketahui (mis. 10 putaran, tandai titik awal dan akhir secara manual), catat total pulsa yang terhitung program
3. Hitung `CPR_TOTAL` = total pulsa ÷ jumlah putaran, gunakan nilai ini sebagai `PULSES_PER_REV` pada program
4. Tambahkan perhitungan RPM ke program encoder menggunakan rumus pada Dasar Teori C.1
5. Uji dengan memutar motor menggunakan sumber daya terpisah (atau motor dari Modul 2), amati nilai RPM pada Serial Monitor

**Kode Program (Encoder + Konversi RPM):**
```cpp
#include <Arduino.h>

#define ENCODER_A_PIN 32
#define ENCODER_B_PIN 33

// Hasil kalibrasi: total pulsa / jumlah putaran penuh poros output
// Contoh pada motor referensi: 2124 (SESUAIKAN dengan hasil kalibrasi motor Anda)
const float PULSES_PER_REV = 2124.0;

volatile int32_t encoderCount = 0;
portMUX_TYPE encoderMux = portMUX_INITIALIZER_UNLOCKED;

int32_t previousCount = 0;
unsigned long previousMillis = 0;
const unsigned long SAMPLE_INTERVAL_MS = 100;

void IRAM_ATTR encoderA_ISR()
{
    bool channelB = digitalRead(ENCODER_B_PIN);

    portENTER_CRITICAL_ISR(&encoderMux);
    if (channelB == HIGH)
    {
        encoderCount++;
    }
    else
    {
        encoderCount--;
    }
    portEXIT_CRITICAL_ISR(&encoderMux);
}

void setup()
{
    Serial.begin(115200);

    pinMode(ENCODER_A_PIN, INPUT_PULLUP);
    pinMode(ENCODER_B_PIN, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(ENCODER_A_PIN), encoderA_ISR, RISING);
}

void loop()
{
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= SAMPLE_INTERVAL_MS)
    {
        float deltaTime = (currentMillis - previousMillis) / 1000.0;
        previousMillis = currentMillis;

        int32_t currentCount;
        portENTER_CRITICAL(&encoderMux);
        currentCount = encoderCount;
        portEXIT_CRITICAL(&encoderMux);

        int32_t deltaCount = currentCount - previousCount;
        previousCount = currentCount;

        // RPM = (Delta Pulsa / Delta Waktu) x (60 / CPR_TOTAL)
        float rpm = (deltaCount / deltaTime) * (60.0 / PULSES_PER_REV);

        Serial.printf("RPM: %.2f\n", rpm);
    }
}
```

**Penjelasan Kode:**
| Bagian | Penjelasan |
|---|---|
| `PULSES_PER_REV` | Hasil kalibrasi manual (total pulsa ÷ jumlah putaran) — nilainya spesifik untuk tiap motor, tidak boleh disamakan begitu saja antar motor berbeda |
| `deltaTime = (currentMillis - previousMillis) / 1000.0` | Interval waktu sampling dikonversi ke detik, agar hasil RPM sesuai satuan (menit) |
| `rpm = (deltaCount / deltaTime) * (60.0 / PULSES_PER_REV)` | Implementasi langsung rumus RPM pada Dasar Teori C.1 |

---

### PERCOBAAN 2 — Filtering Sederhana dengan Alpha (Low-Pass Filter)

**Tujuan:**
Mahasiswa mampu mengidentifikasi noise pada sinyal RPM mentah dan menerapkan low-pass filter alpha untuk menghaluskannya.

**`platformio.ini`:**
```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
```

**Langkah Kerja:**
1. Gunakan program dari Percobaan 1, amati nilai RPM pada **Serial Plotter** (bukan hanya Serial Monitor) untuk melihat fluktuasi secara visual
2. Implementasikan low-pass filter alpha pada nilai RPM, tampilkan RPM mentah dan RPM terfilter secara bersamaan di Serial Plotter
3. Uji beberapa nilai `α` berbeda (mis. 0.3, 0.7, 0.9), amati trade-off antara kehalusan sinyal (smoothing) dan kecepatan respons (lag) terhadap perubahan kecepatan motor yang sebenarnya

**Kode Program (Encoder + RPM + Low-Pass Filter Alpha — Program Lengkap):**
```cpp
#include <Arduino.h>

#define ENCODER_A_PIN 32
#define ENCODER_B_PIN 33

// Hasil kalibrasi Percobaan 1 — SESUAIKAN dengan motor Anda
const float PULSES_PER_REV = 2124.0;

#define FILTER_ALPHA 0.7 // sesuaikan: makin besar = makin halus, makin lambat merespons

volatile int32_t encoderCount = 0;
portMUX_TYPE encoderMux = portMUX_INITIALIZER_UNLOCKED;

int32_t previousCount = 0;
unsigned long previousMillis = 0;
const unsigned long SAMPLE_INTERVAL_MS = 100;

float rawRPM = 0.0;
float filteredRPM = 0.0;

void IRAM_ATTR encoderA_ISR()
{
    bool channelB = digitalRead(ENCODER_B_PIN);

    portENTER_CRITICAL_ISR(&encoderMux);
    if (channelB == HIGH)
    {
        encoderCount++;
    }
    else
    {
        encoderCount--;
    }
    portEXIT_CRITICAL_ISR(&encoderMux);
}

void setup()
{
    Serial.begin(115200);

    pinMode(ENCODER_A_PIN, INPUT_PULLUP);
    pinMode(ENCODER_B_PIN, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(ENCODER_A_PIN), encoderA_ISR, RISING);
}

void loop()
{
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= SAMPLE_INTERVAL_MS)
    {
        float deltaTime = (currentMillis - previousMillis) / 1000.0;
        previousMillis = currentMillis;

        int32_t currentCount;
        portENTER_CRITICAL(&encoderMux);
        currentCount = encoderCount;
        portEXIT_CRITICAL(&encoderMux);

        int32_t deltaCount = currentCount - previousCount;
        previousCount = currentCount;

        // Percobaan 1: konversi pulsa -> RPM
        rawRPM = (deltaCount / deltaTime) * (60.0 / PULSES_PER_REV);

        // Percobaan 2: filtering alpha
        filteredRPM = FILTER_ALPHA * filteredRPM + (1.0 - FILTER_ALPHA) * rawRPM;

        // Format khusus agar terbaca oleh ESP32 Serial Plotter (Serial Monitor -> Plotter)
        Serial.print(">");
        Serial.print("RPM_Mentah:");
        Serial.print(rawRPM, 2);
        Serial.print(",");
        Serial.print("RPM_Alpha:");
        Serial.print(filteredRPM, 2);
        Serial.println();
    }
}
```

**Penjelasan Kode:**
| Bagian | Penjelasan |
|---|---|
| `filteredRPM = FILTER_ALPHA * filteredRPM + (1.0 - FILTER_ALPHA) * rawRPM` | Implementasi langsung persamaan low-pass filter alpha pada Dasar Teori C.2 |
| `Serial.print(">")` ... format `Nama:nilai,` | Format khusus yang dikenali ESP32 Serial Plotter pada VSCode/Arduino IDE untuk menampilkan beberapa variabel dalam satu grafik sekaligus |
| Nilai `FILTER_ALPHA` diuji bertahap | Menunjukkan trade-off mendasar: semakin agresif filtering (semakin halus), semakin besar pula keterlambatan (lag) terhadap perubahan nyata |

---

### PERCOBAAN 3 — Filtering dengan Kalman Filter

**Tujuan:**
Mahasiswa mampu menerapkan Kalman filter 1D sebagai metode filtering alternatif untuk sinyal RPM, serta membandingkan hasilnya dengan low-pass filter alpha pada Percobaan 2.

**`platformio.ini`:**
```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
```

**Langkah Kerja:**
1. Gunakan kembali program dari Percobaan 1–2
2. Implementasikan kelas `KalmanFilter` sederhana sesuai kode di bawah
3. Terapkan Kalman filter pada sinyal RPM mentah, tampilkan RPM mentah, RPM hasil filter alpha, dan RPM hasil Kalman filter secara bersamaan pada Serial Plotter
4. Uji pengaruh perubahan nilai `Q` (mis. 0.001 vs 0.1) dan `R` (mis. 0.1 vs 5) terhadap kehalusan dan responsivitas hasil filter
5. Bandingkan secara visual: pada kondisi RPM yang sama, manakah yang lebih halus namun tetap responsif — filter alpha (Percobaan 2) atau Kalman filter?

**Kode Program (Encoder + RPM + Filter Alpha + Kalman Filter — Program Lengkap):**
```cpp
#include <Arduino.h>

#define ENCODER_A_PIN 32
#define ENCODER_B_PIN 33

// Hasil kalibrasi Percobaan 1 — SESUAIKAN dengan motor Anda
const float PULSES_PER_REV = 2124.0;

#define FILTER_ALPHA 0.7 // filter alpha (Percobaan 2), sebagai pembanding

volatile int32_t encoderCount = 0;
portMUX_TYPE encoderMux = portMUX_INITIALIZER_UNLOCKED;

int32_t previousCount = 0;
unsigned long previousMillis = 0;
const unsigned long SAMPLE_INTERVAL_MS = 100;

float rawRPM = 0.0;
float filteredRPM = 0.0; // hasil filter alpha

class KalmanFilter
{
private:
    float Q = 0.001; // process noise - seberapa besar nilai sebenarnya diperkirakan berubah
    float R = 0.1;   // measurement noise - seberapa "berisik" hasil pengukuran sensor
    float X = 0;     // estimasi nilai saat ini
    float P = 1;     // estimasi ketidakpastian (error covariance)
    float K = 0;     // Kalman gain

public:
    float update(float measurement)
    {
        // Predict
        P += Q;

        // Update
        K = P / (P + R);
        X = X + K * (measurement - X);
        P = (1 - K) * P;

        return X;
    }
};

KalmanFilter rpmKalman;
float kalmanRPM = 0.0; // hasil Kalman filter

void IRAM_ATTR encoderA_ISR()
{
    bool channelB = digitalRead(ENCODER_B_PIN);

    portENTER_CRITICAL_ISR(&encoderMux);
    if (channelB == HIGH)
    {
        encoderCount++;
    }
    else
    {
        encoderCount--;
    }
    portEXIT_CRITICAL_ISR(&encoderMux);
}

void setup()
{
    Serial.begin(115200);

    pinMode(ENCODER_A_PIN, INPUT_PULLUP);
    pinMode(ENCODER_B_PIN, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(ENCODER_A_PIN), encoderA_ISR, RISING);
}

void loop()
{
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= SAMPLE_INTERVAL_MS)
    {
        float deltaTime = (currentMillis - previousMillis) / 1000.0;
        previousMillis = currentMillis;

        int32_t currentCount;
        portENTER_CRITICAL(&encoderMux);
        currentCount = encoderCount;
        portEXIT_CRITICAL(&encoderMux);

        int32_t deltaCount = currentCount - previousCount;
        previousCount = currentCount;

        // Percobaan 1: konversi pulsa -> RPM
        rawRPM = (deltaCount / deltaTime) * (60.0 / PULSES_PER_REV);

        // Percobaan 2: filtering alpha (pembanding)
        filteredRPM = FILTER_ALPHA * filteredRPM + (1.0 - FILTER_ALPHA) * rawRPM;

        // Percobaan 3: filtering Kalman
        kalmanRPM = rpmKalman.update(rawRPM);

        Serial.print(">");
        Serial.print("RPM_Mentah:");
        Serial.print(rawRPM, 2);
        Serial.print(",RPM_Alpha:");
        Serial.print(filteredRPM, 2);
        Serial.print(",RPM_Kalman:");
        Serial.print(kalmanRPM, 2);
        Serial.println();
    }
}
```

**Penjelasan Kode:**
| Bagian | Penjelasan |
|---|---|
| `P += Q` (tahap Predict) | Menaikkan ketidakpastian estimasi seiring waktu, sebelum ada pengukuran baru yang menguranginya kembali |
| `K = P / (P + R)` | Kalman gain — rasio ketidakpastian model terhadap total ketidakpastian (model + pengukuran); menentukan seberapa besar pengukuran baru "dipercaya" |
| `X = X + K * (measurement - X)` (tahap Update) | Estimasi diperbarui sebagian menuju nilai pengukuran baru, sebesar proporsi `K` |
| `P = (1 - K) * P` | Ketidakpastian berkurang setelah menggabungkan informasi dari pengukuran baru |
| Nilai `Q` dan `R` sebagai parameter tuning | Berbeda dari filter alpha yang hanya punya 1 parameter, Kalman filter punya 2 parameter yang saling memengaruhi — lebih fleksibel namun juga lebih kompleks untuk di-tuning |

**Latihan Tambahan:**
Bandingkan jumlah parameter yang perlu di-tuning (filter alpha: 1 parameter vs Kalman filter: 2 parameter) serta kompleksitas komputasi keduanya. Diskusikan: dalam kondisi seperti apa kompleksitas tambahan Kalman filter sepadan dengan manfaatnya dibanding filter alpha yang jauh lebih sederhana?

---

### PERCOBAAN 4 — Kontrol Closed-Loop: On-Off vs Proportional (P)

**Tujuan:**
Mahasiswa mampu mengimplementasikan dan membandingkan kontrol on-off dengan kontrol Proportional (P) untuk mengatur kecepatan motor DC menuju target RPM.

**Skema Rangkaian:**
Gunakan rangkaian motor DC + driver dari **Modul 2** (ENA = GPIO 25, IN1 = GPIO 26, IN2 = GPIO 27), digabung dengan encoder dan filtering RPM dari Percobaan 1–3 modul ini (filter alpha atau Kalman, keduanya dapat digunakan sebagai sumber RPM terfilter).

**`platformio.ini`:**
```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
```

**Langkah Kerja:**
1. Tentukan target RPM tetap (mis. 100 RPM) langsung di kode program
2. Implementasikan kontrol **on-off**: motor diberi PWM maksimum jika RPM terfilter di bawah target, dan dimatikan jika sudah mencapai/melebihi target — amati osilasi kecepatan motor di sekitar target pada Serial Plotter
3. Implementasikan kontrol **Proportional (P)**: `outputPWM = Kp × error`, coba beberapa nilai Kp, amati apakah RPM aktual dapat mendekati target tanpa osilasi sebesar on-off, namun tetap menyisakan selisih (steady-state error)
4. Bandingkan kedua pendekatan pada Serial Plotter: RPM target, RPM terfilter, dan PWM yang diberikan

**Kode Program (Encoder + Filter + Kontrol On-Off/Proportional — Program Lengkap):**
```cpp
#include <Arduino.h>

// ==================== ENCODER ====================
#define ENCODER_A_PIN 32
#define ENCODER_B_PIN 33

// Hasil kalibrasi Percobaan 1 — SESUAIKAN dengan motor Anda
const float PULSES_PER_REV = 2124.0;

volatile int32_t encoderCount = 0;
portMUX_TYPE encoderMux = portMUX_INITIALIZER_UNLOCKED;

int32_t previousCount = 0;
unsigned long previousMillis = 0;
const unsigned long SAMPLE_INTERVAL_MS = 100;

float rawRPM = 0.0;
float filteredRPM = 0.0;
#define FILTER_ALPHA 0.7 // filter alpha dari Percobaan 2

// ==================== DRIVER MOTOR (Modul 2) ====================
#define ENA 25
#define IN1 26
#define IN2 27

const int pwmChannel = 0;
const int pwmFreq = 1000;
const int pwmResolution = 8; // 0-255

// ==================== KONTROL ====================
const float targetRPM = 100.0;
const float Kp = 3.0; // untuk mode Proportional, sesuaikan hasil pengamatan

bool useProportional = true; // ganti false untuk menguji mode on-off

void IRAM_ATTR encoderA_ISR()
{
    bool channelB = digitalRead(ENCODER_B_PIN);

    portENTER_CRITICAL_ISR(&encoderMux);
    if (channelB == HIGH)
    {
        encoderCount++;
    }
    else
    {
        encoderCount--;
    }
    portEXIT_CRITICAL_ISR(&encoderMux);
}

void applyControl(float rpmInput)
{
    float error = targetRPM - rpmInput;
    int pwmOutput = 0;

    if (useProportional)
    {
        // Kontrol Proportional
        pwmOutput = (int)(Kp * error);
    }
    else
    {
        // Kontrol On-Off
        pwmOutput = (error > 0) ? 255 : 0;
    }

    pwmOutput = constrain(pwmOutput, 0, 255);
    ledcWrite(pwmChannel, pwmOutput);

    Serial.print(">");
    Serial.print("Target:");
    Serial.print(targetRPM, 2);
    Serial.print(",RPM:");
    Serial.print(rpmInput, 2);
    Serial.print(",PWM:");
    Serial.print(pwmOutput);
    Serial.println();
}

void setup()
{
    Serial.begin(115200);

    pinMode(ENCODER_A_PIN, INPUT_PULLUP);
    pinMode(ENCODER_B_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(ENCODER_A_PIN), encoderA_ISR, RISING);

    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    ledcSetup(pwmChannel, pwmFreq, pwmResolution);
    ledcAttachPin(ENA, pwmChannel);

    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
}

void loop()
{
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= SAMPLE_INTERVAL_MS)
    {
        float deltaTime = (currentMillis - previousMillis) / 1000.0;
        previousMillis = currentMillis;

        int32_t currentCount;
        portENTER_CRITICAL(&encoderMux);
        currentCount = encoderCount;
        portEXIT_CRITICAL(&encoderMux);

        int32_t deltaCount = currentCount - previousCount;
        previousCount = currentCount;

        // Percobaan 1: konversi pulsa -> RPM
        rawRPM = (deltaCount / deltaTime) * (60.0 / PULSES_PER_REV);

        // Percobaan 2: filtering alpha
        filteredRPM = FILTER_ALPHA * filteredRPM + (1.0 - FILTER_ALPHA) * rawRPM;

        // Percobaan 4: kontrol on-off / proportional
        applyControl(filteredRPM);
    }
}
```

**Penjelasan Kode:**
| Bagian | Penjelasan |
|---|---|
| `useProportional` | Flag sederhana untuk beralih antara mode on-off dan Proportional tanpa mengubah struktur program |
| `pwmOutput = (error > 0) ? 255 : 0` | Kontrol on-off — hanya dua kemungkinan output, penuh atau mati sama sekali |
| `pwmOutput = (int)(Kp * error)` | Kontrol Proportional — output berskala langsung dengan besar error |
| `constrain(pwmOutput, 0, 255)` | Membatasi output PWM agar tetap berada pada rentang valid 8-bit, mencegah nilai negatif atau melebihi batas maksimum |

---

### PERCOBAAN 5 — Kontrol PID Lengkap & Tuning

**Tujuan:**
Mahasiswa mampu mengimplementasikan kontrol PID lengkap untuk mengatur kecepatan motor DC menuju target RPM yang dapat diubah secara interaktif, serta melakukan tuning parameter Kp, Ki, dan Kd.

**Skema Rangkaian:**

| Komponen | Pin ESP32 | Keterangan |
|---|---|---|
| Encoder — Channel A / B | GPIO 32 / GPIO 33 | Sama seperti Percobaan 1–3 |
| Driver motor — ENA / IN1 / IN2 | GPIO 25 / GPIO 26 / GPIO 27 | Sama seperti Percobaan 4 (Modul 2) |
| Tombol Naik Target RPM | GPIO 14 | `INPUT_PULLUP` |
| Tombol Turun Target RPM | GPIO 16 | `INPUT_PULLUP` |

**`platformio.ini`:**
```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
```

**Langkah Kerja:**
1. Gabungkan encoder (Percobaan 1), filtering alpha (Percobaan 2), dan kontrol motor (Percobaan 4) menjadi satu program PID lengkap sesuai kode di bawah
2. Rangkai dua tombol untuk menaikkan/menurunkan target RPM secara interaktif saat program berjalan
3. Jalankan program dengan nilai awal Kp, Ki, Kd yang disediakan, amati respons pada Serial Plotter (Target, RPM terfilter, PWM)
4. **Tuning:** ubah nilai Kp terlebih dahulu (dengan Ki=Kd=0) hingga respons cukup cepat namun belum berosilasi berlebihan, lalu tambahkan Ki secara bertahap untuk menghilangkan steady-state error, dan Kd secukupnya untuk meredam overshoot
5. Amati efek `integral clamping` (`INTEGRAL_MIN`/`INTEGRAL_MAX`) dengan mengubah/menghapus batasnya — apa yang terjadi pada respons sistem saat target RPM diturunkan tiba-tiba ke 0?
6. **Latihan tambahan:** ganti sumber `filteredRPM` pada program dari hasil filter alpha (Percobaan 2) menjadi hasil Kalman filter (Percobaan 3), amati apakah respons kontrol PID berubah

**Kode Program (Kontrol PID Lengkap — Encoder, Filter, PID, Tombol Target):**
```cpp
#include <Arduino.h>
#include <math.h>

// ==================== ENCODER ====================
constexpr uint8_t ENCODER_A_PIN = 32;
constexpr uint8_t ENCODER_B_PIN = 33;

// Hasil kalibrasi Percobaan 1 — SESUAIKAN dengan motor Anda
constexpr float PULSES_PER_OUTPUT_REV = 2124.0f;

volatile int32_t encoderCount = 0;
portMUX_TYPE encoderMux = portMUX_INITIALIZER_UNLOCKED;

int32_t previousCount = 0;
uint32_t previousSampleTime = 0;
constexpr uint32_t SAMPLE_INTERVAL_MS = 100;

// ==================== DRIVER MOTOR (L298N) ====================
constexpr uint8_t ENA_PIN = 25;
constexpr uint8_t IN1_PIN = 26;
constexpr uint8_t IN2_PIN = 27;

constexpr int PWM_CHANNEL = 0;
constexpr uint32_t PWM_FREQUENCY = 1000;
constexpr uint8_t PWM_RESOLUTION = 8;

constexpr int PWM_MIN = 0;
constexpr int PWM_MIN_RUN = 55; // PWM minimum agar motor benar-benar berputar
constexpr int PWM_MAX = 255;

// ==================== TOMBOL TARGET RPM ====================
constexpr uint8_t BUTTON_UP_PIN = 14;
constexpr uint8_t BUTTON_DOWN_PIN = 16;
constexpr float TARGET_STEP_RPM = 10.0f;
constexpr float TARGET_MIN_RPM = 0.0f;
constexpr float TARGET_MAX_RPM = 200.0f;
constexpr uint32_t DEBOUNCE_MS = 40;

float targetRPM = 0.0f;

struct Button
{
    uint8_t pin;
    bool lastRawState;
    bool stableState;
    uint32_t lastChangeTime;
};

Button buttonUp = {BUTTON_UP_PIN, HIGH, HIGH, 0};
Button buttonDown = {BUTTON_DOWN_PIN, HIGH, HIGH, 0};

// ==================== FILTER (ALPHA — Percobaan 2) ====================
constexpr float FILTER_ALPHA = 0.70f; // hasil Percobaan 2

float rawRPM = 0.0f;
float filteredRPM = 0.0f;

// ==================== PID ====================
// Nilai awal untuk memulai tuning — BUKAN nilai final
float Kp = 2.0f;
float Ki = 0.8f;
float Kd = 0.05f;

constexpr float INTEGRAL_MIN = -150.0f;
constexpr float INTEGRAL_MAX = 150.0f;

float integralError = 0.0f;
float previousError = 0.0f;
int currentPWM = 0;

// ==================== INTERRUPT ENCODER ====================
void IRAM_ATTR encoderA_ISR()
{
    bool channelB = digitalRead(ENCODER_B_PIN);

    portENTER_CRITICAL_ISR(&encoderMux);
    if (channelB == HIGH)
        encoderCount++;
    else
        encoderCount--;
    portEXIT_CRITICAL_ISR(&encoderMux);
}

// ==================== PEMBACAAN TOMBOL (DEBOUNCE) ====================
bool buttonPressed(Button &button)
{
    bool rawState = digitalRead(button.pin);

    if (rawState != button.lastRawState)
    {
        button.lastRawState = rawState;
        button.lastChangeTime = millis();
    }

    if (millis() - button.lastChangeTime >= DEBOUNCE_MS && rawState != button.stableState)
    {
        button.stableState = rawState;
        if (button.stableState == LOW)
        {
            return true;
        }
    }
    return false;
}

// ==================== KONTROL MOTOR ====================
void applyMotorPWM(int pwm)
{
    pwm = constrain(pwm, PWM_MIN, PWM_MAX);
    currentPWM = pwm;

    if (pwm == 0)
    {
        ledcWrite(PWM_CHANNEL, 0);
        digitalWrite(IN1_PIN, LOW);
        digitalWrite(IN2_PIN, LOW);
        return;
    }

    digitalWrite(IN1_PIN, HIGH);
    digitalWrite(IN2_PIN, LOW);
    ledcWrite(PWM_CHANNEL, pwm);
}

void stopMotor()
{
    applyMotorPWM(0);
    integralError = 0.0f;
    previousError = 0.0f;
}

// ==================== PERHITUNGAN PID ====================
void updatePID(float deltaTime)
{
    if (targetRPM <= 0.0f)
    {
        stopMotor();
        return;
    }

    float error = targetRPM - filteredRPM;
    integralError += error * deltaTime;
    integralError = constrain(integralError, INTEGRAL_MIN, INTEGRAL_MAX);

    float derivativeError = (error - previousError) / deltaTime;

    float pidOutput = (Kp * error) + (Ki * integralError) + (Kd * derivativeError);

    // Jika output PID kecil tapi target belum nol, beri PWM minimum agar motor mulai bergerak
    if (pidOutput > 0.0f && pidOutput < PWM_MIN_RUN)
    {
        pidOutput = PWM_MIN_RUN;
    }

    pidOutput = constrain(pidOutput, (float)PWM_MIN, (float)PWM_MAX);
    applyMotorPWM((int)pidOutput);

    previousError = error;
}

// ==================== SETUP ====================
void setup()
{
    Serial.begin(115200);

    pinMode(ENCODER_A_PIN, INPUT_PULLUP);
    pinMode(ENCODER_B_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(ENCODER_A_PIN), encoderA_ISR, RISING);

    pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
    pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);

    pinMode(IN1_PIN, OUTPUT);
    pinMode(IN2_PIN, OUTPUT);
    ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION); // konfigurasi channel PWM (core 2.x)
    ledcAttachPin(ENA_PIN, PWM_CHANNEL);                   // hubungkan channel ke pin ENA

    stopMotor();
    previousSampleTime = millis();
}

// ==================== LOOP ====================
void loop()
{
    if (buttonPressed(buttonUp))
    {
        targetRPM = constrain(targetRPM + TARGET_STEP_RPM, TARGET_MIN_RPM, TARGET_MAX_RPM);
    }

    if (buttonPressed(buttonDown))
    {
        targetRPM = constrain(targetRPM - TARGET_STEP_RPM, TARGET_MIN_RPM, TARGET_MAX_RPM);
        if (targetRPM <= 0.0f)
        {
            stopMotor();
        }
    }

    uint32_t currentTime = millis();
    if (currentTime - previousSampleTime >= SAMPLE_INTERVAL_MS)
    {
        float deltaTime = (currentTime - previousSampleTime) / 1000.0f;

        int32_t currentCount;
        portENTER_CRITICAL(&encoderMux);
        currentCount = encoderCount;
        portEXIT_CRITICAL(&encoderMux);

        int32_t deltaCount = currentCount - previousCount;
        previousCount = currentCount;

        // Percobaan 1: konversi pulsa -> RPM
        rawRPM = fabsf((deltaCount * 60.0f) / (PULSES_PER_OUTPUT_REV * deltaTime));

        // Percobaan 2: filtering alpha (dapat diganti Kalman filter dari Percobaan 3 — lihat Latihan Tambahan)
        filteredRPM = FILTER_ALPHA * filteredRPM + (1.0f - FILTER_ALPHA) * rawRPM;

        // Percobaan 5: kontrol PID
        updatePID(deltaTime);

        // Output untuk Serial Plotter
        Serial.print(">Target:");
        Serial.print(targetRPM, 2);
        Serial.print(",RPM:");
        Serial.print(filteredRPM, 2);
        Serial.print(",PWM:");
        Serial.print(currentPWM);
        Serial.println();

        previousSampleTime = currentTime;
    }
}
```

**Penjelasan Kode:**
| Bagian | Penjelasan |
|---|---|
| `updatePID()` | Implementasi langsung persamaan PID diskret pada Dasar Teori C.5, termasuk integral clamping dan PWM minimum |
| `integralError = constrain(integralError, INTEGRAL_MIN, INTEGRAL_MAX)` | Mencegah *integral windup* — akumulasi error yang membesar tanpa batas saat sistem tidak dapat mengejar target dalam waktu lama |
| `PWM_MIN_RUN` | Mengatasi *deadzone* motor — output PID kecil namun bukan nol dinaikkan ke ambang minimum agar motor benar-benar mulai berputar |
| `buttonPressed()` dengan `struct Button` | Debouncing berbasis state machine (sama seperti Modul 1) untuk kedua tombol target RPM, mencegah satu kali tekan terhitung berkali-kali |
| Alur `loop()` | Menggabungkan seluruh Percobaan 1–4 modul ini dalam satu siklus sampling: decoding encoder → konversi RPM → filtering → kontrol PID → tampilkan hasil |

**Tugas Akhir Modul 5:**
Lakukan proses tuning PID secara sistematis: catat respons sistem (overshoot, settling time, steady-state error) untuk minimal 3 kombinasi Kp/Ki/Kd berbeda, sajikan dalam bentuk tabel perbandingan, dan tentukan kombinasi parameter terbaik menurut kriteria yang Anda tetapkan sendiri (mis. settling time tercepat dengan overshoot < 10%). Sebagai nilai tambah, ulangi salah satu kombinasi terbaik menggunakan Kalman filter (Percobaan 3) sebagai pengganti filter alpha, dan bandingkan hasilnya.

---

## F. Tugas Pasca Praktikum (Simulasi Wokwi)

Motor DC + encoder quadrature JGB37-520 yang digunakan pada modul ini kemungkinan **tidak tersedia** sebagai part siap pakai di [Wokwi](https://wokwi.com), sehingga closed-loop nyata (motor fisik sebagai *plant*) tidak dapat direproduksi langsung. Sebagai gantinya, tugas berikut menggunakan pendekatan **simulasi software**: motor digantikan oleh model matematis sederhana (*plant* orde-1) yang dihitung di dalam kode itu sendiri — pendekatan umum yang dipakai untuk menguji algoritma kontrol sebelum diuji ke hardware asli. Kerjakan tugas berikut **setelah** kegiatan praktikum selesai.

> **Tips:** Ekstensi Wokwi untuk VSCode dapat meneruskan output Serial simulasi ke Serial Monitor/Serial Plotter VSCode yang sama seperti digunakan pada hardware asli (lihat Bagian D), sehingga visualisasi grafik tetap dapat dilakukan dengan cara yang sudah dikenal.

**Tugas 1 — Filtering pada Sinyal Sintetis (Wokwi):**
1. Buat project Wokwi baru dengan board **ESP32**, rangkai satu **potensiometer** sebagai baseline sinyal
2. Pada kode, tambahkan **noise buatan** ke nilai potensiometer setiap sampling (mis. `noisyValue = baseValue + random(-50, 50)`), mensimulasikan sinyal RPM mentah yang berisik seperti pada Percobaan 1
3. Terapkan **filter alpha** (Percobaan 2) dan **Kalman filter** (Percobaan 3) pada sinyal sintetis yang sama, kirim ketiga nilai (mentah, alpha, Kalman) ke Serial Plotter secara bersamaan
4. Bandingkan hasil kedua filter pada sinyal buatan ini dengan hasil pada Percobaan 1–3 (hardware asli) — apakah kesimpulan mengenai trade-off kehalusan vs lag tetap konsisten?

**Tugas 2 — PID terhadap Plant Simulasi (Wokwi):**
Ganti sumber RPM dengan **model plant orde-1 dalam kode** (mis. `simulatedRPM += (pwmOutput - simulatedRPM) * dt / tau`, dengan `tau` sebagai konstanta waktu motor buatan), lalu jalankan kontroler PID (Percobaan 5) terhadap plant simulasi tersebut alih-alih motor fisik. Uji beberapa kombinasi Kp/Ki/Kd pada plant simulasi ini terlebih dahulu sebagai *starting point* sebelum tuning ulang pada motor fisik — diskusikan apakah parameter yang baik pada simulasi juga baik pada hardware asli, dan mengapa bisa berbeda (mismatch antara model sederhana dengan motor nyata).

**Pengumpulan:** Sertakan link project Wokwi (mode *share*, pastikan visibility public/unlisted) beserta laporan singkat pada berkas terpisah.

---

## G. Referensi
1. sss2022, *Closed-Loop Speed Control of a DC Motor With Encoder Using a Discrete PI Controller on Arduino*, Instructables — rujukan formula RPM, filter, dan struktur kontrol PI, https://www.instructables.com/Closed-Loop-Speed-Control-of-a-DC-Motor-With-Encod/
2. Greg Welch & Gary Bishop, *An Introduction to the Kalman Filter*, University of North Carolina at Chapel Hill — referensi dasar teori Kalman filter
3. Espressif Systems, *ESP32 Arduino Core Documentation — LEDC*, https://docs.espressif.com/projects/arduino-esp32/
4. Katsuhiko Ogata, *Modern Control Engineering*, Pearson — referensi dasar teori kontrol PID
5. PlatformIO Documentation, https://docs.platformio.org/
6. badlogic, *Serial Plotter — VSCode Extension*, digunakan untuk visualisasi data Serial Plotter pada seluruh Percobaan modul ini, https://github.com/badlogic/serial-plotter
