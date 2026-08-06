# MODUL 1
# PENGANTAR MIKROKONTROLER & DASAR PEMROGRAMAN

**Mata Kuliah:** Praktikum Mikrokontroler & Embedded System
**Alokasi Waktu:** 4 x Percobaan (@ 100–150 menit)
**Platform:** STM32 Blackpill (Framework HAL) & ESP32 (Framework ESP-IDF dan Arduino)
**IDE:** VSCode + PlatformIO

---

## A. Capaian Pembelajaran

Setelah menyelesaikan Modul 1, praktikan mampu:
1. Melakukan instalasi dan konfigurasi PlatformIO pada VSCode sebagai lingkungan pengembangan
2. Menjelaskan arsitektur board STM32 Blackpill dan ESP32, serta membedakan karakteristik framework HAL, ESP-IDF, dan Arduino
3. Melakukan flashing program Blink ke STM32 Blackpill menggunakan ST-Link (framework HAL)
4. Melakukan flashing program Blink ke ESP32 menggunakan framework ESP-IDF
5. Mengidentifikasi dan membedakan berbagai jenis tombol/saklar (NO, NC, toggle switch, DIP switch, matrix button)
6. Mengimplementasikan pembacaan GPIO untuk setiap jenis tombol pada ESP32 menggunakan framework Arduino
7. Mengimplementasikan debouncing pada input tombol berbasis GPIO

---

## B. Alat dan Bahan

| No | Nama Komponen | Spesifikasi | Jumlah |
|---|---|---|---|
| 1 | Board STM32 Blackpill | STM32F401CCU6 / STM32F411CEU6 | 1 |
| 2 | ST-Link V2 | programmer/debugger via SWD | 1 |
| 3 | Board ESP32 DevKit | ESP32 DevKit v1 | 1 |
| 4 | Kabel jumper female-female | untuk ST-Link ke Blackpill | 4 |
| 5 | Kabel USB Micro/USB-C | untuk ST-Link dan ESP32 ke PC | 2 |
| 6 | Breadboard | 830 titik | 1 |
| 7 | Kabel jumper male-male | — | secukupnya |
| 8 | LED + resistor 220Ω | untuk uji Blink ESP32 (bila tidak ada LED onboard) | 1 |
| 9 | Pushbutton (tactile) | Normally Open (NO) | 2 |
| 10 | Limit switch / microswitch | Normally Close (NC) | 1 |
| 11 | Toggle/slide switch | 2 posisi (latching) | 1 |
| 12 | DIP switch | 4-bit atau 8-bit | 1 |
| 13 | Matrix keypad | 4x4 | 1 |
| 14 | Resistor | 10kΩ (pull-up/pull-down eksternal jika diperlukan) | secukupnya |
| 15 | Laptop/PC | Windows/Mac/Linux, VSCode terinstal | 1 |

---

## C. Dasar Teori

### C.1 PlatformIO dan VSCode sebagai Lingkungan Pengembangan
PlatformIO adalah ekosistem pengembangan embedded yang bersifat cross-platform, mendukung ratusan board dan berbagai framework (Arduino, STM32Cube/HAL, ESP-IDF, dll.) dalam satu antarmuka yang seragam. Satu keunggulan penting PlatformIO adalah kemampuannya menjalankan **framework berbeda pada chip yang sama** — misalnya ESP32 dapat diprogram baik menggunakan framework Arduino (sederhana) maupun ESP-IDF (native, lebih mendalam) — cukup dengan mengubah konfigurasi pada `platformio.ini`.

Struktur project PlatformIO terdiri atas:
- `platformio.ini` — file konfigurasi board, framework, dan upload protocol
- `src/` — folder berisi kode program utama
- `include/` — folder header file
- `lib/` — folder library tambahan

