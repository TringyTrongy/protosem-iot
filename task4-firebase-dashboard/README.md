# Task 4 — Firebase IoT Monitoring Dashboard

- `firmware/bulb_v2_firebase/` — ESP32 + DHT11 (GPIO4) + LDR (GPIO34) + relay (GPIO26). Signs in to Firebase with Email/Password, writes `/sensors/current`, follows `/bulb`, and (Task 5) logs to `/logs` and runs auto mode.
  - Libraries: *Firebase Arduino Client Library for ESP8266 and ESP32* (Mobizt), *DHT sensor library* (Adafruit) + *Adafruit Unified Sensor*.
  - Copy `secrets.example.h` to `secrets.h` and fill it in.
- `dashboard/` — Firebase Hosting project for `bulb-v2`. `public/index.html` is the dashboard as deployed at https://bulb-v2.web.app/.
- `dashboard/database.rules.json` — Realtime Database rules (`auth != null`).

Database layout:

```
/sensors/current   { temperature, humidity, ldr, updatedAt }
/bulb              { status: true|false, mode: "manual"|"auto" }
/settings          { ldrThreshold }
/logs/<pushId>     { timestamp, temperature, humidity, ldr, bulbState, mode }
```

Page: https://tringytrongy.github.io/protosem-iot/task-4.html
