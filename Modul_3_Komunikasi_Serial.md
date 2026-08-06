# MODUL 3
# KOMUNIKASI SERIAL & INTERFACING MODUL (UART/I2C/SPI/DMA)

**Mata Kuliah:** Praktikum Mikrokontroler & Embedded System
**Alokasi Waktu:** 4 x Percobaan (@ 100–150 menit)
**Platform:** ESP32 (Framework Arduino)
**IDE:** VSCode + PlatformIO

> **Catatan:** Modul ini melanjutkan penggunaan ESP32 dengan framework Arduino seperti pada Modul 1–2. Langkah instalasi VSCode, PlatformIO, dan driver USB-to-Serial tidak diulang di sini — lihat kembali **Modul 1, Bagian D**. Percobaan 1 (UART) dan bagian board-to-board pada Percobaan 2 (I2C) membutuhkan **dua board ESP32** yang saling berkomunikasi.

---

## A. Capaian Pembelajaran

Setelah menyelesaikan Modul 3, praktikan mampu:
1. Menjelaskan prinsip kerja protokol komunikasi UART, I2C, dan SPI
2. Mengimplementasikan komunikasi UART antar dua board ESP32
3. Mengimplementasikan komunikasi I2C antar dua board ESP32 dengan skema master-slave, serta interfacing modul I2C (OLED display)
4. Mengimplementasikan interfacing modul eksternal melalui SPI (IMU MPU6500)
5. Menjelaskan konsep DMA dan mengimplementasikan pembacaan data IMU melalui SPI dengan DMA diaktifkan, serta membandingkannya dengan metode pembacaan blocking biasa

---

## B. Alat dan Bahan

| No | Nama Komponen | Spesifikasi | Jumlah |
|---|---|---|---|
| 1 | Board ESP32 DevKit | ESP32 DevKit v1 | 2 |
| 2 | Kabel USB Micro/USB-C | untuk tiap ESP32 ke PC | 2 |
| 3 | Breadboard | 830 titik | 1 |
| 4 | Kabel jumper male-male / male-female | — | secukupnya |
| 5 | Modul OLED display | SSD1306, 128x64, I2C | 1 |
| 6 | Modul IMU MPU6500 | breakout dengan interface SPI | 1 |
| 7 | Resistor pull-up | 4.7kΩ (opsional, jika bus I2C tidak stabil) | 2 |
| 8 | Laptop/PC | VSCode + PlatformIO terinstal | 1 |

---

## C. Dasar Teori

### C.1 UART (Universal Asynchronous Receiver-Transmitter)
UART adalah protokol komunikasi serial **asinkron** — tidak menggunakan sinyal clock bersama, sehingga kedua perangkat harus disepakati terlebih dahulu **baud rate**-nya (kecepatan transmisi, mis. 115200 bps) agar dapat saling memahami data. Komunikasi UART menggunakan dua jalur: **TX** (transmit) dan **RX** (receive), dengan aturan pin TX satu perangkat disambungkan ke pin RX perangkat lainnya (silang). ESP32 memiliki beberapa UART hardware; selain UART0 (digunakan untuk Serial Monitor/upload program), tersedia UART1 dan UART2 yang dapat dikonfigurasi bebas pada pin GPIO yang diinginkan menggunakan `HardwareSerial`.

### C.2 I2C (Inter-Integrated Circuit)
I2C adalah protokol komunikasi serial **sinkron** yang hanya membutuhkan dua jalur: **SDA** (Serial Data) dan **SCL** (Serial Clock), memungkinkan banyak perangkat terhubung pada bus yang sama. Setiap perangkat (**slave**) memiliki alamat unik (7-bit), sedangkan satu perangkat bertindak sebagai **master** yang mengatur clock dan menginisiasi komunikasi. Pada ESP32, pin default I2C adalah **SDA = GPIO 21** dan **SCL = GPIO 22**, diakses melalui library `Wire`. Modul display OLED (SSD1306) adalah contoh umum perangkat I2C — modul ini memiliki alamat tetap (umumnya `0x3C`) dan dikendalikan melalui perintah-perintah yang telah diabstraksi oleh library (mis. Adafruit SSD1306), sehingga praktikan tidak perlu menulis manual setiap byte perintah ke controller display.