### C.2 STM32 Blackpill dan Framework HAL
Blackpill adalah development board berbasis mikrokontroler STM32F401/F411 (ARM Cortex-M4, 32-bit). Karakteristik utama:
- CPU ARM Cortex-M4 dengan FPU, clock hingga 84–100 MHz
- Memori Flash 256–512 KB, RAM 64–128 KB
- LED onboard di pin **PC13** (aktif LOW), tombol user di pin **PA0**
- Tidak memiliki programmer onboard — memerlukan **ST-Link** eksternal via protokol **SWD** (SWDIO, SWCLK, GND, 3.3V)

**HAL (Hardware Abstraction Layer)** adalah pustaka resmi STMicroelectronics yang menyediakan fungsi standar untuk mengakses peripheral tanpa menulis langsung ke register, namun tetap memberi kontrol rinci. Ciri khasnya: diawali `HAL_Init()`, konfigurasi peripheral menggunakan struct (mis. `GPIO_InitTypeDef`), dan clock peripheral harus diaktifkan eksplisit (mis. `__HAL_RCC_GPIOC_CLK_ENABLE()`).

### C.3 ESP32 dan Framework ESP-IDF
ESP32 adalah mikrokontroler 32-bit dual-core (Xtensa LX6) dengan WiFi dan Bluetooth terintegrasi, serta dilengkapi USB-to-Serial bawaan sehingga dapat langsung diprogram melalui kabel USB tanpa programmer eksternal.

**ESP-IDF (Espressif IoT Development Framework)** adalah framework resmi dan native dari Espressif, dibangun di atas FreeRTOS. Berbeda dengan framework Arduino yang menyederhanakan program menjadi `setup()` dan `loop()`, ESP-IDF menggunakan struktur berbasis **task/component** dengan titik masuk program berupa fungsi `app_main()`. ESP-IDF memberi akses lebih penuh ke fitur ESP32 (mis. konfigurasi low-level WiFi, task scheduling FreeRTOS secara langsung) dan umum digunakan pada pengembangan produk IoT tingkat lanjut.

| Aspek | Framework Arduino | Framework ESP-IDF |
|---|---|---|
| Titik masuk program | `setup()` dan `loop()` | `app_main()` |
| Delay/timing | `delay(ms)` | `vTaskDelay(pdMS_TO_TICKS(ms))` |
| Tingkat abstraksi | Tinggi (mudah dipelajari) | Rendah–menengah (lebih native, lebih kuat) |
| Akses fitur RTOS | Tersembunyi/terbatas | Langsung (task, semaphore, queue) |

### C.4 Macam-Macam Tombol/Saklar
- **Normally Open (NO):** kontak *terbuka* saat idle, *tertutup* saat ditekan. Umum digunakan sebagai pushbutton momentary
- **Normally Close (NC):** kebalikan dari NO — kontak *tertutup* saat idle, *terbuka* saat ditekan/aktif. Umum pada limit switch untuk aplikasi safety
- **Toggle/Slide Switch:** bersifat *latching* — mempertahankan posisi terakhir tanpa perlu ditahan
- **DIP Switch:** kumpulan beberapa saklar kecil dalam satu paket, umum untuk pengaturan konfigurasi (mis. addressing)
- **Matrix Button (Keypad):** susunan tombol dalam format baris-kolom (mis. keypad 4x4 = 16 tombol), dibaca dengan teknik *scanning* sehingga jumlah pin yang dibutuhkan jauh lebih sedikit

### C.5 Debouncing
Kontak mekanik pada tombol/saklar menghasilkan beberapa transisi sinyal HIGH-LOW dalam waktu sangat singkat akibat getaran fisik saat kontak bersentuhan/terlepas ("bouncing"). Tanpa penanganan, satu kali aksi tekan dapat terbaca sebagai beberapa kali event. **Debouncing** memastikan hanya satu transisi valid yang terdeteksi, dengan menunggu sinyal stabil selama periode waktu tertentu (mis. 20–50ms) sebelum event dianggap sah.

---

## D. Persiapan Sebelum Praktikum

