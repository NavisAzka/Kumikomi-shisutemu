# Tutorial Setup VSCode & PlatformIO

Panduan ini akan membantu kamu menyiapkan lingkungan pemrograman untuk praktikum Mikrokontroler & Embedded System, mulai dari nol sampai siap untuk upload program pertama ke board (ESP32 atau STM32 Blackpill).

Ikuti langkah-langkah di bawah ini secara berurutan — jangan lompat-lompat, karena beberapa langkah bergantung pada langkah sebelumnya.

---

## Daftar Isi
1. [Apa itu VSCode dan PlatformIO?](#1-apa-itu-vscode-dan-platformio)
2. [Persyaratan Sistem](#2-persyaratan-sistem)
3. [Instalasi Visual Studio Code](#3-instalasi-visual-studio-code)
4. [Instalasi Ekstensi PlatformIO](#4-instalasi-ekstensi-platformio)
5. [Mengenal PlatformIO Home](#5-mengenal-platformio-home)
6. [Membuat Project Baru](#6-membuat-project-baru)
7. [Mengenal Struktur Folder Project](#7-mengenal-struktur-folder-project)
8. [Mengenal Toolbar PlatformIO](#8-mengenal-toolbar-platformio)
9. [Instalasi Driver USB](#9-instalasi-driver-usb)
10. [Uji Coba: Upload Program Pertama](#10-uji-coba-upload-program-pertama)
11. [Troubleshooting (Masalah Umum)](#11-troubleshooting-masalah-umum)
12. [Referensi](#12-referensi)

---

## 1. Apa itu VSCode dan PlatformIO?

**Visual Studio Code (VSCode)** adalah aplikasi *text editor* (aplikasi untuk menulis kode) buatan Microsoft. VSCode sendiri sebenarnya bukan alat khusus untuk mikrokontroler — ia menjadi bisa dipakai untuk itu setelah dipasangi **ekstensi (extension)** tambahan.

**PlatformIO** adalah ekstensi VSCode yang mengubahnya menjadi lingkungan pemrograman untuk berbagai jenis mikrokontroler (ESP32, STM32, Arduino, dan ratusan board lainnya). Anggap saja PlatformIO seperti "Arduino IDE versi lebih canggih" — mendukung lebih banyak board, lebih banyak *framework* (cara pemrograman), dan lebih mudah mengelola banyak project sekaligus.

**Kenapa kita pakai PlatformIO, bukan Arduino IDE biasa?**
- Bisa dipakai untuk banyak jenis board (ESP32 maupun STM32) dalam satu aplikasi yang sama
- Struktur project lebih rapi dan mudah dikelola
- Instalasi library lebih praktis
- Banyak dipakai di dunia profesional/industri

---

## 2. Persyaratan Sistem

Sebelum mulai, pastikan laptop/PC kamu memenuhi hal berikut:
- Sistem operasi **Windows 10/11**, **macOS**, atau **Linux**
- Koneksi internet yang stabil (instalasi PlatformIO membutuhkan proses download)
- Ruang penyimpanan kosong minimal **2 GB**
- Kabel USB sesuai board yang akan dipakai (Micro-USB atau USB-C)
- Hak akses **Administrator** di laptop/PC (untuk instalasi driver)

---

## 3. Instalasi Visual Studio Code

1. Buka browser, kunjungi situs resmi VSCode:
   ```
   https://code.visualstudio.com/
   ```
2. Klik tombol **Download**. Situs akan otomatis mendeteksi sistem operasimu dan menawarkan installer yang sesuai.

   ![Gambar 1: Tampilan halaman download di code.visualstudio.com](img/download_vscode.png)

3. Jalankan file installer yang sudah terdownload (mis. `VSCodeUserSetup-x64-x.xx.x.exe` untuk Windows).
4. Ikuti proses instalasi:
   - Centang **"I accept the agreement"**, klik **Next**
   - Biarkan lokasi instalasi default, klik **Next**


   - Pada halaman **Select Additional Tasks**, disarankan mencentang:
     - ☑ *Add "Open with Code" action to Windows Explorer file context menu*
     - ☑ *Add to PATH*

5. Klik **Install**, tunggu hingga selesai, lalu klik **Finish** (biarkan opsi *Launch Visual Studio Code* tetap tercentang).
6. VSCode akan terbuka otomatis. Kamu akan melihat halaman **Welcome**.

> ✅ **Checkpoint:** Jika VSCode berhasil terbuka tanpa pesan error, instalasi tahap ini sudah berhasil. Lanjut ke langkah berikutnya.

---

## 4. Instalasi Ekstensi PlatformIO

1. Di VSCode, klik ikon **Extensions** pada sidebar kiri (ikon berbentuk kotak-kotak kecil), atau tekan `Ctrl+Shift+X`.

   ![Gambar 5: Lokasi ikon Extensions pada sidebar kiri VSCode](img/vscode_extensions_icon.png)

2. Ketik **"PlatformIO IDE"** pada kotak pencarian.
3. Pastikan kamu memilih ekstensi resmi yang dipublikasikan oleh **PlatformIO** (biasanya muncul paling atas, ada logo semut 🐜). Klik **Install**.

   ![Gambar 6: Hasil pencarian "PlatformIO IDE" di Extensions Marketplace](img/pencarian_platformio_ide.png)

4. Tunggu proses instalasi. Ini membutuhkan waktu **beberapa menit** karena PlatformIO akan mengunduh komponen tambahan (Python, PlatformIO Core, dsb.) secara otomatis di latar belakang. Pastikan koneksi internet tetap stabil selama proses ini.

5. Setelah selesai, VSCode biasanya meminta untuk **reload/restart window**. Klik tombol tersebut jika muncul.
6. Setelah VSCode terbuka kembali, kamu akan melihat ikon baru berbentuk **semut/kepala alien 🐜** pada sidebar kiri — itu adalah ikon PlatformIO.

   ![Gambar 8: Ikon PlatformIO pada sidebar kiri VSCode setelah instalasi berhasil](img/ikon_platformio_sidebar.png)

> ⚠️ **Catatan:** Jangan menutup VSCode secara paksa selama proses instalasi PlatformIO Core berlangsung (biasanya ditandai dengan progress bar di bagian bawah/kanan bawah jendela). Tunggu sampai benar-benar selesai.

---

## 5. Mengenal PlatformIO Home

1. Klik ikon PlatformIO (semut) pada sidebar kiri.
2. Klik **"Open"** atau **"PIO Home"** untuk membuka halaman utama PlatformIO.

   ![Gambar 9: Tampilan PlatformIO Home — menu New Project, Import Project, Projects, Boards, dsb.](img/platformio_home.png)

Halaman **PlatformIO Home** ini akan sering kamu gunakan sepanjang praktikum, terutama untuk membuat project baru dan membuka project yang sudah ada.

---

## 6. Membuat Project Baru

1. Pada PlatformIO Home, klik tombol **"+ New Project"**.

2. Isi form yang muncul:
   - **Name:** nama project (bebas, contoh: `percobaan-blink`)
   - **Board:** ketik nama board yang dipakai. Contoh:
     - Untuk ESP32: ketik `esp32dev` atau cari **"Espressif ESP32 Dev Module"**
     - Untuk STM32 Blackpill: cari **"Blackpill F411CE"** atau **"Blackpill F401CC"** (sesuaikan dengan chip di board kamu)
   - **Framework:** pilih sesuai kebutuhan modul praktikum (mis. **Arduino** atau **STM32Cube**)
   - **Location:** biarkan default, atau tentukan folder pilihanmu sendiri (matikan toggle "Use default location" jika ingin memilih lokasi lain)

   ![Gambar 11: Form New Project dengan contoh pengisian Name, Board, dan Framework](img/form_new_project.png)

3. Klik **Finish**. Tunggu beberapa saat — PlatformIO akan otomatis mengunduh package board/framework yang dibutuhkan jika belum tersedia di komputermu. Pastikan komputermu tersambung ke Internet.

4. Setelah selesai, VSCode akan otomatis membuka folder project yang baru dibuat.

> 💡 **Tips:** Nama project sebaiknya singkat, tanpa spasi, dan menggambarkan isi project (contoh: `modul1-blink-stm32`, `modul2-motor-dc`). Ini memudahkan kamu mencari project lama nantinya.

---

## 7. Mengenal Struktur Folder Project

Setelah project dibuat, perhatikan panel **Explorer** di sisi kiri VSCode. Kamu akan melihat struktur folder seperti ini:

```
nama-project/
├── .pio/            <- folder otomatis, berisi file hasil build (jangan diedit manual)
├── .vscode/         <- pengaturan khusus VSCode untuk project ini
├── include/         <- tempat menyimpan file header (.h) custom
├── lib/             <- tempat menyimpan library yang kamu buat sendiri
├── src/              
│   └── main.cpp     <- FILE UTAMA tempat kamu menulis kode program!
├── test/            <- tempat menulis unit test (jarang dipakai di praktikum ini)
└── platformio.ini   <- FILE KONFIGURASI project (board, framework, library, dst.)
```

![Gambar 13: Panel Explorer VSCode menampilkan struktur folder project di atas](img/struktur_folder_project.png)

Dua file yang **paling sering kamu sentuh** selama praktikum:
- **`src/main.cpp`** — tempat kamu menulis kode program (`setup()`, `loop()`, dst.)
- **`platformio.ini`** — tempat kamu mengatur board, framework, dan library tambahan yang dibutuhkan

> ⚠️ **Penting:** Setiap modul praktikum akan memberikan contoh isi `platformio.ini`. Pastikan kamu menyalinnya dengan benar ke file `platformio.ini` di project kamu sebelum mulai menulis kode.

---

## 8. Mengenal Toolbar PlatformIO

Perhatikan **status bar** di bagian **paling bawah** jendela VSCode (garis biru). Di sana terdapat beberapa ikon penting yang akan sering kamu pakai:

| Ikon | Nama | Fungsi |
|---|---|---|
| ✓ (centang) | **Build** | Mengompilasi/mengecek kode program tanpa mengunggahnya ke board. Gunakan ini dulu untuk memeriksa apakah ada error sebelum upload |
| → (panah kanan) | **Upload** | Mengompilasi sekaligus mengunggah program ke board |
| 🔌 (colokan) | **Serial Monitor** | Membuka jendela untuk melihat output `Serial.print()`/`printf()` dari board |
| 🗑 (tempat sampah) | **Clean** | Membersihkan hasil build sebelumnya (berguna jika terjadi error aneh setelah banyak perubahan) |
| 🏠 (rumah) | **PIO Home** | Kembali ke halaman PlatformIO Home |

![Gambar 14: Status bar VSCode dengan panah penunjuk ke masing-masing ikon di atas](img/status_bar_vscode.png)

> 💡 **Tips:** Kamu juga bisa menjalankan perintah-perintah di atas lewat menu **Command Palette** (`Ctrl+Shift+P`), lalu ketik "PlatformIO: Upload" atau "PlatformIO: Build".

---

## 9. Instalasi Driver USB

Sebelum board bisa terhubung dan diprogram dari laptop/PC, kamu perlu menginstal driver yang sesuai. Driver yang dibutuhkan **berbeda-beda tergantung board**:

### 9.1 Driver untuk STM32 (via ST-Link)

STM32 Blackpill tidak bisa diprogram langsung lewat kabel USB — dibutuhkan alat tambahan bernama **ST-Link**.

1. Buka situs resmi STMicroelectronics:
   ```
   https://www.st.com/en/development-tools/stsw-link009.html
   ```
2. Buat akun gratis (jika diminta), lalu unduh driver **STSW-LINK009**.
3. Jalankan installer, ikuti instruksi sampai selesai.

   ![Gambar 15: Halaman download STSW-LINK009 di situs ST](img/download_stlink_driver.png)

### 9.2 Driver untuk ESP32 (USB-to-Serial)

ESP32 memakai chip USB-to-Serial bawaan untuk komunikasi dengan PC. Ada dua jenis chip yang umum dipakai — periksa dulu chip kecil di board ESP32 kamu (biasanya tertulis di dekat konektor USB):

**Jika chip-nya CP2102 / CP2104 (Silicon Labs):**
```
https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers
```

**Jika chip-nya CH340 / CH340C / CH341 (WCH):**
```
https://www.wch-ic.com/downloads/CH341SER_ZIP.html
```

![Gambar 16: Contoh penanda chip CP2102/CH340 pada board ESP32 — biasanya berbentuk chip kecil persegi di dekat port USB](img/chip_cp2102_ch340.png)

1. Unduh driver sesuai chip yang terdeteksi.
2. Ekstrak file (jika berbentuk .zip), lalu jalankan installer di dalamnya.
3. Ikuti instruksi sampai selesai.

   ![Gambar 17: Contoh tampilan installer driver CP2102/CH340](img/installer_driver_cp2102.png)

4. **Restart laptop/PC** setelah instalasi driver selesai, agar sistem operasi mengenali driver baru dengan sempurna.

> ⚠️ **Peringatan Keamanan:** Selalu unduh driver dari situs resmi seperti di atas. Hindari mengunduh driver dari situs pihak ketiga atau torrent — berisiko mengandung malware.

---

## 10. Uji Coba: Upload Program Pertama

Setelah semua langkah di atas selesai, mari kita uji apakah semuanya sudah berjalan dengan benar.

1. Buat project baru (ikuti **Langkah 6**) dengan board ESP32 dan framework Arduino.
2. Sambungkan board ke laptop/PC menggunakan kabel USB.
3. Buka file `src/main.cpp`, hapus isinya, lalu tulis kode berikut:

   ```cpp
   #include <Arduino.h>

   void setup() {
     Serial.begin(115200);
     Serial.println("Halo, PlatformIO berhasil disetup!");
   }

   void loop() {
     Serial.println("Program berjalan...");
     delay(1000);
   }
   ```

4. Klik ikon **Upload (→)** pada status bar.

   ![Gambar 18: Proses upload berjalan di terminal bawah VSCode](img/proses_upload_terminal.png)

5. Tunggu proses **Building** dan **Uploading** selesai. Jika berhasil, akan muncul tulisan seperti `[SUCCESS]` di terminal.

   ![Gambar 19: Pesan SUCCESS setelah proses upload selesai](img/pesan_success_upload.png)

6. Klik ikon **Serial Monitor (🔌)**, pastikan baud rate di pojok kanan bawah terminal diatur ke **115200** (sesuai `Serial.begin(115200)` pada kode).
7. Kamu akan melihat tulisan "Halo, PlatformIO berhasil disetup!" muncul di layar, diikuti "Program berjalan..." setiap 1 detik.

   ![Gambar 20: Tampilan Serial Monitor menampilkan output program](img/serial_monitor_output.png)

> 🎉 **Selamat!** Jika kamu berhasil sampai ke sini, artinya VSCode dan PlatformIO sudah siap digunakan untuk seluruh modul praktikum.

---

## 11. Troubleshooting (Masalah Umum)

| Masalah | Kemungkinan Penyebab & Solusi |
|---|---|
| Board tidak muncul di daftar **Port** saat mau upload | Driver USB belum terinstal dengan benar, atau kabel USB yang dipakai hanya kabel **charging** (tidak mendukung transfer data). Coba ganti kabel USB. |
| Proses upload gagal, muncul pesan **"Failed to connect"** (ESP32) | Sebagian board ESP32 perlu ditekan tombol **BOOT/FLASH** saat proses upload dimulai. Tekan dan tahan tombol tersebut, lalu lepas begitu proses upload mulai berjalan |
| Upload STM32 gagal, ST-Link tidak terdeteksi | Periksa kembali kabel SWDIO, SWCLK, GND, dan 3.3V dari ST-Link ke board — pastikan tidak terbalik atau longgar |
| Serial Monitor menampilkan karakter aneh/tidak terbaca | Baud rate di Serial Monitor tidak sesuai dengan `Serial.begin()` pada kode. Samakan keduanya (umumnya 115200) |
| Instalasi PlatformIO Core macet lama / gagal | Periksa koneksi internet. Jika masih gagal, coba restart VSCode dan ulangi instalasi ekstensi |
| Port serial "sedang dipakai" (`Port busy`/`Access denied`) | Ada aplikasi lain yang sedang membuka port yang sama (mis. Serial Monitor/Serial Plotter yang belum ditutup). Tutup semua jendela yang membuka port tersebut, lalu coba lagi |
| Build gagal karena library tidak ditemukan | Pastikan `lib_deps` pada `platformio.ini` sudah diisi sesuai instruksi modul, lalu build ulang (PlatformIO akan otomatis mengunduh library yang dibutuhkan) |

Jika masalah masih berlanjut, catat pesan error lengkap yang muncul di terminal, lalu tanyakan ke asisten praktikum atau dosen pengampu.

---

## 12. Referensi
1. Visual Studio Code Official Documentation, https://code.visualstudio.com/docs
2. PlatformIO Documentation, https://docs.platformio.org/
3. STMicroelectronics, *STSW-LINK009 — ST-Link USB Driver*, https://www.st.com/en/development-tools/stsw-link009.html
4. Silicon Labs, *CP210x USB to UART Bridge VCP Drivers*, https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers
5. WCH, *CH340/CH341 Driver*, https://www.wch-ic.com/downloads/CH341SER_ZIP.html