### C.3 SPI (Serial Peripheral Interface)
SPI adalah protokol komunikasi serial sinkron **full-duplex** (dapat mengirim dan menerima data secara bersamaan), menggunakan empat jalur: **MOSI** (Master Out Slave In), **MISO** (Master In Slave Out), **SCK** (Serial Clock), dan **CS/SS** (Chip Select). Berbeda dengan I2C yang menggunakan pengalamatan, SPI memilih perangkat tujuan melalui jalur CS terpisah untuk masing-masing slave — sehingga umumnya lebih cepat namun membutuhkan lebih banyak jalur pin dibanding I2C. Banyak sensor presisi tinggi seperti IMU (Inertial Measurement Unit) MPU6500 menyediakan antarmuka SPI, diakses melalui pembacaan/penulisan **register** — setiap register memiliki alamat 8-bit, dengan bit paling signifikan (MSB) menandai operasi baca (`1`) atau tulis (`0`).

### C.4 DMA (Direct Memory Access)
DMA adalah mekanisme perangkat keras yang memungkinkan transfer data antara peripheral dan memori **tanpa melibatkan CPU secara langsung** pada setiap byte data. Tanpa DMA, pembacaan/pengiriman data mengharuskan CPU secara aktif menangani transfer tiap byte (*blocking*), yang menghabiskan waktu eksekusi CPU. Pada ESP32, DMA untuk SPI diaktifkan langsung saat inisialisasi bus SPI (parameter *DMA channel* pada `spi_bus_initialize()`), sehingga transfer data berukuran besar — misalnya membaca beberapa register sekaligus pada IMU dalam satu transaksi — dapat dilakukan hardware secara mandiri, dan CPU hanya perlu menunggu transaksi selesai alih-alih menangani tiap byte secara manual.

---

## D. Persiapan Sebelum Praktikum

1. Pastikan PlatformIO sudah terinstal (lihat **Modul 1, Bagian D.1–D.2**)
2. Siapkan **dua board ESP32** — satu akan berperan sebagai pengirim/master, satu lagi sebagai penerima/slave, pada Percobaan 1 dan bagian board-to-board Percobaan 2
3. Library `Wire` (I2C) dan `SPI` sudah termasuk dalam Arduino-ESP32 core, tidak perlu instalasi tambahan
4. Untuk interfacing OLED pada Percobaan 2, tambahkan library **Adafruit SSD1306** dan **Adafruit GFX** melalui PlatformIO Library Manager atau pada `platformio.ini`:
   ```ini
   lib_deps =
       adafruit/Adafruit SSD1306@^2.5.9
       adafruit/Adafruit GFX Library@^1.11.9
   ```
5. Interfacing MPU6500 pada Percobaan 3 dan 4 dilakukan dengan pembacaan register SPI secara manual (tanpa library eksternal), agar praktikan memahami langsung protokol SPI yang mendasarinya
6. Buat project baru untuk Modul 3:
   - Name: `modul3-komunikasi-serial`
   - Board: **"Espressif ESP32 Dev Module"**
   - Framework: **Arduino**
7. Pastikan `platformio.ini` berisi:
   ```ini
   [env:esp32dev]
   platform = espressif32
   board = esp32dev
   framework = arduino
   ```
8. Karena Percobaan 1 dan bagian board-to-board Percobaan 2 membutuhkan dua sketch program berbeda (pengirim & penerima) yang berjalan di board terpisah, buat **dua project PlatformIO terpisah**, atau gunakan dua folder `src` berbeda yang di-build bergantian ke masing-masing board

---

## E. Kegiatan Praktikum

### PERCOBAAN 1 — Komunikasi UART Antar ESP32

**Tujuan:**
Mahasiswa mampu mengimplementasikan komunikasi UART antara dua board ESP32 menggunakan UART hardware kedua (UART2).

**Skema Rangkaian:**