### D.1 Instalasi Visual Studio Code

1. Buka browser, kunjungi situs resmi Visual Studio Code: `https://code.visualstudio.com/`
2. Klik tombol **Download** sesuai sistem operasi yang digunakan (Windows/Mac/Linux)

   *[Gambar 1: Tampilan halaman download Visual Studio Code]*
3. Jalankan file installer yang telah diunduh
4. Ikuti proses instalasi: setujui lisensi (*I accept the agreement*) → **Next**
5. Pada halaman **Select Additional Tasks**, disarankan mencentang opsi:
   - *Add "Open with Code" action to Windows Explorer file context menu*
   - *Add to PATH*

   *[Gambar 2: Halaman Select Additional Tasks pada installer]*
6. Klik **Install**, tunggu proses selesai, lalu klik **Finish** (centang *Launch Visual Studio Code* agar aplikasi langsung terbuka)
7. Pastikan VSCode berhasil terbuka dan menampilkan halaman Welcome

   *[Gambar 3: Tampilan awal VSCode setelah instalasi]*

### D.2 Instalasi Ekstensi PlatformIO IDE

1. Buka VSCode, klik ikon **Extensions** pada sidebar kiri (ikon kotak/persegi bersusun), atau tekan `Ctrl+Shift+X`

   *[Gambar 4: Lokasi ikon Extensions pada sidebar VSCode]*
2. Pada kolom pencarian, ketik **"PlatformIO IDE"**
3. Pilih ekstensi **PlatformIO IDE** yang dipublikasikan oleh **PlatformIO** (biasanya hasil teratas), klik **Install**

   *[Gambar 5: Hasil pencarian ekstensi PlatformIO IDE di Marketplace]*
4. Tunggu proses instalasi hingga selesai — proses ini membutuhkan koneksi internet dan dapat memakan waktu beberapa menit karena PlatformIO Core (Python) akan diunduh dan dikonfigurasi secara otomatis
5. Setelah selesai, VSCode akan meminta untuk **restart/reload window** — klik tombol tersebut jika muncul
6. Setelah VSCode terbuka kembali, akan muncul ikon baru berbentuk semut (PlatformIO) pada sidebar kiri — klik ikon tersebut untuk membuka **PlatformIO Home**

   *[Gambar 6: Ikon PlatformIO pada sidebar dan tampilan PlatformIO Home]*
7. Pastikan halaman **PlatformIO Home** berhasil terbuka tanpa pesan error, sebagai tanda instalasi berhasil

### D.3 Membuat Project Baru di PlatformIO

1. Pada halaman **PlatformIO Home**, klik tombol **"+ New Project"**

   *[Gambar 7: Tombol New Project pada PlatformIO Home]*
2. Isi form yang muncul:
   - **Name:** nama project (mis. `modul1-blink-blackpill`)
   - **Board:** ketik dan pilih board yang sesuai (mis. "Blackpill F411CE" atau "Espressif ESP32 Dev Module")
   - **Framework:** pilih framework sesuai kebutuhan (mis. "STM32Cube (HAL)", "Espressif IoT Development Framework", atau "Arduino")

   *[Gambar 8: Form New Project — pengisian Name, Board, dan Framework]*
3. Klik **Finish**, tunggu PlatformIO membuat struktur project secara otomatis (proses ini mengunduh package board/framework terkait jika belum tersedia di sistem)
4. Setelah selesai, VSCode akan menampilkan struktur folder project pada **Explorer**:
   - `platformio.ini` — file konfigurasi utama
   - `src/` — folder kode program (berisi `main.c` atau `main.cpp`)
   - `include/`, `lib/`, `test/` — folder pendukung

   *[Gambar 9: Struktur folder project PlatformIO pada Explorer VSCode]*

### D.4 Mengenal Toolbar PlatformIO

Pada bagian bawah jendela VSCode (status bar), terdapat ikon-ikon PlatformIO yang akan sering digunakan selama praktikum:

