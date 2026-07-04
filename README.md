# Sistem Monitoring Suhu dan Kelembaban Ruangan Ujian

Project Akhir Mata Kuliah Sistem Mikrokontroler
Departemen Informatika – Universitas Teknologi Bandung

**Nama:** Wafiq Salma Aulia
**NIM:** 23552011427
**Kelas:** TIF K - 23B
**Dosen Pengampu:** Muchamad Rusdan, S.T., M.T.

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
- YouTube: [tempel link video YouTube Anda]

## Code
- Source code utama (Sketch.ino)
  #include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ---------- Konfigurasi Pin ----------
#define DHTPIN     15
#define DHTTYPE    DHT22
#define LED_HIJAU  2
#define LED_MERAH  4
#define BUZZER     5
#define RELAY_KIPAS 18

// ---------- Konfigurasi WiFi (Wokwi Simulated Network) ----------
const char* ssid     = "Wokwi-GUEST";
const char* password = "";

// ---------- Konfigurasi ThingSpeak ----------
const char* server        = "http://api.thingspeak.com/update";
String      apiKey        = "QSMCVSS4Q92PIZK2";

// ---------- Ambang Batas Kondisi Nyaman Ruangan Ujian ----------
const float SUHU_BATAS   = 30.0;  // derajat Celcius
const float HUMID_BATAS  = 70.0;  // persen RH

// ---------- Objek Sensor & LCD ----------
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

unsigned long lastSend = 0;
const unsigned long interval = 15000; // kirim data tiap 15 detik

void setup() {
  Serial.begin(115200);

  pinMode(LED_HIJAU, OUTPUT);
  pinMode(LED_MERAH, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(RELAY_KIPAS, OUTPUT);

  digitalWrite(LED_HIJAU, LOW);
  digitalWrite(LED_MERAH, LOW);
  digitalWrite(BUZZER, LOW);
  digitalWrite(RELAY_KIPAS, LOW);

  dht.begin();

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Monitoring Suhu");
  lcd.setCursor(0, 1);
  lcd.print("Ruangan Ujian");

  connectWiFi();
  delay(2000);
  lcd.clear();
}

void connectWiFi() {
  Serial.print("Menghubungkan ke WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Terhubung!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  float suhu     = dht.readTemperature();
  float kelembaban = dht.readHumidity();

  if (isnan(suhu) || isnan(kelembaban)) {
    Serial.println("Gagal membaca sensor DHT22!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sensor Error!");
    delay(2000);
    return;
  }

  // Tampilkan ke LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Suhu : ");
  lcd.print(suhu, 1);
  lcd.print(" C");
  lcd.setCursor(0, 1);
  lcd.print("Lembab: ");
  lcd.print(kelembaban, 1);
  lcd.print(" %");

  // Tampilkan ke Serial Monitor
  Serial.print("Suhu: ");
  Serial.print(suhu);
  Serial.print(" C | Kelembaban: ");
  Serial.print(kelembaban);
  Serial.println(" %");

  // Logika kontrol aktuator
  if (suhu > SUHU_BATAS || kelembaban > HUMID_BATAS) {
    digitalWrite(LED_MERAH, HIGH);
    digitalWrite(LED_HIJAU, LOW);
    digitalWrite(BUZZER, HIGH);
    digitalWrite(RELAY_KIPAS, HIGH);
    Serial.println("Status: TIDAK NYAMAN -> Kipas AKTIF, Alarm berbunyi");
  } else {
    digitalWrite(LED_MERAH, LOW);
    digitalWrite(LED_HIJAU, HIGH);
    digitalWrite(BUZZER, LOW);
    digitalWrite(RELAY_KIPAS, LOW);
    Serial.println("Status: NORMAL");
  }

  // Kirim data ke ThingSpeak tiap 15 detik
  if (millis() - lastSend > interval) {
    kirimDataThingSpeak(suhu, kelembaban);
    lastSend = millis();
  }

  delay(2000);
}

void kirimDataThingSpeak(float suhu, float kelembaban) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(server) + "?api_key=" + apiKey +
                 "&field1=" + String(suhu) +
                 "&field2=" + String(kelembaban);

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      Serial.print("Data terkirim ke ThingSpeak. Kode HTTP: ");
      Serial.println(httpCode);
    } else {
      Serial.print("Gagal mengirim data. Error: ");
      Serial.println(http.errorToString(httpCode));
    }
    http.end();
  } else {
    Serial.println("WiFi tidak terhubung, mencoba reconnect...");
    connectWiFi();
  }
}


- Source code (Diagram.json)
  "version": 1,
  "author": "Mahasiswa",
  "editor": "wokwi",
  "parts": [
    { "type": "wokwi-esp32-devkit-v1", "id": "esp", "top": 0, "left": 0, "attrs": {} },
    { "type": "wokwi-dht22", "id": "dht22", "top": -80, "left": 200, "attrs": {} },
    { "type": "wokwi-lcd1602", "id": "lcd1", "top": -160, "left": -260, "attrs": { "pins": "i2c" } },
    { "type": "wokwi-led", "id": "led_hijau", "top": 100, "left": 250, "attrs": { "color": "green" } },
    { "type": "wokwi-led", "id": "led_merah", "top": 130, "left": 250, "attrs": { "color": "red" } },
    { "type": "wokwi-buzzer", "id": "buzzer1", "top": 160, "left": 250, "attrs": {} },
    { "type": "wokwi-resistor", "id": "r1", "top": 100, "left": 300, "attrs": { "value": "220" } },
    { "type": "wokwi-resistor", "id": "r2", "top": 130, "left": 300, "attrs": { "value": "220" } },
    { "type": "wokwi-relay-module", "id": "relay1", "top": 200, "left": 250, "attrs": {} }
  ],
  "connections": [
    [ "esp:D15", "dht22:SDA", "black", [] ],
    [ "esp:3V3", "dht22:VCC", "red", [] ],
    [ "esp:GND.1", "dht22:GND", "black", [] ],

    [ "esp:D21", "lcd1:SDA", "green", [] ],
    [ "esp:D22", "lcd1:SCL", "yellow", [] ],
    [ "esp:3V3", "lcd1:VCC", "red", [] ],
    [ "esp:GND.1", "lcd1:GND", "black", [] ],

    [ "esp:D2", "r1:1", "green", [] ],
    [ "r1:2", "led_hijau:A", "green", [] ],
    [ "led_hijau:C", "esp:GND.2", "black", [] ],

    [ "esp:D4", "r2:1", "red", [] ],
    [ "r2:2", "led_merah:A", "red", [] ],
    [ "led_merah:C", "esp:GND.2", "black", [] ],

    [ "esp:D5", "buzzer1:1", "orange", [] ],
    [ "buzzer1:2", "esp:GND.2", "black", [] ],

    [ "esp:D18", "relay1:IN", "blue", [] ],
    [ "esp:5V", "relay1:VCC", "red", [] ],
    [ "esp:GND.2", "relay1:GND", "black", [] ]
  ],
  "dependencies": {}
}