| Board | Pin | Terhubung ke |
|---|---|---|
| ESP32 Transmitter — TX2 | GPIO 17 | RX2 (GPIO 16) pada ESP32 Receiver |
| ESP32 Transmitter — RX2 | GPIO 16 | TX2 (GPIO 17) pada ESP32 Receiver |
| ESP32 Transmitter — GND | GND | GND pada ESP32 Receiver |

> Kedua board tetap terhubung ke PC melalui USB masing-masing (untuk power dan Serial Monitor UART0), sehingga total terdapat 3 kabel yang menghubungkan kedua board: TX↔RX (silang) dan GND bersama.

**Langkah Kerja:**
1. Siapkan dua project terpisah: satu untuk **Transmitter**, satu untuk **Receiver**
2. Tulis dan upload kode Transmitter ke board pertama
3. Tulis dan upload kode Receiver ke board kedua
4. Sambungkan TX2-RX2 (silang) dan GND kedua board sesuai skema
5. Buka Serial Monitor pada board Receiver, amati data yang diterima setiap detik

**Kode Program (ESP32 Transmitter):**
```cpp
#include <Arduino.h>

HardwareSerial UART(2);

void setup() {
    Serial.begin(115200);
    UART.begin(115200, SERIAL_8N1, 16, 17); // RX, TX

    Serial.println("ESP32 Transmitter");
}

void loop() {
    UART.println("Hello ESP32!");
    Serial.println("Data terkirim");

    delay(1000);
}
```

**Kode Program (ESP32 Receiver):**
```cpp
#include <Arduino.h>

HardwareSerial UART(2);

void setup() {
    Serial.begin(115200);
    UART.begin(115200, SERIAL_8N1, 16, 17); // RX, TX

    Serial.println("ESP32 Receiver");
}

void loop() {
    if (UART.available()) {
        String data = UART.readStringUntil('\n');

        Serial.print("Data diterima: ");
        Serial.println(data);
    }
}
```

**Penjelasan Kode:**
| Bagian | Penjelasan |
|---|---|
| `HardwareSerial UART(2)` | Membuat objek UART hardware kedua (UART2) pada ESP32, terpisah dari `Serial` (UART0) yang digunakan untuk Serial Monitor |
| `UART.begin(115200, SERIAL_8N1, 16, 17)` | Menginisialisasi UART2 dengan baud rate 115200, format data 8N1 (8 data bit, no parity, 1 stop bit), pin RX=16 dan TX=17 |
| `UART.println(...)` | Mengirim data melalui UART2 ke board lain |
| `UART.available()` | Memeriksa apakah ada data masuk yang siap dibaca pada buffer UART2 |
| `UART.readStringUntil('\n')` | Membaca data hingga menemukan karakter newline, sesuai data yang dikirim menggunakan `println()` |

---

### PERCOBAAN 2 — Komunikasi I2C: Master-Slave Antar ESP32 & Interfacing OLED

**Tujuan:**
Mahasiswa mampu mengimplementasikan komunikasi I2C antara dua board ESP32 dengan skema master-slave, serta interfacing modul I2C nyata (OLED display).

#### Bagian A — Master-Slave Antar ESP32

**Skema Rangkaian:**

| Board | Pin | Terhubung ke |
|---|---|---|
| ESP32 Master — SDA | GPIO 21 | SDA (GPIO 21) pada ESP32 Slave |
| ESP32 Master — SCL | GPIO 22 | SCL (GPIO 22) pada ESP32 Slave |
| ESP32 Master — GND | GND | GND pada ESP32 Slave |

> Jika komunikasi tidak stabil (data tidak terbaca/terputus-putus), tambahkan resistor pull-up 4.7kΩ dari SDA dan SCL masing-masing ke 3.3V.

**Langkah Kerja:**
1. Siapkan dua project terpisah: satu untuk **Master**, satu untuk **Slave**
2. Tulis dan upload kode Master ke board pertama
3. Tulis dan upload kode Slave ke board kedua
4. Sambungkan SDA-SDA, SCL-SCL, dan GND-GND kedua board sesuai skema
5. Buka Serial Monitor pada board Slave, amati data string yang diterima setiap detik

