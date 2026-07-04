# Sistem Monitoring Suhu dan Kelembaban Ruangan Ujian (Topik Smart Transportation & Parking - No. 49)

Project Akhir Mata Kuliah Sistem Mikrokontroler
Teknik Informatika – Universitas Teknologi Bandung

- **Nama:** Wafiq Salma Aulia
- **NIM:** 23552011427
- **Kelas:** TIF K - 23B
- **Dosen Pengampu:** Muchamad Rusdan, S.T., M.T.

## Deskripsi
Proyek ini bertujuan untuk memantau suhu dan kelembaban ruangan ujian secara
real-time menggunakan sensor DHT22 yang terhubung ke mikrokontroler ESP32.
Data dikirimkan ke platform ThingSpeak melalui protokol HTTP agar dapat
dipantau secara real-time. Sistem juga dilengkapi aktuator otomatis (LED,
buzzer, dan relay kipas) yang aktif ketika suhu atau kelembaban melewati
ambang batas nyaman.

## Komponen yang digunakan
- ESP32 DevKit V1
- Sensor DHT22
- LCD 16x2 I2C
- LED (merah & hijau)
- Buzzer aktif
- Modul Relay + Kipas DC
- Platform IoT: ThingSpeak
- Protokol: HTTP

## Tautan
- Wokwi: https://wokwi.com/projects/468433544845045761
- YouTube: https://youtu.be/h8KMHGTJVe0?si=wtGOQksuqrUaMuCW

## Code
- Source code (Sketch.ino) ada pada file yang sudah saya lampirkan, dengan nama file " monitoring_suhu_kelembaban_ruangan_ujian.ino "

- Source code (Diagram.json) ada pada file yang sudah saya lampirkan, dengan nama file " diagram.json "
