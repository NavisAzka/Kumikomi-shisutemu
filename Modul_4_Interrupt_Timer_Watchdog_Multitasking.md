# MODUL 4
# INTERRUPT, TIMER, WATCHDOG, & MULTITASKING

**Mata Kuliah:** Praktikum Mikrokontroler & Embedded System
**Alokasi Waktu:** 5 x Percobaan (@ 100–150 menit)
**Platform:** ESP32 (Framework Arduino)
**IDE:** VSCode + PlatformIO

> **Catatan:** Modul ini melanjutkan penggunaan ESP32 dengan framework Arduino seperti pada Modul 1–3. Langkah instalasi VSCode, PlatformIO, dan driver USB-to-Serial tidak diulang di sini — lihat kembali **Modul 1, Bagian D**. Seluruh contoh kode pada modul ini mengasumsikan **Arduino-ESP32 core versi 3.x ke atas**, konsisten dengan Modul 2 dan 3 (API Timer dan Watchdog berbeda signifikan pada core versi 2.x).

---

## A. Capaian Pembelajaran

Setelah menyelesaikan Modul 4, praktikan mampu:
1. Menjelaskan mengapa external interrupt tetap dapat berfungsi meskipun program utama mengalami hang, serta memanfaatkannya sebagai mekanisme pemulihan manual (manual recovery)
2. Menjelaskan konsep interrupt sebagai alternatif polling, dan mengimplementasikan external interrupt untuk penghitungan pulsa (encoder)
3. Mengimplementasikan timer interrupt untuk menjalankan tugas periodik tanpa memblokir program utama (`delay()`)
4. Menjelaskan fungsi watchdog timer dan mengimplementasikannya untuk menjaga keandalan sistem
5. Mengimplementasikan multitasking dasar menggunakan FreeRTOS, termasuk komunikasi antar task melalui queue

---

## B. Alat dan Bahan

| No | Nama Komponen | Spesifikasi | Jumlah |
|---|---|---|---|
| 1 | Board ESP32 DevKit | ESP32 DevKit v1 | 1 |
| 2 | Motor DC + encoder magnetik quadrature | modul dengan konektor JST 6-pin, mis. tipe JGB37-520: Merah/Putih = daya motor (+/−), Kuning/Hijau = Channel A/Channel B encoder, Biru/Hitam = VCC/GND encoder (dapat memakai motor dari Modul 2 — **verifikasi ulang pemetaan warna pada modul fisik Anda**, karena dapat berbeda antar produsen) | 1 |
| 3 | LED + resistor 220Ω | untuk uji timer interrupt & indikator tombol darurat | 1 |
| 4 | Pushbutton (tactile) | dipakai ulang pada tombol darurat (Percobaan 1) dan latihan tambahan queue (Percobaan 5) | 1 |
| 5 | Breadboard | 830 titik | 1 |
| 6 | Kabel jumper male-male | — | secukupnya |
| 7 | Laptop/PC | VSCode + PlatformIO terinstal | 1 |

---

## C. Dasar Teori

### C.1 External Interrupt
Pendekatan **polling** memeriksa status suatu pin secara terus-menerus di dalam `loop()`, yang kurang efisien untuk event yang jarang terjadi atau butuh respons cepat. **Interrupt** memungkinkan CPU merespons suatu event secara langsung — saat event terjadi (mis. perubahan sinyal pada pin GPIO), eksekusi program utama dihentikan sementara untuk menjalankan fungsi khusus yang disebut **ISR (Interrupt Service Routine)**, lalu kembali melanjutkan program utama.

Konsep penting terkait interrupt pada ESP32:
- **Trigger mode:** `RISING` (LOW→HIGH), `FALLING` (HIGH→LOW), atau `CHANGE` (kedua arah)
- **ISR harus singkat:** tidak boleh berisi operasi yang memblokir/lambat seperti `delay()` atau `Serial.print()` dalam jumlah banyak
- **Variabel `volatile`:** variabel yang diakses baik dari ISR maupun dari `loop()` harus dideklarasikan `volatile` agar compiler tidak melakukan optimisasi yang menyebabkan nilai terbaru tidak terbaca dengan benar
- **`IRAM_ATTR`:** menandai agar fungsi ISR dieksekusi dari RAM, bukan Flash — wajib pada ESP32 karena Flash dapat sedang diakses oleh proses lain saat interrupt terjadi
- **Proteksi variabel bersama (`portMUX_TYPE`):** karena ESP32 memiliki dua core, sepasang fungsi `noInterrupts()`/`interrupts()` (yang hanya menonaktifkan interrupt pada satu core) tidak cukup aman untuk melindungi variabel yang diakses bersama oleh ISR dan `loop()`. Pendekatan yang benar pada ESP32 adalah menggunakan **critical section** berbasis `portMUX_TYPE` beserta `portENTER_CRITICAL_ISR()`/`portEXIT_CRITICAL_ISR()` (di dalam ISR) dan `portENTER_CRITICAL()`/`portEXIT_CRITICAL()` (di luar ISR), yang benar-benar aman terhadap akses simultan dari kedua core
- **Interrupt tetap berjalan meski program hang:** karena ISR dipicu di level hardware, ISR akan tetap dieksekusi meskipun `loop()` sedang terjebak dalam kondisi blocking/infinite loop, selama interrupt global tidak dinonaktifkan — sifat ini dapat dimanfaatkan sebagai mekanisme pemulihan darurat (lihat Percobaan 1)