**Kode Program (ESP32 I2C Master — kirim string):**
```cpp
#include <Arduino.h>
#include <Wire.h>

#define SLAVE_ADDR 0x08

void setup() {
    Serial.begin(115200);

    Wire.begin();              // SDA=21 SCL=22

    Serial.println("ESP32 I2C Master");
}

void loop() {
    Wire.beginTransmission(SLAVE_ADDR);
    Wire.write("Hello ESP32");
    Wire.endTransmission();

    Serial.println("Data terkirim");

    delay(1000);
}
```

**Kode Program (ESP32 I2C Slave — terima string):**
```cpp
#include <Arduino.h>
#include <Wire.h>

#define SLAVE_ADDR 0x08

void receiveEvent(int bytes)
{
    Serial.print("Data diterima : ");

    while (Wire.available())
    {
        char c = Wire.read();
        Serial.print(c);
    }

    Serial.println();
}

void setup()
{
    Serial.begin(115200);

    Wire.begin((uint8_t)SLAVE_ADDR);

    Wire.onReceive(receiveEvent);

    Serial.println("ESP32 I2C Slave");
}

void loop()
{
}
```

**Penjelasan Kode:**
| Bagian | Penjelasan |
|---|---|
| `Wire.begin()` (tanpa parameter) | Menginisialisasi I2C sebagai **master** pada pin default (SDA=21, SCL=22) |
| `Wire.begin((uint8_t)SLAVE_ADDR)` | Menginisialisasi I2C sebagai **slave** dengan alamat tertentu (0x08) |
| `Wire.beginTransmission()` / `Wire.write()` / `Wire.endTransmission()` | Rangkaian fungsi pada master untuk memulai, mengisi data, dan mengirimkan transmisi ke slave dengan alamat yang dituju |
| `Wire.onReceive(receiveEvent)` | Mendaftarkan fungsi callback yang otomatis dipanggil saat slave menerima data dari master |

#### Bagian B — Interfacing OLED Display (SSD1306)

**Skema Rangkaian:**

| Komponen | Pin ESP32 | Keterangan |
|---|---|---|
| OLED SSD1306 — SDA | GPIO 21 | Bus I2C default |
| OLED SSD1306 — SCL | GPIO 22 | Bus I2C default |
| OLED SSD1306 — VCC/GND | 3.3V, GND | Periksa datasheet modul (umumnya toleran 3.3–5V) |

**Langkah Kerja:**
1. Rangkai modul OLED sesuai skema (dapat menggunakan bus I2C yang sama dengan Bagian A, namun dilakukan pada project/waktu terpisah)
2. Tulis program inisialisasi OLED dan tampilkan teks sederhana
3. Modifikasi program agar menampilkan nilai yang berubah (mis. counter) secara real-time pada layar OLED

**Kode Program (Interfacing OLED SSD1306):**
```cpp
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int counter = 0;

void setup() {
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("Inisialisasi OLED gagal!");
    while (true) {}
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  display.clearDisplay();

  display.setCursor(0, 0);
  display.println("Modul 3 - I2C OLED");

  display.setCursor(0, 20);
  display.setTextSize(2);
  display.printf("Count: %d", counter);

  display.display();

  counter++;
  delay(500);
}
```

**Penjelasan Kode:**
| Bagian | Penjelasan |
|---|---|
| `Adafruit_SSD1306 display(...)` | Membuat objek display, menghubungkannya ke bus `Wire` (I2C) default |
| `display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)` | Menginisialisasi komunikasi I2C ke modul OLED pada alamat `0x3C`, sekaligus mengaktifkan charge pump internal OLED |
| `display.setCursor()` / `display.println()` / `display.printf()` | Mengatur posisi kursor teks dan menuliskan konten ke buffer layar (belum tampil di layar fisik) |
| `display.display()` | Mengirimkan seluruh isi buffer ke layar OLED melalui I2C — baru pada titik inilah data benar-benar tampil |

---

### PERCOBAAN 3 — Interfacing SPI: IMU MPU6500 (Pembacaan Register SPI Manual)

