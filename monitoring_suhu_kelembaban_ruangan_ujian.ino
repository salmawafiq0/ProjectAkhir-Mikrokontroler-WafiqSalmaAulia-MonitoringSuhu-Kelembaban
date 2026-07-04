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