### C.2 Timer Interrupt
Selain interrupt yang dipicu oleh perubahan sinyal eksternal, ESP32 memiliki **hardware timer** internal yang dapat memicu interrupt secara **periodik** berdasarkan hitungan waktu, tanpa bergantung pada sinyal dari luar. Timer interrupt memungkinkan tugas periodik (mis. membaca sensor tiap interval tertentu) dijalankan secara presisi **tanpa memblokir** program utama — berbeda dengan `delay()` yang menghentikan seluruh eksekusi program selama periode tunggu.

### C.3 Watchdog Timer (WDT)
Watchdog Timer adalah timer khusus yang akan **me-reset sistem secara otomatis** jika tidak "diberi makan" (direset ulang) dalam periode waktu tertentu. Tujuannya adalah menjaga keandalan sistem embedded — jika program mengalami *hang* (macet, mis. akibat infinite loop atau kondisi tak terduga), watchdog akan mendeteksi bahwa sistem tidak lagi responsif dan memicu reset otomatis agar sistem kembali berjalan normal, alih-alih macet tanpa batas waktu.

### C.4 Multitasking dengan FreeRTOS
Arduino-ESP32 core dibangun di atas **FreeRTOS**, sebuah RTOS (Real-Time Operating System) yang memungkinkan beberapa **task** berjalan secara "paralel" (sebenarnya bergantian sangat cepat oleh scheduler, atau benar-benar paralel karena ESP32 memiliki dua core CPU). Setiap task memiliki fungsi, ukuran stack, dan prioritas masing-masing, dibuat menggunakan `xTaskCreate()`/`xTaskCreatePinnedToCore()`. Task yang berjalan pada RTOS umumnya tidak menggunakan `delay()` biasa, melainkan `vTaskDelay()` agar tidak memblokir task lain secara tidak perlu. Komunikasi antar task dapat dilakukan melalui **queue** (`xQueueSend()`/`xQueueReceive()`), memungkinkan satu task mengirim data ke task lain secara aman.

---

## D. Persiapan Sebelum Praktikum

1. Pastikan PlatformIO sudah terinstal (lihat **Modul 1, Bagian D.1–D.2**)
2. Buat project baru untuk Modul 4:
   - Name: `modul4-interrupt-timer-rtos`
   - Board: **"Espressif ESP32 Dev Module"**
   - Framework: **Arduino**
3. Pastikan `platformio.ini` berisi:
   ```ini
   [env:esp32dev]
   platform = espressif32
   board = esp32dev
   framework = arduino
   ```
4. Tidak ada library eksternal tambahan yang dibutuhkan pada modul ini — seluruh fitur (interrupt, timer, watchdog, FreeRTOS) sudah tersedia dalam Arduino-ESP32 core

---

## E. Kegiatan Praktikum

### PERCOBAAN 1 — External Interrupt pada Kondisi Sistem Hang (Manual Recovery)

**Tujuan:**
Mahasiswa mampu membuktikan bahwa external interrupt tetap dapat berjalan meskipun program utama (`loop()`) mengalami hang/macet total, serta memanfaatkannya sebagai mekanisme pemulihan manual (manual recovery) yang melengkapi watchdog timer otomatis pada Percobaan 4.

> **Konsep kunci:** Selama interrupt global tidak dinonaktifkan (mis. melalui `noInterrupts()` atau critical section yang berkepanjangan), CPU akan tetap menjalankan ISR kapan pun trigger-nya terjadi — **bahkan jika `loop()` sedang terjebak dalam `while(true) {}` atau kondisi blocking lainnya**. Ini karena interrupt bekerja di level hardware yang independen dari kode apa yang sedang dieksekusi CPU, selama interrupt tersebut tidak sedang di-mask.