**Tujuan:**
Mahasiswa mampu mengimplementasikan komunikasi SPI dengan membaca data akselerometer dari IMU MPU6500 melalui pembacaan register SPI secara langsung.

**Skema Rangkaian:**

| Komponen | Pin ESP32 | Keterangan |
|---|---|---|
| MPU6500 — CS | GPIO 15 | Chip Select |
| MPU6500 — MOSI | GPIO 23 | SPI default ESP32 |
| MPU6500 — MISO | GPIO 19 | SPI default ESP32 |
| MPU6500 — SCK | GPIO 18 | SPI default ESP32 |
| MPU6500 — VCC/GND | 3.3V, GND | MPU6500 umumnya hanya toleran 3.3V |

**Langkah Kerja:**
1. Rangkai modul MPU6500 sesuai skema
2. Implementasikan fungsi baca/tulis register dasar (`readRegister`/`writeRegister`) sesuai protokol SPI MPU6500 (bit MSB pada alamat register menandai operasi baca)
3. Baca register `WHO_AM_I` (alamat `0x75`) untuk memverifikasi komunikasi berhasil (nilai yang diharapkan: `0x70` untuk MPU6500)
4. Bangunkan sensor dari sleep mode melalui register `PWR_MGMT_1` (alamat `0x6B`)
5. Implementasikan pembacaan data akselerometer (register `0x3B`–`0x40`) secara berkala, konversi ke satuan g

**Kode Program (Baca Register MPU6500 via SPI):**
```cpp
#include <Arduino.h>
#include <SPI.h>

#define MPU_CS 15

void writeRegister(uint8_t reg, uint8_t value) {
  digitalWrite(MPU_CS, LOW);
  SPI.transfer(reg & 0x7F); // MSB=0 -> operasi tulis
  SPI.transfer(value);
  digitalWrite(MPU_CS, HIGH);
}

uint8_t readRegister(uint8_t reg) {
  digitalWrite(MPU_CS, LOW);
  SPI.transfer(reg | 0x80);  // MSB=1 -> operasi baca
  uint8_t value = SPI.transfer(0x00);
  digitalWrite(MPU_CS, HIGH);
  return value;
}

void readAccel(int16_t &ax, int16_t &ay, int16_t &az) {
  digitalWrite(MPU_CS, LOW);
  SPI.transfer(0x3B | 0x80); // mulai dari ACCEL_XOUT_H

  uint8_t buf[6];
  for (int i = 0; i < 6; i++) {
    buf[i] = SPI.transfer(0x00);
  }
  digitalWrite(MPU_CS, HIGH);

  ax = (buf[0] << 8) | buf[1];
  ay = (buf[2] << 8) | buf[3];
  az = (buf[4] << 8) | buf[5];
}

void setup() {
  Serial.begin(115200);

  pinMode(MPU_CS, OUTPUT);
  digitalWrite(MPU_CS, HIGH);

  SPI.begin(); // SCK=18, MISO=19, MOSI=23 (default ESP32)

  writeRegister(0x6B, 0x00); // wake up dari sleep mode

  uint8_t whoami = readRegister(0x75);
  Serial.printf("WHO_AM_I: 0x%02X (diharapkan 0x70)\n", whoami);
}

void loop() {
  int16_t ax, ay, az;
  readAccel(ax, ay, az);

  // Sensitivitas default +-2g -> 16384 LSB/g
  float axg = ax / 16384.0;
  float ayg = ay / 16384.0;
  float azg = az / 16384.0;

  Serial.printf("Accel X:%.2fg Y:%.2fg Z:%.2fg\n", axg, ayg, azg);
  delay(200);
}
```

