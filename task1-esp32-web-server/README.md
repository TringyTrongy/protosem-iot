# Task 1 — ESP32 Web Server & HTML LED Control

`esp32_webserver_led/esp32_webserver_led.ino` runs an HTTP server on the ESP32 and serves a page with ON/OFF buttons that switch an LED on GPIO2.

1. Set `WIFI_SSID` / `WIFI_PASS` (2.4 GHz network).
2. Board: ESP32 Dev Module. Upload.
3. Serial Monitor at 115200 baud shows the IP address; open it on a phone on the same Wi-Fi.

Wiring: GPIO2 → 220 Ω → LED anode; LED cathode → GND.

Page: https://tringytrongy.github.io/protosem-iot/task-1.html