**Skema Rangkaian:**

| Komponen | Pin ESP32 | Keterangan |
|---|---|---|
| LED (+resistor 220Ω) | GPIO 2 | Sebagai indikator visual sistem berjalan normal (akan dipakai kembali pada Percobaan 3 — Timer Interrupt) |
| Pushbutton "Darurat" | GPIO 27 | `INPUT_PULLUP`, dipasang ke interrupt (trigger `FALLING`) |

**Langkah Kerja:**
1. Rangkai LED dan pushbutton sesuai skema
2. Implementasikan program normal: LED berkedip + pesan status dicetak ke Serial secara berkala
3. Pasang ISR pada pushbutton yang **langsung memanggil `esp_restart()`** (bukan sekadar mengatur flag), dengan tombol berfungsi sebagai "tombol darurat"
4. Jalankan program, uji tombol darurat dalam kondisi normal — pastikan board berhasil restart
5. Aktifkan simulasi hang dengan meng-uncomment blok `while (true) {}` pada `loop()`, amati bahwa LED berhenti berkedip dan Serial berhenti mencetak pesan (sistem tampak benar-benar macet)
6. **Saat sistem dalam kondisi hang tersebut**, tekan tombol darurat — amati bahwa board tetap berhasil restart meskipun `loop()` sedang terjebak total
7. Diskusikan: bandingkan mekanisme ini dengan Watchdog Timer (Percobaan 4) — kapan sebaiknya masing-masing digunakan?

**Kode Program (Tombol Darurat — Restart Manual Saat Sistem Hang):**
```cpp
#include <Arduino.h>
#include <esp_system.h>

#define BUTTON_PIN 27
#define LED_PIN 2

void IRAM_ATTR emergencyISR()
{
    // Pengecualian terhadap aturan "ISR harus singkat": karena loop()
    // mungkin sedang hang, restart HARUS dilakukan langsung di dalam ISR
    // agar benar-benar dapat memulihkan sistem kapan pun dibutuhkan
    esp_restart();
}

void setup()
{
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), emergencyISR, FALLING);

    Serial.println("Sistem berjalan normal. Tombol darurat siap kapan saja.");
}

void loop()
{
    digitalWrite(LED_PIN, HIGH);
    delay(300);
    digitalWrite(LED_PIN, LOW);
    delay(300);
    Serial.println("loop() berjalan normal...");

    // Uncomment blok berikut untuk mensimulasikan sistem hang:
    // Serial.println("Mensimulasikan hang! LED & Serial akan berhenti...");
    // while (true) {
    //     // program terjebak di sini tanpa henti — coba tekan tombol darurat sekarang
    // }
}
```

**Penjelasan Kode:**
| Bagian | Penjelasan |
|---|---|
| `esp_restart()` dipanggil langsung di `emergencyISR()` | Berbeda dari praktik umum (ISR hanya set flag, aksi berat dilakukan di `loop()`), di sini aksi pemulihan **harus** terjadi di dalam ISR — karena jika `loop()` sedang hang, ia tidak akan pernah sempat memeriksa flag apa pun |
| `while (true) {}` (disimulasikan) | Mewakili kondisi hang nyata (mis. akibat bug, sensor yang tidak merespons, atau kondisi tak terduga lainnya) — program benar-benar berhenti merespons pada `loop()` |
| Tombol darurat tetap berfungsi saat hang | Membuktikan bahwa ISR berjalan independen dari status `loop()`, selama interrupt global tidak dinonaktifkan |
| Perbandingan dengan Watchdog (Percobaan 4) | Watchdog mendeteksi hang secara **otomatis** berdasarkan timeout tanpa perlu campur tangan manusia; tombol darurat bersifat **manual** (perlu ditekan pengguna) namun dapat merespons **lebih cepat dan kapan saja**, tanpa menunggu periode timeout — keduanya saling melengkapi, bukan saling menggantikan |

---

### PERCOBAAN 2 — External Interrupt: Decoding Quadrature Encoder (Pulsa & Arah)

**Tujuan:**
Mahasiswa mampu mengimplementasikan external interrupt untuk melakukan decoding sinyal quadrature encoder (2 channel), menghitung jumlah pulsa sekaligus arah putaran secara periodik.

> **Catatan:** Konversi jumlah pulsa menjadi kecepatan putar (RPM) — termasuk kalibrasi pulsa per putaran dan filtering — akan dibahas pada **Modul 5**. Percobaan ini berfokus murni pada penerapan external interrupt untuk decoding quadrature.

