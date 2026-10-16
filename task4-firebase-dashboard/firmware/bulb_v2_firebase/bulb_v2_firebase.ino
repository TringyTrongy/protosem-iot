/*
  Bulb-V2 — Firebase firmware (IoT Tasks 4 and 5)
  ESP32 DevKit V1 + DHT11 + LDR + 1-channel relay switching a bulb
  Pins match our rig: DHT11 GPIO4, LDR GPIO34, relay GPIO5.

  What it does
    - Reads the DHT11 (temperature, humidity) and the LDR every 5 s and writes them to
      /sensors/current, which the dashboard listens to.
    - Reads /bulb (status + mode) and /settings/ldrThreshold every second.
        manual mode: the relay follows /bulb/status, which the dashboard's button sets.
        auto mode:   the ESP32 decides from the LDR and writes /bulb/status itself,
                     so the dashboard always shows what the bulb is really doing.
    - Every 30 s pushes one timestamped entry under /logs (Task 5). The timestamp is the
      Firebase server time in milliseconds, which is what the dashboard's CSV export reads.

  Database layout (matches the dashboard in ../../dashboard/public/index.html)
    /sensors/current   { temperature, humidity, ldr, updatedAt }
    /bulb              { status: true|false, mode: "manual"|"auto" }
    /settings          { ldrThreshold: number }
    /logs/<pushId>     { timestamp, temperature, humidity, ldr, bulbState: "ON"|"OFF", mode }

  Libraries (Arduino Library Manager)
    - "Firebase Arduino Client Library for ESP8266 and ESP32" by Mobizt  (Firebase_ESP_Client.h)
    - "DHT sensor library" by Adafruit  (+ "Adafruit Unified Sensor")

  Before flashing: copy secrets.example.h to secrets.h and fill it in.
  secrets.h is in .gitignore so passwords never reach the repository.
*/

#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"   // prints token generation status
#include "addons/RTDBHelper.h"
#include <DHT.h>
#include "secrets.h"              // WIFI_SSID, WIFI_PASSWORD, API_KEY, DATABASE_URL, USER_EMAIL, USER_PASSWORD

// ---------- Pins ----------
#define DHT_PIN    4      // DHT11 data pin
#define DHT_TYPE   DHT11
#define LDR_PIN    34     // LDR voltage divider output (ADC1 pin, works while Wi-Fi is on)
#define RELAY_PIN  5      // relay module IN pin
#define RELAY_ACTIVE_LOW false  // this rig's relay switches ON with HIGH; set true for an active-LOW board

// ---------- Timing ----------
const unsigned long SENSOR_INTERVAL_MS  = 5000;   // how often /sensors/current is updated
const unsigned long CONTROL_INTERVAL_MS = 1000;   // how often /bulb and the threshold are read
const unsigned long LOG_INTERVAL_MS     = 30000;  // how often a row is added to /logs

// ---------- Auto mode ----------
// Wiring: 3.3V -> LDR -> GPIO34 -> 10k -> GND, so the reading goes UP when the room is brighter.
// The bulb turns ON when the reading drops below (threshold - HYSTERESIS) and OFF when it
// rises above (threshold + HYSTERESIS). The gap stops the relay chattering near the threshold.
const int HYSTERESIS = 100;

DHT dht(DHT_PIN, DHT_TYPE);

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

bool   bulbOn       = false;
String mode         = "manual";
int    ldrThreshold = 1500;

float  lastTemp = NAN;
float  lastHum  = NAN;
int    lastLdr  = 0;

unsigned long lastSensorMs  = 0;
unsigned long lastControlMs = 0;
unsigned long lastLogMs     = 0;

// ---------- Relay ----------
// Active-HIGH board (this rig): drive the pin HIGH for ON, LOW for OFF.
// Active-LOW board: drive LOW for ON, and release the pin (INPUT, high impedance) for OFF.
//   Driving a 5 V relay module's input HIGH from a 3.3 V ESP32 pin can leak enough current to
//   keep it half-on; releasing the pin avoids that (the fix from my LCU-1 firmware).
void setRelay(bool on) {
  bulbOn = on;
  if (RELAY_ACTIVE_LOW) {
    if (on) {
      pinMode(RELAY_PIN, OUTPUT);
      digitalWrite(RELAY_PIN, LOW);
    } else {
      pinMode(RELAY_PIN, INPUT);
    }
  } else {
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, on ? HIGH : LOW);
  }
  Serial.printf("Relay -> %s\n", on ? "ON" : "OFF");
}

// ---------- Wi-Fi ----------
void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nConnected, IP: ");
  Serial.println(WiFi.localIP());
}

// ---------- Firebase ----------
void connectFirebase() {
  config.api_key      = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email     = USER_EMAIL;      // same email/password user as the dashboard login
  auth.user.password  = USER_PASSWORD;
  config.token_status_callback = tokenStatusCallback;

  Firebase.reconnectNetwork(true);
  fbdo.setBSSLBufferSize(4096, 1024);
  Firebase.begin(&config, &auth);

  Serial.print("Signing in to Firebase");
  while (!Firebase.ready()) {
    delay(300);
    Serial.print(".");
  }
  Serial.println(" ready");
}

