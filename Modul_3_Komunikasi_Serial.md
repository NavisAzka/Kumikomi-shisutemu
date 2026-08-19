# MODUL 3
# KOMUNIKASI SERIAL & INTERFACING MODUL (UART/I2C/SPI/DMA)

**Mata Kuliah:** Praktikum Mikrokontroler & Embedded System
**Alokasi Waktu:** 5 x Percobaan (@ 100–150 menit)
**Platform:** ESP32 (Framework Arduino)
**IDE:** VSCode + PlatformIO

> **Catatan:** Modul ini melanjutkan penggunaan ESP32 dengan framework Arduino seperti pada Modul 1–2. Langkah instalasi VSCode, PlatformIO, dan driver USB-to-Serial tidak diulang di sini — lihat **[setup_vscode_platformio.md](setup_vscode_platformio.md)**. Percobaan 1 (UART), bagian board-to-board pada Percobaan 2 (I2C), dan Percobaan 3 (SPI) membutuhkan **dua board ESP32** yang saling berkomunikasi.

---

## Daftar Isi
- [A. Capaian Pembelajaran](#a-capaian-pembelajaran)
- [B. Alat dan Bahan](#b-alat-dan-bahan)
- [C. Dasar Teori](#c-dasar-teori)
  - [C.1 UART (Universal Asynchronous Receiver-Transmitter)](#c1-uart-universal-asynchronous-receiver-transmitter)
  - [C.2 I2C (Inter-Integrated Circuit)](#c2-i2c-inter-integrated-circuit)
  - [C.3 SPI (Serial Peripheral Interface)](#c3-spi-serial-peripheral-interface)
  - [C.4 DMA (Direct Memory Access)](#c4-dma-direct-memory-access)
- [D. Persiapan Sebelum Praktikum](#d-persiapan-sebelum-praktikum)
- [E. Kegiatan Praktikum](#e-kegiatan-praktikum)
  - [PERCOBAAN 1 — Komunikasi UART Antar ESP32](#percobaan-1--komunikasi-uart-antar-esp32)
  - [PERCOBAAN 2 — Komunikasi I2C: Master-Slave Antar ESP32 & Interfacing OLED](#percobaan-2--komunikasi-i2c-master-slave-antar-esp32--interfacing-oled)
  - [PERCOBAAN 3 — Komunikasi SPI Antar ESP32 (Master-Slave)](#percobaan-3--komunikasi-spi-antar-esp32-master-slave)
  - [PERCOBAAN 4 — Interfacing SPI: IMU MPU6500 (Pembacaan Register SPI Manual)](#percobaan-4--interfacing-spi-imu-mpu6500-pembacaan-register-spi-manual)
  - [PERCOBAAN 5 — DMA: Pembacaan IMU MPU6500 via SPI dengan DMA](#percobaan-5--dma-pembacaan-imu-mpu6500-via-spi-dengan-dma)
- [F. Tugas Pasca Praktikum (Simulasi Wokwi)](#f-tugas-pasca-praktikum-simulasi-wokwi)
- [G. Referensi](#g-referensi)

---

## A. Capaian Pembelajaran

Setelah menyelesaikan Modul 3, praktikan mampu:
1. Menjelaskan prinsip kerja protokol komunikasi UART, I2C, dan SPI
2. Mengimplementasikan komunikasi UART antar dua board ESP32
3. Mengimplementasikan komunikasi I2C antar dua board ESP32 dengan skema master-slave, serta interfacing lebih dari satu modul I2C sekaligus pada satu bus (OLED display + IMU MPU6050)
4. Mengimplementasikan komunikasi SPI antar dua board ESP32 dengan skema master-slave
5. Mengimplementasikan interfacing modul eksternal melalui SPI (IMU MPU6500)
6. Menjelaskan konsep DMA dan mengimplementasikan pembacaan data IMU melalui SPI dengan DMA diaktifkan, serta membandingkannya dengan metode pembacaan blocking biasa

---

## B. Alat dan Bahan

| No | Nama Komponen | Spesifikasi | Jumlah |
|---|---|---|---|
| 1 | Board ESP32 DevKit | ESP32 DevKit v1 | 2 |
| 2 | Kabel USB Micro/USB-C | untuk tiap ESP32 ke PC | 2 |
| 3 | Breadboard | 830 titik | 1 |
| 4 | Kabel jumper male-male / male-female | — | secukupnya |
| 5 | Modul OLED display | SSD1306, 128x64, I2C | 1 |
| 6 | Modul IMU MPU6050 | breakout dengan interface I2C | 1 |
| 7 | Modul IMU MPU6500 | breakout dengan interface SPI | 1 |
| 8 | Resistor pull-up | 4.7kΩ (opsional, jika bus I2C tidak stabil) | 2 |
| 9 | Laptop/PC | VSCode + PlatformIO terinstal | 1 |

---

## C. Dasar Teori

### C.1 UART (Universal Asynchronous Receiver-Transmitter)
UART adalah protokol komunikasi serial **asinkron** — tidak menggunakan sinyal clock bersama, sehingga kedua perangkat harus disepakati terlebih dahulu **baud rate**-nya (kecepatan transmisi, mis. 115200 bps) agar dapat saling memahami data. Komunikasi UART menggunakan dua jalur: **TX** (transmit) dan **RX** (receive), dengan aturan pin TX satu perangkat disambungkan ke pin RX perangkat lainnya (silang). ESP32 memiliki beberapa UART hardware; selain UART0 (digunakan untuk Serial Monitor/upload program), tersedia UART1 dan UART2 yang dapat dikonfigurasi bebas pada pin GPIO yang diinginkan menggunakan `HardwareSerial`.

![Gambar 1: Diagram wiring UART silang antar dua board (TX papan A ke RX papan B, RX papan A ke TX papan B, GND bersama)](img/wiring_uart_silang.png)

### C.2 I2C (Inter-Integrated Circuit)
I2C adalah protokol komunikasi serial **sinkron** yang hanya membutuhkan dua jalur: **SDA** (Serial Data) dan **SCL** (Serial Clock), memungkinkan banyak perangkat terhubung pada bus yang sama. Setiap perangkat (**slave**) memiliki alamat unik (7-bit), sedangkan satu perangkat bertindak sebagai **master** yang mengatur clock dan menginisiasi komunikasi. Pada ESP32, pin default I2C adalah **SDA = GPIO 21** dan **SCL = GPIO 22**, diakses melalui library `Wire`.

Karena setiap perangkat I2C dibedakan melalui **alamat**, bukan jalur fisik terpisah seperti SPI, **lebih dari satu perangkat dapat berbagi SDA/SCL yang sama** selama alamatnya berbeda. Dua contoh perangkat I2C yang digunakan pada modul ini:
- **OLED display (SSD1306):** alamat tetap `0x3C`, dikendalikan melalui perintah-perintah yang telah diabstraksi oleh library (mis. Adafruit SSD1306), sehingga praktikan tidak perlu menulis manual setiap byte perintah ke controller display
- **IMU MPU6050:** alamat default `0x68` (dapat berubah menjadi `0x69` tergantung kondisi pin `AD0`), diakses melalui pembacaan/penulisan register secara langsung (mis. `PWR_MGMT_1` untuk membangunkan sensor, `ACCEL_XOUT_H` untuk data akselerometer) — mirip prinsipnya dengan MPU6500 pada Percobaan 4, hanya berbeda protokol fisik (I2C, bukan SPI)

![Gambar 2: Diagram bus I2C dengan satu master dan beberapa slave (OLED 0x3C, MPU6050 0x68) berbagi jalur SDA/SCL yang sama](img/diagram_bus_i2c.png)

### C.3 SPI (Serial Peripheral Interface)
SPI adalah protokol komunikasi serial sinkron **full-duplex** (dapat mengirim dan menerima data secara bersamaan), menggunakan empat jalur: **MOSI** (Master Out Slave In), **MISO** (Master In Slave Out), **SCK** (Serial Clock), dan **CS/SS** (Chip Select). Berbeda dengan I2C yang menggunakan pengalamatan, SPI memilih perangkat tujuan melalui jalur CS terpisah untuk masing-masing slave — sehingga umumnya lebih cepat namun membutuhkan lebih banyak jalur pin dibanding I2C. Banyak sensor presisi tinggi seperti IMU (Inertial Measurement Unit) MPU6500 menyediakan antarmuka SPI, diakses melalui pembacaan/penulisan **register** — setiap register memiliki alamat 8-bit, dengan bit paling signifikan (MSB) menandai operasi baca (`1`) atau tulis (`0`).

Berbeda dengan I2C yang mendukung mode master maupun slave secara native melalui library `Wire`, library `SPI` bawaan Arduino-ESP32 **hanya mendukung mode master**. Untuk menjadikan ESP32 sebagai **SPI slave** (mis. saat dua board ESP32 berkomunikasi langsung via SPI), diperlukan driver `spi_slave` dari ESP-IDF (`driver/spi_slave.h`) yang tetap dapat dipanggil langsung dari sketch Arduino, karena Arduino-ESP32 core dibangun di atas ESP-IDF.

![Gambar 3: Diagram wiring SPI master-slave (MOSI-MOSI, MISO-MISO, SCK-SCK, CS-CS terhubung langsung tanpa disilang)](img/wiring_spi_master_slave.png)

### C.4 DMA (Direct Memory Access)
DMA adalah mekanisme perangkat keras yang memungkinkan transfer data antara peripheral dan memori **tanpa melibatkan CPU secara langsung** pada setiap byte data. Tanpa DMA, pembacaan/pengiriman data mengharuskan CPU secara aktif menangani transfer tiap byte (*blocking*), yang menghabiskan waktu eksekusi CPU. Pada ESP32, DMA untuk SPI diaktifkan langsung saat inisialisasi bus SPI (parameter *DMA channel* pada `spi_bus_initialize()`), sehingga transfer data berukuran besar — misalnya membaca beberapa register sekaligus pada IMU dalam satu transaksi — dapat dilakukan hardware secara mandiri, dan CPU hanya perlu menunggu transaksi selesai alih-alih menangani tiap byte secara manual.

![Gambar 4: Diagram blok perbandingan alur transfer data blocking (CPU menangani tiap byte) vs DMA (CPU hanya memicu lalu menunggu, hardware DMA menangani transfer)](img/diagram_blocking_vs_dma.png)

---

## D. Persiapan Sebelum Praktikum

1. Pastikan PlatformIO sudah terinstal (lihat **[setup_vscode_platformio.md](setup_vscode_platformio.md)**)
2. Siapkan **dua board ESP32** — satu akan berperan sebagai pengirim/master, satu lagi sebagai penerima/slave, pada Percobaan 1, bagian board-to-board Percobaan 2, dan Percobaan 3
3. Library `Wire` (I2C) dan `SPI` sudah termasuk dalam Arduino-ESP32 core, tidak perlu instalasi tambahan
4. Untuk interfacing OLED pada Percobaan 2, tambahkan library **Adafruit SSD1306** dan **Adafruit GFX** melalui PlatformIO Library Manager atau pada `platformio.ini`:
   ```ini
   lib_deps =
       adafruit/Adafruit SSD1306@^2.5.9
       adafruit/Adafruit GFX Library@^1.11.9
   ```
5. Interfacing MPU6500 pada Percobaan 4 dan 5 dilakukan dengan pembacaan register SPI secara manual (tanpa library eksternal), agar praktikan memahami langsung protokol SPI yang mendasarinya
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
8. Karena Percobaan 1, bagian board-to-board Percobaan 2, dan Percobaan 3 membutuhkan dua sketch program berbeda (pengirim & penerima, atau master & slave) yang berjalan di board terpisah, buat **dua project PlatformIO terpisah**, atau gunakan dua folder `src` berbeda yang di-build bergantian ke masing-masing board

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

**`platformio.ini`** (sama untuk project Transmitter maupun Receiver):
```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
```

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
Mahasiswa mampu mengimplementasikan komunikasi I2C antara dua board ESP32 dengan skema master-slave, serta interfacing lebih dari satu modul I2C nyata sekaligus pada satu bus (OLED display dan IMU MPU6050).

#### Bagian A — Master-Slave Antar ESP32

**Skema Rangkaian:**

| Board | Pin | Terhubung ke |
|---|---|---|
| ESP32 Master — SDA | GPIO 21 | SDA (GPIO 21) pada ESP32 Slave |
| ESP32 Master — SCL | GPIO 22 | SCL (GPIO 22) pada ESP32 Slave |
| ESP32 Master — GND | GND | GND pada ESP32 Slave |

> Jika komunikasi tidak stabil (data tidak terbaca/terputus-putus), tambahkan resistor pull-up 4.7kΩ dari SDA dan SCL masing-masing ke 3.3V.

**`platformio.ini`** (sama untuk project Master maupun Slave):
```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
```

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

#### Bagian B — Interfacing Multi-Device I2C: OLED + MPU6050

**Skema Rangkaian:**

| Komponen | Pin ESP32 | Keterangan |
|---|---|---|
| OLED SSD1306 — SDA | GPIO 21 | Bus I2C default — **dibagi bersama** dengan MPU6050 |
| OLED SSD1306 — SCL | GPIO 22 | Bus I2C default — **dibagi bersama** dengan MPU6050 |
| OLED SSD1306 — VCC/GND | 3.3V, GND | Periksa datasheet modul (umumnya toleran 3.3–5V) |
| MPU6050 — SDA | GPIO 21 | Bus I2C default — pin **sama persis** dengan OLED |
| MPU6050 — SCL | GPIO 22 | Bus I2C default — pin **sama persis** dengan OLED |
| MPU6050 — VCC/GND | 3.3V, GND | Periksa datasheet modul |

> Kedua perangkat disambungkan ke **pin SDA/SCL yang sama** — ini adalah inti dari percobaan ini: membuktikan bahwa I2C dapat melayani banyak perangkat pada satu bus fisik, selama alamatnya berbeda (OLED = `0x3C`, MPU6050 = `0x68`).

**`platformio.ini`:**
```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps =
    adafruit/Adafruit SH110X@^2.1.11
    adafruit/Adafruit GFX Library@^1.11.9
```
> MPU6050 diakses melalui pembacaan/penulisan register `Wire` secara manual, sehingga tidak memerlukan library tambahan.

**Langkah Kerja:**
1. Rangkai OLED dan MPU6050 pada bus I2C yang sama sesuai skema (SDA dan SCL kedua modul terhubung ke pin GPIO yang sama)
2. Jalankan I2C scanner sederhana (`Wire.beginTransmission(addr)` untuk tiap alamat 1–127, cek `Wire.endTransmission() == 0`) untuk memverifikasi **kedua alamat** (`0x3C` dan `0x68`) terdeteksi pada bus yang sama
3. Inisialisasi kedua perangkat dalam satu program: OLED via library Adafruit SSD1306, MPU6050 via pembacaan/penulisan register manual
4. Bangunkan MPU6050 dari sleep mode (tulis `0x00` ke register `PWR_MGMT_1`, alamat `0x6B`), lalu baca data akselerometer secara berkala (register `ACCEL_XOUT_H`, alamat `0x3B`)
5. Tampilkan hasil pembacaan akselerometer pada layar OLED (bukan hanya Serial Monitor) — buktikan kedua perangkat dapat diakses bergantian pada bus yang sama tanpa saling mengganggu

**Kode Program (OLED + MPU6050 pada Satu Bus I2C):**
```cpp
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ==================== OLED ====================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ==================== MPU6050 ====================
#define MPU6050_ADDR 0x68
#define PWR_MGMT_1   0x6B
#define ACCEL_XOUT_H 0x3B

void writeRegister(uint8_t reg, uint8_t value)
{
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

int16_t read16(uint8_t reg)
{
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false); // repeated start, bus tetap dikuasai

    Wire.requestFrom(MPU6050_ADDR, 2);

    uint8_t high = Wire.read();
    uint8_t low  = Wire.read();

    return (high << 8) | low;
}

void setup()
{
    Serial.begin(115200);
    Wire.begin(21, 22); // SDA, SCL - satu bus untuk OLED dan MPU6050

    // Inisialisasi OLED (alamat 0x3C)
    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR))
    {
        Serial.println("Inisialisasi OLED gagal!");
        while (true) {}
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    // Inisialisasi MPU6050 (alamat 0x68) - bangunkan dari sleep mode
    writeRegister(PWR_MGMT_1, 0x00);
    delay(100);
}

void loop()
{
    int16_t ax = read16(ACCEL_XOUT_H);
    int16_t ay = read16(ACCEL_XOUT_H + 2);
    int16_t az = read16(ACCEL_XOUT_H + 4);

    // Sensitivitas default +-2g -> 16384 LSB/g
    float accX = ax / 16384.0;
    float accY = ay / 16384.0;
    float accZ = az / 16384.0;

    Serial.printf("Accel X:%.2f Y:%.2f Z:%.2f\n", accX, accY, accZ);

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("MPU6050 + OLED (I2C)");
    display.setCursor(0, 20);
    display.printf("X: %.2f g", accX);
    display.setCursor(0, 32);
    display.printf("Y: %.2f g", accY);
    display.setCursor(0, 44);
    display.printf("Z: %.2f g", accZ);
    display.display();

    delay(200);
}
```

**Penjelasan Kode:**
| Bagian | Penjelasan |
|---|---|
| `Wire.begin(21, 22)` | Diinisialisasi **satu kali saja** di awal, digunakan bersama oleh OLED maupun MPU6050 — bukti bahwa satu bus I2C dapat melayani banyak perangkat |
| `display.begin(..., OLED_ADDR)` vs `writeRegister/read16(..., MPU6050_ADDR)` | Kedua perangkat diakses melalui fungsi `Wire` yang sama, namun dengan **alamat berbeda** (`0x3C` vs `0x68`) — inilah mekanisme yang memungkinkan keduanya berbagi SDA/SCL yang sama tanpa bentrok |
| `Wire.endTransmission(false)` pada `read16()` | Mengirim **repeated start** alih-alih melepas bus sepenuhnya, agar transaksi baca register MPU6050 tidak diselingi perangkat lain di tengah proses |
| `writeRegister(PWR_MGMT_1, 0x00)` | MPU6050 default dalam kondisi sleep saat pertama dinyalakan; register `PWR_MGMT_1` perlu ditulis `0x00` agar sensor aktif mengukur |
| Urutan `loop()`: baca MPU6050 → tulis ke OLED | Menunjukkan kedua perangkat diakses **bergantian** pada bus fisik yang sama dalam satu siklus program, tanpa memerlukan bus I2C terpisah |

---

### PERCOBAAN 3 — Komunikasi SPI Antar ESP32 (Master-Slave)

**Tujuan:**
Mahasiswa mampu mengimplementasikan komunikasi SPI antara dua board ESP32 dengan skema master-slave, sebagai dasar protokol SPI sebelum mempelajari interfacing modul sensor eksternal (MPU6500) pada percobaan berikutnya.

**Skema Rangkaian:**

| Board | Pin | Terhubung ke |
|---|---|---|
| ESP32 Master — MOSI | GPIO 23 | MOSI (GPIO 23) pada ESP32 Slave |
| ESP32 Master — MISO | GPIO 19 | MISO (GPIO 19) pada ESP32 Slave |
| ESP32 Master — SCK | GPIO 18 | SCK (GPIO 18) pada ESP32 Slave |
| ESP32 Master — CS | GPIO 5 | CS (GPIO 5) pada ESP32 Slave |
| ESP32 Master — GND | GND | GND pada ESP32 Slave |

> Berbeda dengan UART (silang TX-RX), jalur SPI dihubungkan **langsung sesuai nama pin yang sama** (MOSI-MOSI, MISO-MISO, SCK-SCK, CS-CS) — pemilihan master/slave ditentukan oleh program pada masing-masing board, bukan oleh pengabelan.

**`platformio.ini`** (sama untuk project Master maupun Slave):
```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
```

**Langkah Kerja:**
1. Siapkan dua project terpisah: satu untuk **Master**, satu untuk **Slave**
2. Tulis dan upload kode Master (menggunakan library `SPI` standar) ke board pertama
3. Tulis dan upload kode Slave (menggunakan driver `spi_slave` ESP-IDF, karena library `SPI` Arduino tidak mendukung mode slave) ke board kedua
4. Sambungkan MOSI-MOSI, MISO-MISO, SCK-SCK, CS-CS, dan GND-GND kedua board sesuai skema
5. Buka Serial Monitor pada board Slave, amati data yang diterima setiap detik

**Kode Program (ESP32 SPI Master — kirim string):**
```cpp
#include <Arduino.h>
#include <SPI.h>

#define SLAVE_CS 5

void setup() {
  Serial.begin(115200);

  pinMode(SLAVE_CS, OUTPUT);
  digitalWrite(SLAVE_CS, HIGH);

  SPI.begin(); // SCK=18, MISO=19, MOSI=23 (VSPI default)

  Serial.println("ESP32 SPI Master");
}

void loop() {
  const char msg[] = "Hello ESP32";

  digitalWrite(SLAVE_CS, LOW);
  for (size_t i = 0; i < sizeof(msg); i++) {
    SPI.transfer(msg[i]);
  }
  digitalWrite(SLAVE_CS, HIGH);

  Serial.println("Data terkirim");
  delay(1000);
}
```

**Kode Program (ESP32 SPI Slave — terima string):**
```cpp
#include <Arduino.h>
#include "driver/spi_slave.h"

#define PIN_MOSI 23
#define PIN_MISO 19
#define PIN_SCK  18
#define PIN_CS   5

void setup() {
  Serial.begin(115200);

  spi_bus_config_t busConfig = {};
  busConfig.mosi_io_num = PIN_MOSI;
  busConfig.miso_io_num = PIN_MISO;
  busConfig.sclk_io_num = PIN_SCK;
  busConfig.quadwp_io_num = -1;
  busConfig.quadhd_io_num = -1;

  spi_slave_interface_config_t slaveConfig = {};
  slaveConfig.mode = 0;
  slaveConfig.spics_io_num = PIN_CS;
  slaveConfig.queue_size = 1;

  spi_slave_initialize(HSPI_HOST, &busConfig, &slaveConfig, SPI_DMA_CH_AUTO);

  Serial.println("ESP32 SPI Slave siap");
}

void loop() {
  char rxBuf[32] = {0};
  char txBuf[32] = {0}; // slave tidak mengirim balik data apa pun pada percobaan ini

  spi_slave_transaction_t trans = {};
  trans.length = sizeof(rxBuf) * 8; // panjang maksimum transaksi, dalam bit
  trans.tx_buffer = txBuf;
  trans.rx_buffer = rxBuf;

  spi_slave_transmit(HSPI_HOST, &trans, portMAX_DELAY); // menunggu hingga master memulai transaksi

  Serial.print("Data diterima: ");
  Serial.println(rxBuf);
}
```

**Penjelasan Kode:**
| Bagian | Penjelasan |
|---|---|
| `SPI.begin()` pada Master | Menginisialisasi SPI hardware ESP32 sebagai **master** pada pin default VSPI (SCK=18, MISO=19, MOSI=23); pengaturan CS dilakukan manual via `digitalWrite()` |
| `digitalWrite(SLAVE_CS, LOW/HIGH)` | Mengawali dan mengakhiri satu transaksi SPI — slave hanya memproses data yang dikirim selama CS berada pada kondisi LOW |
| `spi_slave_initialize(HSPI_HOST, ...)` | Menginisialisasi SPI hardware ESP32 sebagai **slave** menggunakan driver ESP-IDF, karena library `SPI` Arduino tidak menyediakan mode slave |
| `spi_slave_transmit(HSPI_HOST, &trans, portMAX_DELAY)` | Menunggu (blocking) hingga master menginisiasi dan menyelesaikan satu transaksi SPI, lalu mengisi `rxBuf` dengan data yang diterima |
| `trans.length` | Panjang maksimum transaksi yang disiapkan slave, dalam satuan **bit** — jumlah data yang benar-benar dikirim master bisa lebih pendek |

---

### PERCOBAAN 4 — Interfacing SPI: IMU MPU6500 (Pembacaan Register SPI Manual)

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

**`platformio.ini`:**
```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
```
> Tidak ada library tambahan — pembacaan register dilakukan manual via `SPI.transfer()`.

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

### PERCOBAAN 5 — DMA: Pembacaan IMU MPU6500 via SPI dengan DMA

**Tujuan:**
Mahasiswa mampu menjelaskan konsep DMA pada SPI dan mengimplementasikan pembacaan data IMU MPU6500 secara *burst* (satu transaksi sekaligus) menggunakan driver SPI master ESP-IDF dengan DMA diaktifkan, dibandingkan dengan pembacaan byte-per-byte (blocking) pada Percobaan 4.

> **Catatan:** Percobaan ini menggunakan driver SPI master dari ESP-IDF (`driver/spi_master.h`) yang tetap dapat dipanggil langsung dari sketch Arduino karena Arduino-ESP32 core dibangun di atas ESP-IDF. API ini (termasuk `SPI_DMA_CH_AUTO`) sudah tersedia sejak ESP-IDF 4.x yang dibawa oleh **Arduino-ESP32 core 2.0.x** (platform `espressif32` resmi tanpa versi khusus), sehingga tidak memerlukan platform komunitas tambahan seperti pada Modul 2.

**Skema Rangkaian:**
Gunakan rangkaian MPU6500 yang sama dengan **Percobaan 4** (CS=GPIO 15, MOSI=GPIO 23, MISO=GPIO 19, SCK=GPIO 18).

**`platformio.ini`:**
```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
```
> Tidak ada library tambahan — driver `spi_master` diakses langsung dari header ESP-IDF (`driver/spi_master.h`) yang sudah termasuk dalam Arduino-ESP32 core.

**Langkah Kerja:**
1. Gunakan kembali rangkaian MPU6500 dari Percobaan 4
2. Jalankan kembali kode Percobaan 4 sebagai pembanding, catat estimasi waktu pembacaan 6 byte data akselerometer secara byte-per-byte
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
| Perbandingan waktu (`micros()`) | Satu transaksi 7-byte via DMA umumnya lebih efisien dibanding 7 kali pemanggilan `SPI.transfer()` satu-per-satu seperti pada Percobaan 4, karena overhead per-panggilan fungsi pada CPU berkurang |

**Tugas Akhir Modul 3:**
Rancang sistem akuisisi data sederhana yang menggabungkan beberapa topik modul ini — misalnya: satu ESP32 membaca data akselerometer dari MPU6500 (SPI) dan menampilkannya pada OLED (I2C), lalu mengirimkan data tersebut ke ESP32 kedua melalui **UART atau I2C**, dan ESP32 kedua menampilkan data yang diterima pada Serial Monitor (format timestamp + nilai akselerometer).

---

## F. Tugas Pasca Praktikum (Simulasi Wokwi)

[Wokwi](https://wokwi.com) mendukung **simulasi multi-board dalam satu project** — dua (atau lebih) ESP32 dapat diletakkan pada satu diagram dan saling terhubung melalui pin virtual, cocok untuk mensimulasikan Percobaan board-to-board (UART, I2C, dan SPI) tanpa hardware fisik ganda. Kerjakan tugas berikut **setelah** kegiatan praktikum selesai.

> **Catatan:** Ketersediaan part **MPU6050/MPU6500** dan **OLED SSD1306** pada Wokwi dapat berubah dari waktu ke waktu — periksa panel "Parts" pada editor Wokwi sebelum memulai. Jika sensor IMU tidak tersedia, ganti dengan **potensiometer** sebagai sumber data pengganti (nilai analog yang dikirim menggantikan pembacaan akselerometer).

**Tugas 1 — UART Dua Board di Wokwi:**
1. Buat satu project Wokwi dengan **dua board ESP32**, hubungkan TX2-RX2 (silang) dan GND keduanya sesuai skema Percobaan 1
2. Implementasikan ulang kode Transmitter dan Receiver pada masing-masing board, verifikasi data diterima dengan benar melalui dua jendela Serial Monitor Wokwi (satu per board)
3. Modifikasi agar data yang dikirim berupa **pembacaan potensiometer virtual** (bukan string statis "Hello ESP32"), sehingga Receiver menampilkan nilai yang berubah-ubah sesuai posisi potensiometer

**Tugas 2 — I2C Multi-Device di Wokwi:**
Pada project Wokwi terpisah, gabungkan Percobaan 2 Bagian A (I2C master-slave dua board) dengan Bagian B (OLED pada satu bus I2C) — board master membaca potensiometer lalu mengirim nilainya ke board slave melalui I2C, dan board slave menampilkan nilai yang diterima pada OLED SSD1306.

**Pengumpulan:** Sertakan link project Wokwi (mode *share*, pastikan visibility public/unlisted) beserta laporan singkat pada berkas terpisah.

---

## G. Referensi
1. Espressif Systems, *ESP32 Technical Reference Manual — UART, I2C, SPI*
2. Espressif Systems, *ESP-IDF Programming Guide — SPI Master Driver*, https://docs.espressif.com/projects/esp-idf/
3. Arduino-ESP32 Core Documentation — Wire (I2C), SPI, https://docs.espressif.com/projects/arduino-esp32/
4. Adafruit, *SSD1306 OLED Library Documentation*, https://github.com/adafruit/Adafruit_SSD1306
5. Register Map Datasheet MPU6500 (InvenSense/TDK)
6. PlatformIO Documentation, https://docs.platformio.org/