**Skema Rangkaian:**

| Komponen | Pin ESP32 | Keterangan |
|---|---|---|
| Encoder — Channel A (C1) | GPIO 32 | `INPUT_PULLUP`, dipasang ke interrupt (trigger `RISING`) — pada modul referensi: kabel **Kuning** |
| Encoder — Channel B (C2) | GPIO 33 | `INPUT_PULLUP`, dibaca di dalam ISR untuk menentukan arah — pada modul referensi: kabel **Hijau** |
| Encoder — VCC | 3.3V–5V (sesuai modul) | Pada modul referensi: kabel **Biru** |
| Encoder — GND | GND | Pada modul referensi: kabel **Hitam** |

> Modul motor+encoder dengan konektor JST 6-pin umumnya memiliki pasangan kabel: **Merah/Putih** untuk daya motor (+/−, tidak digunakan pada percobaan ini), **Kuning/Hijau** untuk Channel A/Channel B encoder, dan **Biru/Hitam** untuk VCC/GND encoder — mengacu pada motor referensi JGB37-520 ([Instructables: Closed-Loop Speed Control of a DC Motor With Encoder](https://www.instructables.com/Closed-Loop-Speed-Control-of-a-DC-Motor-With-Encod/)). **Pemetaan warna kabel dapat berbeda antar produsen** — selalu verifikasi ulang pada modul fisik Anda (mis. dengan multimeter untuk memastikan pasangan motor, dan uji langsung tiap kabel sinyal sambil memutar poros untuk memastikan mana Channel A/B) sebelum menyambungkan ke ESP32.

**Langkah Kerja:**
1. Rangkai encoder sesuai skema — hanya kabel encoder (VCC, GND, Channel A, Channel B) yang perlu disambungkan, kabel motor tidak digunakan pada percobaan ini
2. Implementasikan ISR pada Channel A (trigger `RISING`) yang membaca status Channel B untuk menentukan arah putaran (decoding quadrature)
3. Gunakan `portMUX_TYPE` beserta critical section untuk melindungi variabel `encoderCount` yang diakses bersama oleh ISR dan `loop()`
4. Tampilkan jumlah pulsa dan arah putaran secara periodik (setiap 100ms) menggunakan pola `millis()` non-blocking
5. Amati nilai pulsa dan arah pada Serial Monitor sambil memutar poros motor searah dan berlawanan arah jarum jam

**Kode Program (Decoding Quadrature Encoder via External Interrupt):**
```cpp
#include <Arduino.h>

#define ENCODER_A_PIN 32
#define ENCODER_B_PIN 33

volatile int32_t encoderCount = 0;
portMUX_TYPE encoderMux = portMUX_INITIALIZER_UNLOCKED;

int32_t previousCount = 0;
unsigned long previousMillis = 0;

void IRAM_ATTR encoderA_ISR()
{
    bool channelB = digitalRead(ENCODER_B_PIN);

    portENTER_CRITICAL_ISR(&encoderMux);
    if (channelB == HIGH)
    {
        encoderCount++; // arah maju (CW)
    }
    else
    {
        encoderCount--; // arah mundur (CCW)
    }
    portEXIT_CRITICAL_ISR(&encoderMux);
}

void setup()
{
    Serial.begin(115200);

    pinMode(ENCODER_A_PIN, INPUT_PULLUP);
    pinMode(ENCODER_B_PIN, INPUT_PULLUP);

    attachInterrupt(
        digitalPinToInterrupt(ENCODER_A_PIN),
        encoderA_ISR,
        RISING);

    Serial.println("ESP32 Quadrature Encoder Decoder (External Interrupt)");
}

void loop()
{
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= 100)
    {
        previousMillis = currentMillis;

        int32_t currentCount;
        portENTER_CRITICAL(&encoderMux);
        currentCount = encoderCount;
        portEXIT_CRITICAL(&encoderMux);

        int32_t deltaCount = currentCount - previousCount;
        previousCount = currentCount;

        Serial.printf("Total Pulsa: %ld | Delta: %ld | Arah: %s\n",
                      (long)currentCount,
                      (long)deltaCount,
                      deltaCount > 0 ? "Maju (CW)" : (deltaCount < 0 ? "Mundur (CCW)" : "Diam"));
    }
}
```

**Penjelasan Kode:**
| Bagian | Penjelasan |
|---|---|
| `attachInterrupt(..., encoderA_ISR, RISING)` | Interrupt hanya dipasang pada Channel A; Channel B dibaca langsung (bukan interrupt) di dalam ISR untuk menentukan arah |
| Logika arah (`channelB == HIGH`) | Prinsip decoding quadrature — urutan/fase antara Channel A dan Channel B saat transisi menentukan apakah poros berputar maju atau mundur |
| `portMUX_TYPE encoderMux` + `portENTER_CRITICAL_ISR()`/`portEXIT_CRITICAL_ISR()` | Melindungi akses ke `encoderCount` dari dalam ISR terhadap kemungkinan akses bersamaan oleh core lain — lebih aman dibanding `noInterrupts()`/`interrupts()` pada ESP32 dual-core |
| `portENTER_CRITICAL()`/`portEXIT_CRITICAL()` (di `loop()`) | Versi critical section untuk kode di luar ISR, digunakan saat membaca nilai `encoderCount` agar tidak bentrok dengan ISR yang mungkin berjalan di core lain |
| `deltaCount` (dibandingkan tiap 100ms) | Selisih jumlah pulsa antar periode — tandanya (positif/negatif/nol) menunjukkan arah putaran saat ini; nilai mentah ini yang akan diolah lebih lanjut menjadi RPM pada Modul 5 |

---

### PERCOBAAN 3 — Timer Interrupt

**Tujuan:**
Mahasiswa mampu mengimplementasikan timer interrupt untuk menjalankan tugas periodik (toggle LED) tanpa memblokir program utama.

**Skema Rangkaian:**

| Komponen | Pin ESP32 | Keterangan |
|---|---|---|
| LED (+resistor 220Ω) | GPIO 2 | Anoda ke GPIO, katoda ke GND melalui resistor |

**Langkah Kerja:**
1. Rangkai LED sesuai skema
2. Implementasikan hardware timer yang memicu interrupt setiap 500ms
3. Di dalam ISR, cukup set sebuah flag (`volatile bool`) — jangan langsung memanggil `digitalWrite()` di dalam ISR
4. Pada `loop()`, periksa flag tersebut dan lakukan toggle LED jika flag aktif
5. Amati bahwa LED tetap berkedip konsisten meskipun ditambahkan kode lain pada `loop()` (mis. `Serial.println()` tambahan)

**Kode Program (Timer Interrupt — Toggle LED):**
```cpp
#include <Arduino.h>

#define LED_PIN 2

hw_timer_t *timer = NULL;
volatile bool toggleFlag = false;

void IRAM_ATTR onTimer()
{
    toggleFlag = true;
}

void setup()
{
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);

    timer = timerBegin(1000000);       // frekuensi timer 1 MHz -> 1 tick = 1 us
    timerAttachInterrupt(timer, &onTimer);
    timerAlarm(timer, 500000, true, 0); // alarm setiap 500.000 tick (500ms), autoreload

    Serial.println("Timer interrupt aktif, LED akan toggle tiap 500ms");
}

void loop()
{
    if (toggleFlag)
    {
        toggleFlag = false;
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        Serial.println("Timer interrupt terpicu");
    }
}
```

**Penjelasan Kode:**
| Bagian | Penjelasan |
|---|---|
| `timerBegin(1000000)` | Menginisialisasi hardware timer dengan frekuensi tick 1 MHz (setiap tick = 1 mikrodetik) |
| `timerAttachInterrupt(timer, &onTimer)` | Mendaftarkan fungsi `onTimer()` sebagai ISR yang dipanggil setiap kali timer mencapai nilai alarm |
| `timerAlarm(timer, 500000, true, 0)` | Mengatur nilai alarm 500.000 tick (setara 500ms pada frekuensi 1MHz), `true` mengaktifkan autoreload (berulang otomatis) |
| `toggleFlag` (flag, bukan langsung `digitalWrite` di ISR) | Praktik terbaik — ISR hanya menandai bahwa event terjadi, sedangkan aksi yang lebih "berat" (toggle LED, print) dilakukan di `loop()` |

---

### PERCOBAAN 4 — Watchdog Timer

**Tujuan:**
Mahasiswa mampu mengimplementasikan watchdog timer untuk mendeteksi dan memulihkan sistem dari kondisi hang.

**Langkah Kerja:**
1. Implementasikan inisialisasi Task Watchdog Timer dengan timeout 3 detik
2. Jalankan program normal — di dalam `loop()`, "beri makan" watchdog secara berkala (`esp_task_wdt_reset()`)
3. Amati Serial Monitor, program berjalan normal tanpa reset
4. Simulasikan kondisi hang dengan menambahkan `while(true) {}` pada satu titik di `loop()` (tanpa memanggil reset watchdog), amati bahwa board akan **restart otomatis** setelah timeout tercapai
5. Hapus kembali baris simulasi hang setelah pengamatan selesai

**Kode Program (Watchdog Timer):**
```cpp
#include <Arduino.h>
#include <esp_task_wdt.h>

#define WDT_TIMEOUT_S 3

void setup()
{
    Serial.begin(115200);

    esp_task_wdt_config_t wdtConfig = {
        .timeout_ms = WDT_TIMEOUT_S * 1000,
        .idle_core_mask = 0,
        .trigger_panic = true,
    };
    esp_task_wdt_init(&wdtConfig);
    esp_task_wdt_add(NULL); // daftarkan task saat ini (loop utama) ke watchdog

    Serial.println("Watchdog Timer aktif (timeout 3 detik)");
}

void loop()
{
    Serial.println("Sistem berjalan normal");
    esp_task_wdt_reset(); // "memberi makan" watchdog
    delay(1000);

    // Uncomment baris berikut untuk mensimulasikan sistem hang:
    // while (true) {}
}
```

**Penjelasan Kode:**
| Bagian | Penjelasan |
|---|---|
| `esp_task_wdt_config_t` | Struktur konfigurasi watchdog — `timeout_ms` menentukan batas waktu, `trigger_panic = true` membuat sistem restart otomatis saat timeout terlampaui |
| `esp_task_wdt_add(NULL)` | Mendaftarkan task yang sedang berjalan (dalam hal ini loop utama Arduino) agar diawasi oleh watchdog |
| `esp_task_wdt_reset()` | "Memberi makan" watchdog — memberi tahu sistem bahwa task masih berjalan normal, sehingga timer watchdog direset kembali ke 0 |
| `while (true) {}` (disimulasikan) | Program berhenti merespons — karena `esp_task_wdt_reset()` tidak lagi terpanggil, watchdog akan timeout dan me-restart sistem secara otomatis |

---

### PERCOBAAN 5 — Multitasking dengan FreeRTOS

**Tujuan:**
Mahasiswa mampu mengimplementasikan beberapa task yang berjalan "paralel" menggunakan FreeRTOS, serta komunikasi antar task melalui queue.

**Skema Rangkaian:**
Gunakan rangkaian LED dari Percobaan 3 (GPIO 2), tambahkan pushbutton pada GPIO 27 (`INPUT_PULLUP`) untuk latihan tambahan.

**Langkah Kerja:**
1. Implementasikan dua task terpisah: satu untuk toggle LED (setiap 500ms), satu untuk mencetak pesan ke Serial (setiap 1 detik)
2. Jalankan kedua task menggunakan `xTaskCreatePinnedToCore()`, masing-masing pada core yang berbeda (core 0 dan core 1)
3. Amati bahwa kedua task berjalan bersamaan secara independen — tidak saling menunggu
4. **Latihan tambahan:** tambahkan task ketiga yang membaca status pushbutton dan mengirim jumlah penekanan melalui **queue** ke task lain yang bertugas menampilkannya di Serial Monitor

**Kode Program (Dua Task Paralel — LED & Serial):**
```cpp
#include <Arduino.h>

#define LED_PIN 2

void taskBlink(void *pvParameters)
{
    pinMode(LED_PIN, OUTPUT);
    while (true)
    {
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void taskSerial(void *pvParameters)
{
    while (true)
    {
        Serial.println("Task Serial berjalan...");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void setup()
{
    Serial.begin(115200);

    xTaskCreatePinnedToCore(taskBlink, "TaskBlink", 2048, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(taskSerial, "TaskSerial", 2048, NULL, 1, NULL, 0);
}

void loop()
{
    // Kosong — seluruh pekerjaan dilakukan oleh task FreeRTOS
}
```

**Kode Program — Latihan Tambahan (Komunikasi Antar Task via Queue):**
```cpp
#include <Arduino.h>

#define BUTTON_PIN 27

QueueHandle_t pressQueue;

void taskReadButton(void *pvParameters)
{
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    int lastState = HIGH;
    int pressCount = 0;

    while (true)
    {
        int reading = digitalRead(BUTTON_PIN);
        if (reading == LOW && lastState == HIGH)
        {
            pressCount++;
            xQueueSend(pressQueue, &pressCount, portMAX_DELAY);
        }
        lastState = reading;
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void taskDisplayCount(void *pvParameters)
{
    int receivedCount;
    while (true)
    {
        if (xQueueReceive(pressQueue, &receivedCount, portMAX_DELAY) == pdTRUE)
        {
            Serial.printf("Tombol ditekan! Total: %d\n", receivedCount);
        }
    }
}

void setup()
{
    Serial.begin(115200);

    pressQueue = xQueueCreate(10, sizeof(int));

    xTaskCreatePinnedToCore(taskReadButton, "TaskReadButton", 2048, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(taskDisplayCount, "TaskDisplayCount", 2048, NULL, 1, NULL, 0);
}

void loop()
{
}
```

**Penjelasan Kode:**
| Bagian | Penjelasan |
|---|---|
| `xTaskCreatePinnedToCore(fn, name, stackSize, param, priority, handle, coreID)` | Membuat task baru, `coreID` menentukan task dijalankan pada core 0 atau core 1 secara spesifik |
| `vTaskDelay(pdMS_TO_TICKS(ms))` | Delay berbasis FreeRTOS tick — memberi kesempatan task lain untuk berjalan selama periode tunggu, tidak memblokir seperti `delay()` |
| `QueueHandle_t` / `xQueueCreate()` | Membuat queue sebagai sarana komunikasi antar task, menyimpan data (di sini bertipe `int`) secara thread-safe |
| `xQueueSend(pressQueue, &pressCount, portMAX_DELAY)` | Mengirim data ke queue dari task pembaca tombol; `portMAX_DELAY` berarti menunggu tanpa batas waktu jika queue penuh |
| `xQueueReceive(pressQueue, &receivedCount, portMAX_DELAY)` | Menerima data dari queue pada task penampil — task ini "tidur" (tidak memakan CPU) selama menunggu data baru masuk |

**Tugas Akhir Modul 4:**
Rancang sistem yang menggabungkan seluruh topik modul ini dalam satu program: dilengkapi tombol darurat berbasis external interrupt (Percobaan 1) yang dapat me-restart sistem kapan pun dibutuhkan, task pembaca encoder (external interrupt, Percobaan 2) berjalan pada satu core, task timer periodik untuk logging status ke Serial setiap detik (Percobaan 3) pada core lain, watchdog timer aktif untuk menjaga keandalan sistem (Percobaan 4), dan seluruhnya dikelola sebagai task-task FreeRTOS terpisah (Percobaan 5).

---

## F. Format Laporan Praktikum

1. **Cover** — judul modul, nama, NIM, kelas
2. **Tujuan Praktikum**
3. **Dasar Teori Singkat** (parafrase, bukan salinan modul)
4. **Alat dan Bahan**
5. **Langkah Kerja & Skema Rangkaian** (sertakan foto rangkaian nyata)
6. **Kode Program** (lengkap, dengan komentar)
7. **Hasil dan Pembahasan** (data pengamatan, jawaban pertanyaan analisis, hasil tugas akhir modul)
8. **Kesimpulan**
9. **Lampiran** (foto/video demo, dokumentasi tambahan)

---

## G. Rubrik Penilaian

| Aspek | Bobot | Kriteria |
|---|---|---|
| Implementasi manual recovery via external interrupt | 20% | Tombol darurat berfungsi memulihkan sistem meskipun `loop()` dalam kondisi hang, analisis perbandingan dengan watchdog tepat |
| Implementasi external interrupt (encoder) | 15% | Penghitungan pulsa dan deteksi arah berjalan benar, ISR ditulis sesuai praktik terbaik |
| Implementasi timer interrupt | 15% | LED toggle konsisten via timer, pola flag+loop diterapkan dengan benar |
| Implementasi watchdog timer | 20% | Watchdog berfungsi, sistem berhasil diamati restart otomatis saat disimulasikan hang |
| Implementasi multitasking FreeRTOS | 15% | Dua task (atau lebih dengan queue) berjalan paralel dengan benar |
| Laporan & analisis | 15% | Kelengkapan, kedalaman analisis, kerapian dokumentasi |

---

## H. Referensi
1. Espressif Systems, *ESP32 Technical Reference Manual — Interrupt, Timer, Task Watchdog*
2. Espressif Systems, *ESP-IDF Programming Guide — FreeRTOS, Task Watchdog Timer*, https://docs.espressif.com/projects/esp-idf/
3. Arduino-ESP32 Core Documentation — Timer, https://docs.espressif.com/projects/arduino-esp32/
4. FreeRTOS Official Documentation — Queue Management, https://www.freertos.org/
5. sss2022, *Closed-Loop Speed Control of a DC Motor With Encoder Using a Discrete PI Controller on Arduino*, Instructables — rujukan wiring encoder & motor JGB37-520, https://www.instructables.com/Closed-Loop-Speed-Control-of-a-DC-Motor-With-Encod/

---

## I. Kumpulan Pertanyaan

### Pertanyaan Pra-Praktikum
1. Jelaskan perbedaan mendasar antara external interrupt dan timer interrupt dalam hal apa yang memicu terjadinya interrupt
2. Mengapa watchdog timer penting pada sistem embedded yang beroperasi tanpa pengawasan manusia secara terus-menerus (mis. di lapangan/lokasi terpencil)?
3. Jelaskan mengapa multitasking pada FreeRTOS disebut sebagai "paralel semu" (pseudo-parallel) pada single-core, namun dapat benar-benar paralel pada ESP32 yang memiliki dua core

### Pertanyaan/Analisis Percobaan 1 — External Interrupt pada Kondisi Sistem Hang
1. Jelaskan mengapa ISR pada percobaan ini memanggil `esp_restart()` secara langsung, alih-alih hanya mengatur flag seperti pada Percobaan 3 (Timer Interrupt)
2. Berdasarkan pengamatan, apakah tombol darurat tetap berhasil me-restart sistem saat `loop()` sedang terjebak dalam `while(true) {}`? Jelaskan mengapa hal ini bisa terjadi
3. Bandingkan Watchdog Timer (Percobaan 4) dengan tombol darurat berbasis interrupt pada percobaan ini — sebutkan satu skenario di mana masing-masing lebih unggul dibanding yang lain
4. Dalam kondisi apa mekanisme pemulihan berbasis interrupt pada percobaan ini **tidak akan berfungsi**? (Petunjuk: kaitkan dengan konsep `noInterrupts()`/critical section pada C.1)

### Pertanyaan/Analisis Percobaan 2 — External Interrupt (Encoder)
1. Jelaskan bagaimana pembacaan Channel B di dalam ISR Channel A dapat menentukan arah putaran encoder (prinsip decoding quadrature)
2. Mengapa `portMUX_TYPE` beserta `portENTER_CRITICAL_ISR()`/`portENTER_CRITICAL()` digunakan alih-alih `noInterrupts()`/`interrupts()` pada ESP32? Jelaskan kaitannya dengan arsitektur dual-core ESP32
3. Mengapa pembacaan dan pelaporan nilai pulsa dilakukan menggunakan pola `millis()` non-blocking dengan interval 100ms, bukan `delay()` di dalam `loop()`?
4. Informasi apa saja yang masih dibutuhkan agar nilai pulsa mentah pada percobaan ini dapat dikonversi menjadi kecepatan putar (RPM)? (Tidak perlu diimplementasikan — cukup jelaskan secara konsep, akan dibahas pada Modul 5)

### Pertanyaan/Analisis Percobaan 3 — Timer Interrupt
1. Jelaskan mengapa ISR pada percobaan ini (`onTimer()`) hanya mengatur flag `toggleFlag`, tidak langsung memanggil `digitalWrite()`
2. Apa perbedaan hasil yang akan teramati jika parameter `autoreload` pada `timerAlarm()` diatur `false`?
3. Bandingkan konsistensi interval LED berkedip antara pendekatan timer interrupt pada percobaan ini dengan pendekatan `delay()` biasa — jelaskan skenario di mana `delay()` dapat menyebabkan interval menjadi tidak presisi

### Pertanyaan/Analisis Percobaan 4 — Watchdog Timer
1. Jelaskan apa yang akan terjadi jika `esp_task_wdt_reset()` dihapus dari `loop()`, meskipun tidak ada simulasi `while(true){}`
2. Mengapa nilai timeout watchdog perlu dipilih dengan cermat — jelaskan risiko jika nilainya terlalu singkat maupun terlalu lama
3. Sebutkan satu skenario nyata pada sistem embedded di mana watchdog timer berperan penting untuk menjaga keandalan sistem

### Pertanyaan/Analisis Percobaan 5 — Multitasking FreeRTOS
1. Jelaskan apa yang akan terjadi jika kedua task pada kode utama (`taskBlink` dan `taskSerial`) dijalankan pada core yang sama (`coreID` yang sama), dibandingkan pada core berbeda
2. Mengapa `vTaskDelay()` digunakan alih-alih `delay()` di dalam task FreeRTOS?
3. Jelaskan keuntungan penggunaan queue (`xQueueSend`/`xQueueReceive`) untuk komunikasi antar task dibandingkan menggunakan variabel global biasa