| Ikon | Fungsi |
|---|---|
| ✓ (Build) | Mengompilasi program tanpa mengunggahnya ke board |
| → (Upload) | Mengompilasi dan mengunggah program ke board |
| 🔌 (Serial Monitor) | Membuka jendela Serial Monitor untuk melihat output `Serial.print()`/`printf` |
| 🗑 (Clean) | Membersihkan hasil build sebelumnya |

*[Gambar 10: Toolbar PlatformIO pada status bar VSCode dengan penjelasan tiap ikon]*

### D.5 Instalasi Driver Tambahan

1. Instal driver **ST-Link** (untuk STM32 Blackpill) dari situs resmi STMicroelectronics — **STSW-LINK009**:
   `https://www.st.com/en/development-tools/stsw-link009.html`
   (perlu membuat akun/login gratis di st.com untuk mengunduh)
2. Instal driver USB-to-Serial ESP32, sesuai chip pada board (periksa penanda chip kecil di dekat konektor USB pada board ESP32):
   - Jika chip **CP2102/CP2104** (Silicon Labs): unduh dari
     `https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers`
   - Jika chip **CH340/CH340C/CH341** (WCH): unduh dari situs resmi WCH
     `https://www.wch-ic.com/downloads/CH341SER_ZIP.html`
3. Restart PC/laptop setelah instalasi driver jika diperlukan
4. Siapkan board STM32 Blackpill + ST-Link, dan board ESP32 DevKit, namun **jangan disambungkan terlebih dahulu** sebelum instruksi pada masing-masing percobaan

> **Catatan:** Selalu unduh driver dari situs resmi produsen chip seperti tertulis di atas — hindari mengunduh dari situs pihak ketiga/torrent karena berisiko berisi malware.

---

## E. Kegiatan Praktikum

### PERCOBAAN 1 — Instalasi PlatformIO & Pengenalan STM32 Blackpill (Blink)

**Tujuan:**
Mahasiswa mampu melakukan instalasi PlatformIO, memahami arsitektur board Blackpill, dan berhasil melakukan flashing program Blink menggunakan ST-Link (framework HAL).

**Langkah Kerja:**
1. Pastikan PlatformIO IDE sudah terinstal di VSCode (lihat bagian D)
2. Buat project baru: **PlatformIO Home → New Project**
   - Name: `modul1-blink-blackpill`
   - Board: **"Blackpill F411CE"** (atau **"Blackpill F401CC"** sesuai chip pada board)
   - Framework: **STM32Cube (HAL)**
3. Pastikan `platformio.ini` berisi:
   ```ini
   [env:blackpill_f411ce]
   platform = ststm32
   board = blackpill_f411ce
   framework = stm32cube
   upload_protocol = stlink
   debug_tool = stlink
   ```
4. Sambungkan ST-Link ke Blackpill (SWDIO, SWCLK, GND, 3.3V), lalu ST-Link ke PC via USB
5. Tulis kode Blink berikut pada `src/main.c`
6. **Build**, lalu **Upload** — amati proses flashing pada terminal PlatformIO
7. Amati LED onboard (PC13) berkedip setiap 500ms
8. Diskusi: pinout board Blackpill (GPIO, power, BOOT0, LED onboard, tombol user PA0)

**Kode Program (Blink STM32 HAL):**
```c
#include "stm32f4xx_hal.h"

#define LED_PIN GPIO_PIN_13
#define LED_PORT GPIOC

void GPIO_Init_LED(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOC_CLK_ENABLE();

  GPIO_InitStruct.Pin = LED_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct);
}

int main(void)
{
  HAL_Init();
  GPIO_Init_LED();

  while (1)
  {
    HAL_GPIO_TogglePin(LED_PORT, LED_PIN); // LED onboard aktif LOW
    HAL_Delay(500);
  }
}
```