**Penjelasan Kode:**
| Bagian | Penjelasan |
|---|---|
| `reg \| 0x80` / `reg & 0x7F` | MPU6500 menggunakan bit MSB pada byte alamat register untuk menandai operasi baca (`1`) atau tulis (`0`) |
| `SPI.transfer(value)` | Mengirim satu byte sekaligus menerima satu byte balasan (karakteristik full-duplex SPI) — dipanggil berulang untuk membaca beberapa register/byte berurutan |
| `writeRegister(0x6B, 0x00)` | MPU6500 default dalam kondisi sleep saat pertama dinyalakan; register `PWR_MGMT_1` perlu ditulis `0x00` agar sensor aktif mengukur |
| `readAccel()` | Membaca 6 byte data akselerometer (X, Y, Z masing-masing 2 byte/16-bit) dalam satu urutan transaksi CS LOW–HIGH |
| Pembacaan byte-per-byte via `SPI.transfer()` | Pendekatan ini bersifat **blocking** — setiap panggilan `SPI.transfer()` menunggu transfer 1 byte selesai sebelum lanjut ke byte berikutnya; menjadi pembanding pada Percobaan 4 |

---

### PERCOBAAN 4 — DMA: Pembacaan IMU MPU6500 via SPI dengan DMA

**Tujuan:**
Mahasiswa mampu menjelaskan konsep DMA pada SPI dan mengimplementasikan pembacaan data IMU MPU6500 secara *burst* (satu transaksi sekaligus) menggunakan driver SPI master ESP-IDF dengan DMA diaktifkan, dibandingkan dengan pembacaan byte-per-byte (blocking) pada Percobaan 3.

> **Catatan:** Percobaan ini menggunakan driver SPI master dari ESP-IDF (`driver/spi_master.h`) yang tetap dapat dipanggil langsung dari sketch Arduino karena Arduino-ESP32 core dibangun di atas ESP-IDF. Pastikan `platformio.ini` tidak mengunci platform `espressif32` ke versi lama, agar konsisten dengan API LEDC baru pada Modul 2.

**Skema Rangkaian:**
Gunakan rangkaian MPU6500 yang sama dengan **Percobaan 3** (CS=GPIO 15, MOSI=GPIO 23, MISO=GPIO 19, SCK=GPIO 18).

**Langkah Kerja:**
1. Gunakan kembali rangkaian MPU6500 dari Percobaan 3
2. Jalankan kembali kode Percobaan 3 sebagai pembanding, catat estimasi waktu pembacaan 6 byte data akselerometer secara byte-per-byte
3. Implementasikan pembacaan register akselerometer secara *burst* (satu transaksi sekaligus) menggunakan driver `spi_master` dengan DMA diaktifkan pada inisialisasi bus
4. Ukur waktu satu transaksi burst menggunakan `micros()`, bandingkan dengan hasil pengamatan pada langkah 2

**Kode Program (Pembacaan Burst MPU6500 via SPI Master + DMA):**
```cpp
#include <Arduino.h>
#include "driver/spi_master.h"

#define MPU_CS   15
#define PIN_MOSI 23
#define PIN_MISO 19
#define PIN_SCK  18

spi_device_handle_t mpuHandle;

void initSPI_DMA() {
  spi_bus_config_t busConfig = {};
  busConfig.mosi_io_num = PIN_MOSI;
  busConfig.miso_io_num = PIN_MISO;
  busConfig.sclk_io_num = PIN_SCK;
  busConfig.quadwp_io_num = -1;
  busConfig.quadhd_io_num = -1;
  busConfig.max_transfer_sz = 32;

  // DMA diaktifkan di sini melalui parameter dma_chan (SPI_DMA_CH_AUTO)
  spi_bus_initialize(HSPI_HOST, &busConfig, SPI_DMA_CH_AUTO);

  spi_device_interface_config_t devConfig = {};
  devConfig.clock_speed_hz = 1000000; // 1 MHz
  devConfig.mode = 0;
  devConfig.spics_io_num = MPU_CS;
  devConfig.queue_size = 1;

  spi_bus_add_device(HSPI_HOST, &devConfig, &mpuHandle);
}

void wakeUpMPU() {
  uint8_t tx[2] = {0x6B, 0x00}; // PWR_MGMT_1 = 0x00
  spi_transaction_t trans = {};
  trans.length = 2 * 8; // dalam bit
  trans.tx_buffer = tx;
  spi_device_transmit(mpuHandle, &trans);
}

void setup() {
  Serial.begin(115200);
  initSPI_DMA();
  wakeUpMPU();
}

void loop() {
  uint8_t txBuf[7] = {0};
  uint8_t rxBuf[7] = {0};
  txBuf[0] = 0x3B | 0x80; // baca beruntun mulai dari ACCEL_XOUT_H, 6 byte

  spi_transaction_t trans = {};
  trans.length = sizeof(txBuf) * 8; // dalam bit
  trans.tx_buffer = txBuf;
  trans.rx_buffer = rxBuf;

  unsigned long start = micros();
  spi_device_transmit(mpuHandle, &trans); // satu transaksi, ditangani DMA
  unsigned long elapsed = micros() - start;

  int16_t ax = (rxBuf[1] << 8) | rxBuf[2];
  int16_t ay = (rxBuf[3] << 8) | rxBuf[4];
  int16_t az = (rxBuf[5] << 8) | rxBuf[6];

  Serial.printf("Accel X:%d Y:%d Z:%d | Waktu transaksi: %lu us\n", ax, ay, az, elapsed);
  delay(200);
}
```