// Creates /bulb and /settings with defaults the first time, so the dashboard has something to show.
void ensureDefaults() {
  bool hasMode = Firebase.RTDB.getString(&fbdo, "/bulb/mode") && fbdo.dataType() != "null";
  if (!hasMode) {
    Firebase.RTDB.setString(&fbdo, "/bulb/mode", "manual");
    Firebase.RTDB.setBool(&fbdo, "/bulb/status", false);
  }
  bool hasThreshold = Firebase.RTDB.getInt(&fbdo, "/settings/ldrThreshold") && fbdo.dataType() != "null";
  if (!hasThreshold) {
    Firebase.RTDB.setInt(&fbdo, "/settings/ldrThreshold", ldrThreshold);
  }
}

// ---------- Sensors -> /sensors/current ----------
void readAndUploadSensors() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  lastLdr = analogRead(LDR_PIN);           // 0 (dark) .. 4095 (bright)

  if (isnan(t) || isnan(h)) {
    Serial.println("DHT11 read failed, keeping the last good values");
  } else {
    lastTemp = t;
    lastHum  = h;
  }

  FirebaseJson json;
  if (!isnan(lastTemp)) json.set("temperature", lastTemp);
  if (!isnan(lastHum))  json.set("humidity", lastHum);
  json.set("ldr", lastLdr);
  json.set("updatedAt/.sv", "timestamp");  // server time, so it is right even without NTP

  if (Firebase.RTDB.updateNode(&fbdo, "/sensors/current", &json)) {
    Serial.printf("Sensors: %.1f C, %.0f %%, LDR %d\n", lastTemp, lastHum, lastLdr);
  } else {
    Serial.printf("Sensor upload failed: %s\n", fbdo.errorReason().c_str());
  }
}

// ---------- /bulb + /settings -> relay ----------
void syncControl() {
  if (Firebase.RTDB.getInt(&fbdo, "/settings/ldrThreshold")) {
    ldrThreshold = fbdo.to<int>();
  }

  if (!Firebase.RTDB.getJSON(&fbdo, "/bulb")) {
    Serial.printf("Reading /bulb failed: %s\n", fbdo.errorReason().c_str());
    return;
  }
  FirebaseJson &bulb = fbdo.to<FirebaseJson>();
  FirebaseJsonData r;
  bool wantedOn = bulbOn;
  if (bulb.get(r, "mode") && r.success)   mode = r.to<String>();
  if (bulb.get(r, "status") && r.success) wantedOn = r.to<bool>();

  if (mode == "auto") {
    // The ESP32 decides, then tells the database so the dashboard shows the real state.
    bool next = bulbOn;
    if (lastLdr < ldrThreshold - HYSTERESIS) next = true;    // dark  -> bulb on
    if (lastLdr > ldrThreshold + HYSTERESIS) next = false;   // bright -> bulb off
    if (next != bulbOn) {
      setRelay(next);
      Firebase.RTDB.setBool(&fbdo, "/bulb/status", next);
    }
  } else {
    // Manual: follow the dashboard button.
    if (wantedOn != bulbOn) setRelay(wantedOn);
  }
}

// ---------- One row -> /logs (Task 5) ----------
void pushLog() {
  FirebaseJson row;
  row.set("timestamp/.sv", "timestamp");   // milliseconds since 1970, set by the server
  if (!isnan(lastTemp)) row.set("temperature", lastTemp);
  if (!isnan(lastHum))  row.set("humidity", lastHum);
  row.set("ldr", lastLdr);
  row.set("bulbState", bulbOn ? "ON" : "OFF");
  row.set("mode", mode);

  if (Firebase.RTDB.pushJSON(&fbdo, "/logs", &row)) {
    Serial.printf("Logged row %s\n", fbdo.pushName().c_str());
  } else {
    Serial.printf("Log push failed: %s\n", fbdo.errorReason().c_str());
  }
}

void setup() {
  Serial.begin(115200);
  setRelay(false);                 // bulb off until we know what the dashboard wants
  dht.begin();
  analogReadResolution(12);

  connectWiFi();
  connectFirebase();
  ensureDefaults();

  readAndUploadSensors();
  syncControl();
  pushLog();
  lastSensorMs = lastControlMs = lastLogMs = millis();
}

void loop() {
  if (!Firebase.ready()) return;   // token refresh or reconnect in progress

  unsigned long now = millis();

  if (now - lastSensorMs >= SENSOR_INTERVAL_MS) {
    lastSensorMs = now;
    readAndUploadSensors();
  }
  if (now - lastControlMs >= CONTROL_INTERVAL_MS) {
    lastControlMs = now;
    syncControl();
  }
  if (now - lastLogMs >= LOG_INTERVAL_MS) {
    lastLogMs = now;
    pushLog();
  }
}