**Penjelasan Kode:**
| Baris | Penjelasan |
|---|---|
| `__HAL_RCC_GPIOC_CLK_ENABLE()` | Mengaktifkan clock peripheral GPIOC sebelum dapat digunakan |
| `GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP` | Konfigurasi pin sebagai output push-pull |
| `HAL_GPIO_TogglePin()` | Membalik status pin (HIGH↔LOW) setiap kali dipanggil |
| `HAL_Delay(500)` | Fungsi delay bawaan HAL, satuan milidetik |

---

### PERCOBAAN 2 — Pengenalan ESP32 dengan Framework ESP-IDF (Blink)

**Tujuan:**
Mahasiswa mampu memahami arsitektur ESP32 dan struktur program berbasis ESP-IDF, serta berhasil menjalankan program Blink menggunakan framework tersebut.

**Langkah Kerja:**
1. Buat project baru di PlatformIO:
   - Name: `modul1-blink-esp32-idf`
   - Board: **"Espressif ESP32 Dev Module"**
   - Framework: **Espressif IoT Development Framework (ESP-IDF)**
2. Pastikan `platformio.ini` berisi:
   ```ini
   [env:esp32dev]
   platform = espressif32
   board = esp32dev
   framework = espidf
   ```
3. Amati struktur project ESP-IDF (`src/main.c`, fungsi `app_main()`) — bandingkan dengan struktur project HAL pada Percobaan 1
4. Sambungkan ESP32 ke PC via USB, pastikan port terdeteksi
5. Tulis kode Blink berikut pada `src/main.c`
6. **Build** dan **Upload**, amati LED (GPIO 2, onboard pada sebagian besar board ESP32 DevKit) berkedip setiap 500ms

**Kode Program (Blink ESP32 ESP-IDF):**
```c
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED_PIN GPIO_NUM_2

void app_main(void)
{
  gpio_reset_pin(LED_PIN);
  gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

  while (1)
  {
    gpio_set_level(LED_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(500));
    gpio_set_level(LED_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}
```

**Penjelasan Kode:**
| Baris | Penjelasan |
|---|---|
| `app_main(void)` | Titik masuk program pada ESP-IDF, setara dengan `setup()+loop()` pada Arduino, namun berjalan sebagai task FreeRTOS |
| `gpio_set_direction()` | Mengatur arah pin (output), analog dengan `pinMode()` pada Arduino |
| `gpio_set_level()` | Mengatur level tegangan pin, analog dengan `digitalWrite()` |
| `vTaskDelay(pdMS_TO_TICKS(500))` | Delay berbasis FreeRTOS tick, bukan `delay()` biasa — tidak memblokir task lain |

---

### PERCOBAAN 3 — Pengenalan Macam-Macam Tombol (ESP32 + Framework Arduino)

**Tujuan:**
Mahasiswa mampu mengidentifikasi karakteristik berbagai jenis tombol/saklar dan mengimplementasikan pembacaan GPIO untuk masing-masing jenis menggunakan ESP32 dengan framework Arduino.

**Langkah Kerja:**
1. Buat project baru di PlatformIO:
   - Name: `modul1-tombol-esp32-arduino`
   - Board: **"Espressif ESP32 Dev Module"**
   - Framework: **Arduino**
2. Pastikan `platformio.ini` berisi:
   ```ini
   [env:esp32dev]
   platform = espressif32
   board = esp32dev
   framework = arduino
   ```
3. Rangkai dan uji satu per satu jenis tombol sesuai skema di bawah

**Skema Rangkaian:**

| Komponen | Pin ESP32 | Keterangan |
|---|---|---|
| Pushbutton NO | GPIO 25 | Satu kaki ke GPIO, kaki lain ke GND, gunakan `INPUT_PULLUP` |
| Limit switch NC | GPIO 26 | Satu kaki ke GPIO, kaki lain ke GND, gunakan `INPUT_PULLUP` |
| Toggle switch | GPIO 27 | Satu posisi ke GPIO, posisi lain ke GND |
| DIP switch (4-bit) | GPIO 32, 33, 34, 35 | Tiap switch ke satu GPIO |
| Matrix keypad 4x4 | Baris: GPIO 13,12,14,27<br>Kolom: GPIO 26,25,33,32 | Scanning baris-kolom (sesuaikan agar tidak bentrok dengan pin lain) |