**Penjelasan Kode:**
| Bagian | Penjelasan |
|---|---|
| `spi_bus_initialize(HSPI_HOST, &busConfig, SPI_DMA_CH_AUTO)` | Menginisialisasi bus SPI dengan DMA diaktifkan — `SPI_DMA_CH_AUTO` membiarkan driver memilih channel DMA secara otomatis |
| `spi_bus_add_device()` | Mendaftarkan MPU6500 sebagai device pada bus SPI, termasuk pin CS dan kecepatan clock yang digunakan |
| `spi_device_transmit(mpuHandle, &trans)` | Melakukan **satu transaksi** SPI (kirim alamat register + terima 6 byte data sekaligus) — untuk transfer seukuran ini, driver menangani proses melalui DMA tanpa CPU perlu memanggil fungsi transfer berulang per byte |
| `trans.length` | Panjang transaksi dinyatakan dalam satuan **bit**, bukan byte |
| Perbandingan waktu (`micros()`) | Satu transaksi 7-byte via DMA umumnya lebih efisien dibanding 7 kali pemanggilan `SPI.transfer()` satu-per-satu seperti pada Percobaan 3, karena overhead per-panggilan fungsi pada CPU berkurang |

**Tugas Akhir Modul 3:**
Rancang sistem akuisisi data sederhana yang menggabungkan beberapa topik modul ini — misalnya: satu ESP32 membaca data akselerometer dari MPU6500 (SPI) dan menampilkannya pada OLED (I2C), lalu mengirimkan data tersebut ke ESP32 kedua melalui **UART atau I2C**, dan ESP32 kedua menampilkan data yang diterima pada Serial Monitor (format timestamp + nilai akselerometer).

---

## F. Format Laporan Praktikum

1. **Cover** — judul modul, nama, NIM, kelas
2. **Tujuan Praktikum**
3. **Dasar Teori Singkat** (parafrase, bukan salinan modul)
4. **Alat dan Bahan**
5. **Langkah Kerja & Skema Rangkaian** (sertakan foto rangkaian nyata, termasuk kedua board pada Percobaan 1 dan bagian board-to-board Percobaan 2)
6. **Kode Program** (lengkap, dengan komentar, untuk masing-masing board bila berpasangan)
7. **Hasil dan Pembahasan** (data pengamatan, perbandingan blocking vs DMA, jawaban pertanyaan analisis, hasil tugas akhir modul)
8. **Kesimpulan**
9. **Lampiran** (foto/video demo, dokumentasi tambahan)

---

## G. Rubrik Penilaian

| Aspek | Bobot | Kriteria |
|---|---|---|
| Implementasi UART antar ESP32 | 15% | Data berhasil dikirim dan diterima dengan benar antar dua board |
| Implementasi I2C master-slave & OLED | 20% | Komunikasi I2C antar board dan interfacing OLED berjalan benar |
| Implementasi SPI (MPU6500) | 20% | Inisialisasi dan pembacaan register/data akselerometer MPU6500 via SPI berjalan benar |
| Implementasi & analisis DMA (SPI MPU6500) | 20% | Transaksi burst via DMA berjalan, analisis perbandingan dengan metode blocking tepat |
| Tugas akhir modul (integrasi UART/I2C/SPI) | 10% | Sistem gabungan berfungsi sesuai rancangan |
| Laporan & analisis | 15% | Kelengkapan, kedalaman analisis, kerapian dokumentasi |

