<div align="center">
  <img src="img/header.png" alt="Header Praktikum Mikrokontroler" width="100%" style="border-radius: 10px;">
  <br><br>

  <a href="https://git.io/typing-svg">
    <img src="https://readme-typing-svg.demolab.com?font=Fira+Code&weight=700&size=28&pause=1000&color=007BFF&center=true&vCenter=true&width=600&lines=Selamat+Datang+di+Praktikum;Mikrokontroler+%26+Embedded+System;ESP32%2C+Sensor%2C+Aktuator" alt="Typing SVG" />
  </a>

  <p>
    <b>Praktikum ini merupakan bagian dari mata kuliah Mikrokontroler & Embedded System yang bertujuan untuk memberikan pemahaman praktis mengenai pemrograman mikrokontroler, interfacing sensor/aktuator, komunikasi serial, real-time system, hingga dasar sistem kontrol.</b>
  </p>

  <p>
    <img src="https://img.shields.io/badge/ESP32-E7352C?style=for-the-badge&logo=espressif&logoColor=white" alt="ESP32">
    <img src="https://img.shields.io/badge/STM32-03234B?style=for-the-badge&logo=stmicroelectronics&logoColor=white" alt="STM32">
    <img src="https://img.shields.io/badge/PlatformIO-FF7F00?style=for-the-badge&logo=platformio&logoColor=white" alt="PlatformIO">
    <img src="https://img.shields.io/badge/Arduino-00979D?style=for-the-badge&logo=arduino&logoColor=white" alt="Arduino">
    <img src="https://img.shields.io/badge/FreeRTOS-2496ED?style=for-the-badge&logo=freertos&logoColor=white" alt="FreeRTOS">
  </p>
</div>

---

## 📚 Apa yang Akan Dipelajari?

Dalam modul-modul praktikum ini, kamu akan mempelajari dan mengimplementasikan beberapa konsep penting, di antaranya:

### 1. **Pengantar Mikrokontroler & Dasar Pemrograman**

- Instalasi dan konfigurasi PlatformIO di VSCode
- Pengenalan STM32 Blackpill (framework Arduino) dan ESP32 (framework ESP-IDF & Arduino)
- Pull-up dan pull-down resistor (eksternal maupun internal) pada input tombol
- Debouncing input tombol berbasis GPIO
- Level shifter sebagai konverter level tegangan logika (3.3V ↔ 5V)

### 2. **Sensor Berdasarkan Basis Pengukuran & Aktuator Motor**

- Sensor resistif (LDR), kapasitif (touch sensor TTP223), dan induktif (hall effect)
- Sensor basis lain: akustik (ultrasonik HC-SR04) dan optik (IR obstacle)
- Kontrol kecepatan motor DC menggunakan PWM
- Kontrol motor stepper dan motor servo

### 3. **Komunikasi Serial & Interfacing Modul**

- Komunikasi UART antar board ESP32
- Komunikasi I2C (master-slave antar board, serta multi-device pada satu bus: OLED + IMU)
- Komunikasi SPI antar board maupun ke modul sensor eksternal (IMU MPU6500)
- DMA untuk transfer data SPI secara *burst*

### 4. **Interrupt, Timer, Watchdog, & Multitasking**

- External interrupt untuk manual recovery dan decoding quadrature encoder
- Timer interrupt untuk tugas periodik non-blocking
- Watchdog timer untuk menjaga keandalan sistem
- Multitasking menggunakan FreeRTOS (task & queue)

### 5. **Pemrosesan Data Sensor & Dasar Sistem Kontrol (Filtering & PID)**

- Konversi pulsa encoder menjadi RPM
- Filtering sinyal: low-pass filter alpha dan Kalman filter
- Sistem kontrol closed-loop: on-off dan Proportional (P)
- Kontrol PID lengkap beserta tuning parameter

---

## 🛠 Aplikasi dan Tools yang Dibutuhkan

Apa aja sih aplikasi yang kalian butuhkan?
Nah, untuk melakukan praktikum ini, kamu perlu menginstal aplikasi berikut:

- **Visual Studio Code**
  Editor kode utama yang digunakan sepanjang praktikum.
  Link Download : https://code.visualstudio.com/
- **PlatformIO IDE** (ekstensi VSCode)
  Lingkungan pengembangan embedded untuk membuat, build, dan upload project ke STM32 maupun ESP32.
  Instalasi lengkap ada di [setup_vscode_platformio.md](setup_vscode_platformio.md)
- **Driver USB-to-Serial**
  ST-Link (STSW-LINK009) untuk STM32 Blackpill, dan CP2102/CH340 untuk board ESP32 DevKit.
- **Serial Plotter** (ekstensi VSCode oleh badlogic)
  Dibutuhkan pada Modul 5 untuk memvisualisasikan data filtering & PID secara real-time, karena VSCode tidak memiliki Serial Plotter bawaan seperti Arduino IDE.
  Repository: https://github.com/badlogic/serial-plotter

---

## Bagaimana cara membaca modul dan buat laporannya??

Setiap modul disusun sebagai file Markdown terpisah (`Modul_1` s.d. `Modul_5`), masing-masing berisi capaian pembelajaran, alat & bahan, dasar teori, langkah kerja tiap percobaan (lengkap dengan skema rangkaian, `platformio.ini`, dan kode program), hingga tugas akhir modul. Kamu bisa membuka modul yang ingin dipelajari langsung dari folder ini, atau men-download/meng-clone seluruh repository ini agar dapat membuka semua modul secara offline.

Sebelum memulai Modul 1, pastikan langkah-langkah pada [setup_vscode_platformio.md](setup_vscode_platformio.md) sudah selesai dilakukan.

Format laporan dan rubrik penilaian disesuaikan oleh dosen/asisten pengampu masing-masing kelas.
