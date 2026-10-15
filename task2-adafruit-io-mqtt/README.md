# Task 2 — Adafruit IO Dashboard & MQTT

`bulb_v2_adafruitio_relay/` subscribes to the Adafruit IO feed `bulb-relay` and switches a relay (GPIO26, active LOW) that powers a bulb.

- Libraries: *Adafruit IO Arduino* (installs Adafruit MQTT).
- Copy `arduino_secrets.example.h` to `arduino_secrets.h` and fill in Wi-Fi and Adafruit IO credentials.
- Adafruit IO: feed `bulb-relay`; dashboard `bulb-relay` with a Toggle block (ON / OFF).
- The same sketch is used unchanged in Task 3 (IFTTT voice control).

Wiring: GPIO26 → relay IN, VIN → VCC, GND → GND; bulb live wire through COM / NO. **Mains voltage on the relay side.**

Page: https://tringytrongy.github.io/protosem-iot/task-2.html