---

## H. Referensi
1. Espressif Systems, *ESP32 Technical Reference Manual — UART, I2C, SPI*
2. Espressif Systems, *ESP-IDF Programming Guide — SPI Master Driver*, https://docs.espressif.com/projects/esp-idf/
3. Arduino-ESP32 Core Documentation — Wire (I2C), SPI, https://docs.espressif.com/projects/arduino-esp32/
4. Adafruit, *SSD1306 OLED Library Documentation*, https://github.com/adafruit/Adafruit_SSD1306
5. Register Map Datasheet MPU6500 (InvenSense/TDK)
6. PlatformIO Documentation, https://docs.platformio.org/

---

## I. Kumpulan Pertanyaan

### Pertanyaan Pra-Praktikum
1. Jelaskan perbedaan mendasar antara komunikasi serial sinkron (I2C, SPI) dan asinkron (UART)
2. Mengapa kedua perangkat UART harus memiliki baud rate yang sama agar komunikasi berhasil?
3. Sebutkan perbedaan jumlah jalur komunikasi yang dibutuhkan antara UART, I2C, dan SPI, serta implikasinya terhadap jumlah perangkat yang dapat dihubungkan

### Pertanyaan/Analisis Percobaan 1 — UART
1. Apa yang terjadi jika pin TX dan RX pada kedua board tidak disambungkan secara silang (TX ke TX, RX ke RX)? Jelaskan berdasarkan pemahaman Anda tentang arah aliran data UART
2. Mengapa kedua board tetap memerlukan koneksi USB masing-masing ke PC, meskipun sudah saling terhubung melalui UART2?
3. Apa yang akan terjadi jika baud rate pada kode Transmitter dan Receiver dibuat berbeda?

### Pertanyaan/Analisis Percobaan 2 — I2C (Master-Slave & OLED)
1. Jelaskan mengapa I2C membutuhkan alamat (address) untuk setiap perangkat, sedangkan UART tidak
2. Jelaskan perbedaan antara memanggil `display.println()`/`display.printf()` dengan memanggil `display.display()` — mengapa keduanya diperlukan dan tidak bisa digabung menjadi satu langkah?
3. Apa fungsi resistor pull-up pada jalur SDA dan SCL, dan apa yang dapat terjadi jika resistor ini tidak terpasang pada bus I2C yang panjang/banyak perangkat?

### Pertanyaan/Analisis Percobaan 3 — SPI (MPU6500)
1. Jelaskan fungsi pin CS (Chip Select) pada komunikasi SPI, khususnya jika suatu saat lebih dari satu perangkat SPI dipasang pada bus yang sama
2. Jelaskan makna bit MSB pada byte alamat register yang dikirim ke MPU6500 (`reg | 0x80` vs `reg & 0x7F`)
3. Mengapa register `PWR_MGMT_1` pada MPU6500 perlu ditulis terlebih dahulu sebelum sensor dapat memberikan pembacaan akselerometer yang valid?

### Pertanyaan/Analisis Percobaan 4 — DMA (SPI MPU6500)
1. Berdasarkan pengamatan, bandingkan waktu yang dibutuhkan untuk membaca 6 byte data akselerometer secara byte-per-byte (Percobaan 3) dengan satu transaksi burst via DMA (Percobaan 4)
2. Jelaskan peran parameter `SPI_DMA_CH_AUTO` pada `spi_bus_initialize()` — apa yang terjadi pada transfer data jika DMA tidak diaktifkan (mis. diganti dengan nilai `SPI_DMA_DISABLED`)?
3. Mengapa pendekatan satu transaksi burst (Percobaan 4) lebih menguntungkan dibanding beberapa kali pemanggilan fungsi transfer satu-per-satu (Percobaan 3), terutama jika jumlah data yang dibaca semakin banyak (mis. seluruh data accel + gyro sekaligus)?