**Kode Program (Pushbutton NO & Limit Switch NC):**
```cpp
#define BTN_NO 25
#define SW_NC  26

void setup() {
  Serial.begin(115200);
  pinMode(BTN_NO, INPUT_PULLUP);
  pinMode(SW_NC, INPUT_PULLUP);
}

void loop() {
  bool btnPressed = (digitalRead(BTN_NO) == LOW);  // NO: idle HIGH, ditekan LOW
  bool swActive   = (digitalRead(SW_NC) == HIGH);  // NC: idle LOW, aktif/terputus HIGH

  Serial.printf("NO: %s | NC: %s\n",
                btnPressed ? "DITEKAN" : "idle",
                swActive ? "AKTIF/TERPUTUS" : "idle");
  delay(200);
}
```

**Kode Program (DIP Switch — Baca 4-bit):**
```cpp
int dipPins[4] = {32, 33, 34, 35};

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < 4; i++) pinMode(dipPins[i], INPUT_PULLUP);
}

void loop() {
  uint8_t value = 0;
  for (int i = 0; i < 4; i++) {
    value |= (digitalRead(dipPins[i]) == LOW) << i;
  }
  Serial.printf("Nilai DIP Switch: %d\n", value);
  delay(300);
}
```

**Kode Program (Matrix Keypad 4x4 — Scanning):**
```cpp
#define ROWS 4
#define COLS 4

byte rowPins[ROWS] = {13, 12, 14, 27};
byte colPins[COLS] = {26, 25, 33, 32};

char keymap[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < ROWS; i++) pinMode(rowPins[i], OUTPUT);
  for (int i = 0; i < COLS; i++) pinMode(colPins[i], INPUT_PULLUP);
}

char scanKeypad() {
  for (int r = 0; r < ROWS; r++) {
    for (int i = 0; i < ROWS; i++) digitalWrite(rowPins[i], HIGH);
    digitalWrite(rowPins[r], LOW);

    for (int c = 0; c < COLS; c++) {
      if (digitalRead(colPins[c]) == LOW) {
        delay(20); // antisipasi bouncing awal
        return keymap[r][c];
      }
    }
  }
  return '\0';
}

void loop() {
  char key = scanKeypad();
  if (key != '\0') {
    Serial.printf("Tombol ditekan: %c\n", key);
  }
}
```

---

### PERCOBAAN 4 — Debouncing pada Input Tombol GPIO (ESP32 + Framework Arduino)

**Tujuan:**
Mahasiswa mampu mengidentifikasi permasalahan bouncing pada tombol mekanik dan mengimplementasikan debouncing berbasis software pada ESP32 dengan framework Arduino.

**Langkah Kerja:**
1. Gunakan rangkaian pushbutton NO dari Percobaan 3
2. Implementasikan program dengan **dua variabel counter sekaligus** — satu tanpa debouncing (`counterNoDebounce`) dan satu dengan debouncing (`counterWithDebounce`) — sesuai kode di bawah
3. Tekan tombol satu kali secara normal, amati dan bandingkan kedua nilai counter yang tampil bersamaan pada Serial Monitor
4. Ulangi penekanan beberapa kali — amati bahwa `counterNoDebounce` sering bertambah lebih dari 1 untuk satu kali tekan (indikasi bouncing), sedangkan `counterWithDebounce` konsisten bertambah tepat 1
5. Terapkan konsep debouncing yang sama (bagian "Dengan Debouncing" pada kode) pada fungsi `scanKeypad()` dari Percobaan 3

