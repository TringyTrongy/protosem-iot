// IoT Task 1 - ESP32 Web Server: switch an LED from a web page
// Board: ESP32 DevKit V1 | LED + 220 ohm resistor on GPIO2 (the on-board LED is also on GPIO2)
// Open the Serial Monitor at 115200 baud to see the IP address, then open it in a browser
// on the same Wi-Fi network.

#include <WiFi.h>
#include <WebServer.h>

const char* WIFI_SSID = "YOUR_WIFI_SSID";       // your Wi-Fi name
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";   // your Wi-Fi password

const int LED_PIN = 2;
bool ledOn = false;

WebServer server(80);   // HTTP server on port 80

// Builds the page each time it is requested, so it always shows the current LED state
String buildPage() {
  String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<title>ESP32 LED</title>";
  html += "<style>body{font-family:sans-serif;text-align:center;margin-top:40px}";
  html += "a{display:inline-block;padding:14px 28px;margin:8px;border-radius:8px;color:#fff;text-decoration:none;font-size:18px}";
  html += ".on{background:#16a34a}.off{background:#dc2626}</style></head><body>";
  html += "<h1>ESP32 LED Control</h1>";
  html += "<p>LED is <b>" + String(ledOn ? "ON" : "OFF") + "</b></p>";
  html += "<a class='on' href='/on'>Turn ON</a><a class='off' href='/off'>Turn OFF</a>";
  html += "</body></html>";
  return html;
}

void handleRoot() {
  server.send(200, "text/html", buildPage());
}

// Each button is a link. Clicking it sends an HTTP GET request (/on or /off) to the ESP32.
void handleOn() {
  ledOn = true;
  digitalWrite(LED_PIN, HIGH);
  Serial.println("GET /on  -> LED ON");
  server.sendHeader("Location", "/");   // send the browser back to the main page
  server.send(303);
}

void handleOff() {
  ledOn = false;
  digitalWrite(LED_PIN, LOW);
  Serial.println("GET /off -> LED OFF");
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.mode(WIFI_STA);                  // join an existing network as a station
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Open http://");
  Serial.println(WiFi.localIP());       // type this address into the browser

  server.on("/", handleRoot);           // map each URL path to a handler function
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.onNotFound(handleNotFound);
  server.begin();
}

void loop() {
  server.handleClient();                // check for and answer incoming browser requests
}