**Kode Program (Perbandingan Counter Tanpa vs Dengan Debouncing):**
```cpp
#define BUTTON_PIN 25
#define DEBOUNCE_DELAY 50 // ms

// Tanpa debouncing
int rawLastState = HIGH;
unsigned long counterNoDebounce = 0;

// Dengan debouncing
int lastFlickerableState = HIGH;
int stableState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long counterWithDebounce = 0;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  int reading = digitalRead(BUTTON_PIN);

  // ===== Tanpa Debouncing =====
  // Langsung dihitung setiap transisi HIGH->LOW, tanpa validasi waktu
  if (reading == LOW && rawLastState == HIGH) {
    counterNoDebounce++;
  }
  rawLastState = reading;

  // ===== Dengan Debouncing =====
  if (reading != lastFlickerableState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    if (reading != stableState) {
      stableState = reading;
      if (stableState == LOW) {
        counterWithDebounce++;
      }
    }
  }
  lastFlickerableState = reading;

  // ===== Tampilkan kedua nilai secara bersamaan =====
  static unsigned long lastPrintedNoDebounce = 0, lastPrintedWithDebounce = 0;
  if (counterNoDebounce != lastPrintedNoDebounce || counterWithDebounce != lastPrintedWithDebounce) {
    Serial.printf("Tanpa Debounce: %lu\t| Dengan Debounce: %lu\n", counterNoDebounce, counterWithDebounce);
    lastPrintedNoDebounce = counterNoDebounce;
    lastPrintedWithDebounce = counterWithDebounce;
  }
}
```

**Penjelasan Kode:**
| Bagian | Penjelasan |
|---|---|
| `counterNoDebounce` | Bertambah langsung setiap kali terdeteksi transisi HIGH→LOW tanpa validasi waktu — merepresentasikan efek bouncing apa adanya |
| `lastDebounceTime = millis()` | Menyimpan waktu (ms) saat terjadi perubahan sinyal (`lastFlickerableState`), sebagai acuan awal periode debounce |
| `(millis() - lastDebounceTime) > DEBOUNCE_DELAY` | Memastikan sinyal sudah stabil minimal 50ms sebelum `stableState` diperbarui |
| `counterWithDebounce` | Hanya bertambah saat `stableState` benar-benar berubah menjadi LOW setelah melewati periode debounce |
| Blok "Tampilkan kedua nilai" | Mencetak kedua counter ke Serial Monitor hanya saat salah satu nilainya berubah, agar keduanya mudah dibandingkan secara langsung |

**Tugas Akhir Modul 1:**
Implementasikan sistem penghitung akses sederhana menggunakan matrix keypad 4x4 dengan debouncing yang telah diterapkan pada fungsi `scanKeypad()` — program harus mampu mendeteksi dan menghitung jumlah tombol valid yang ditekan secara akurat (tanpa duplikasi akibat bouncing), ditampilkan melalui Serial Monitor.

---

## F. Format Laporan Praktikum

1. **Cover** — judul modul, nama, NIM, kelas
2. **Tujuan Praktikum**
3. **Dasar Teori Singkat** (parafrase, bukan salinan modul)
4. **Alat dan Bahan**
5. **Langkah Kerja & Skema Rangkaian** (sertakan foto rangkaian nyata dan tangkapan layar `platformio.ini` untuk tiap percobaan)
6. **Kode Program** (lengkap, dengan komentar)
7. **Hasil dan Pembahasan** (jawaban pertanyaan analisis tiap percobaan + hasil tugas akhir modul)
8. **Kesimpulan**
9. **Lampiran** (foto/video demo, dokumentasi tambahan)

---

## G. Rubrik Penilaian

| Aspek | Bobot | Kriteria |
|---|---|---|
| Instalasi PlatformIO & flashing STM32 (Blink) | 15% | Toolchain terinstal benar, program Blink berhasil di-flash via ST-Link |
| Pemahaman & implementasi ESP-IDF (Blink) | 20% | Mampu menjelaskan struktur ESP-IDF dan berhasil menjalankan Blink |
| Implementasi pembacaan berbagai jenis tombol | 25% | NO, NC, toggle, DIP switch, dan matrix keypad terbaca dengan benar |
| Implementasi debouncing | 25% | Debouncing berfungsi, tidak ada duplikasi pembacaan akibat bouncing |
| Laporan & analisis | 15% | Kelengkapan, kedalaman jawaban analisis, kerapian dokumentasi |

---

## H. Referensi
1. STMicroelectronics, *STM32F4 Reference Manual (RM0383/RM0368)*
2. STMicroelectronics, *STM32F4xx HAL Driver User Manual*
3. STMicroelectronics, *UM1724 — ST-Link/V2 User Manual*
4. Espressif Systems, *ESP-IDF Programming Guide*, https://docs.espressif.com/
5. PlatformIO Documentation, https://docs.platformio.org/
6. Arduino Official Documentation, https://docs.arduino.cc/

---

## I. Kumpulan Pertanyaan

### Pertanyaan Pra-Praktikum
1. Apa perbedaan mendasar antara framework Arduino dan framework HAL/ESP-IDF dalam pemrograman mikrokontroler?
2. Mengapa STM32 Blackpill memerlukan programmer eksternal (ST-Link), sedangkan ESP32 dapat langsung diprogram melalui kabel USB?
3. Sebutkan satu contoh aplikasi nyata yang cocok menggunakan saklar Normally Close (NC), dan jelaskan alasannya.

### Pertanyaan/Analisis Percobaan 1 — STM32 Blackpill (Blink)
1. Apa yang terjadi jika `__HAL_RCC_GPIOC_CLK_ENABLE()` dihapus dari program? Jelaskan berdasarkan hasil pengamatan.
2. Bandingkan langkah upload program pada STM32 (via ST-Link) dengan proses upload yang biasa dilakukan pada board berbasis USB langsung — sebutkan minimal 2 perbedaan.
3. Jelaskan fungsi tiap baris pada `platformio.ini` project Anda.

### Pertanyaan/Analisis Percobaan 2 — ESP32 ESP-IDF (Blink)
1. Jelaskan perbedaan antara `delay()` pada Arduino dan `vTaskDelay()` pada ESP-IDF, khususnya terkait FreeRTOS.
2. Mengapa `app_main()` pada ESP-IDF disebut berjalan "sebagai task", sedangkan `loop()` pada Arduino tidak secara eksplisit demikian?
3. Bandingkan waktu/kompleksitas proses build antara project ESP-IDF dan project HAL (Percobaan 1) berdasarkan pengamatan Anda.

### Pertanyaan/Analisis Percobaan 3 — Macam-Macam Tombol
1. Jelaskan mengapa logika pembacaan pushbutton NO dan limit switch NC berkebalikan meskipun keduanya menggunakan `INPUT_PULLUP`
2. Berapa jumlah pin GPIO yang dibutuhkan untuk membaca 16 tombol pada keypad 4x4 menggunakan teknik scanning, dibandingkan jika setiap tombol disambungkan langsung ke satu GPIO tersendiri?
3. Pada aplikasi apa DIP switch lebih sesuai digunakan dibandingkan pushbutton biasa?

### Pertanyaan/Analisis Percobaan 4 — Debouncing
1. Berdasarkan pengamatan, berapa selisih rata-rata antara `counterNoDebounce` dan `counterWithDebounce` untuk satu kali penekanan tombol?
2. Jelaskan mengapa `counterWithDebounce` hanya bertambah setelah `stableState` melewati periode `DEBOUNCE_DELAY`, sedangkan `counterNoDebounce` tidak melalui validasi tersebut
3. Jelaskan secara konsep bagaimana debouncing dapat dilakukan secara hardware (mis. menggunakan kapasitor/RC filter)
