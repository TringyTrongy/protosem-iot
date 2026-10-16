# ProtoSem IoT — Viswes Vijayaragavan Sivakumar (24BCS321)

Code and portfolio pages for the five IoT assignments of ProtoSem 1.18 at Forge Innovation & Ventures, KCT Tech Park.
One ESP32 rig grows from a local web server into a logged, automated, cloud-connected light.

**Portfolio pages:** https://tringytrongy.github.io/protosem-iot/

| Task | Page | Code |
|---|---|---|
| 1. ESP32 Web Server & HTML LED Control | [task-1](https://tringytrongy.github.io/protosem-iot/task-1.html) | [`task1-esp32-web-server/`](task1-esp32-web-server) |
| 2. Adafruit IO Dashboard & MQTT | [task-2](https://tringytrongy.github.io/protosem-iot/task-2.html) | [`task2-adafruit-io-mqtt/`](task2-adafruit-io-mqtt) |
| 3. IFTTT + Adafruit IO Automation | [task-3](https://tringytrongy.github.io/protosem-iot/task-3.html) | [`task3-ifttt-automation/`](task3-ifttt-automation) |
| 4. Firebase IoT Monitoring Dashboard | [task-4](https://tringytrongy.github.io/protosem-iot/task-4.html) | [`task4-firebase-dashboard/`](task4-firebase-dashboard) |
| 5. Firebase Logging, Automation & Data Export | [task-5](https://tringytrongy.github.io/protosem-iot/task-5.html) | [`task5-logging-automation/`](task5-logging-automation) |

Live Firebase dashboard: https://bulb-v2.web.app/ (with history and CSV export: https://bulb-v2.web.app/v2.html)

## The rig

| Part | ESP32 pin |
|---|---|
| LED + 220 Ω (Task 1) | GPIO2 |
| Relay module IN — my Task 2–3 sketch | GPIO26 |
| Relay module IN — Firebase firmware, Tasks 4–5 (active HIGH on our rig) | GPIO5 |
| DHT11 data (Tasks 4–5) | GPIO4 |
| LDR divider midpoint, 3V3 → LDR → pin → 10 kΩ → GND (Tasks 4–5) | GPIO34 |

## Credentials

No passwords, Wi-Fi details or private keys are committed. Each sketch reads them from a header that is git-ignored:

- `task2-adafruit-io-mqtt/bulb_v2_adafruitio_relay/arduino_secrets.h` — copy `arduino_secrets.example.h`
- `task4-firebase-dashboard/firmware/bulb_v2_firebase/secrets.h` — copy `secrets.example.h`

The Firebase web `apiKey` in the dashboards is public by design; the database rules (`auth != null`) protect the data.

## Layout

```
task1-esp32-web-server/      ESP32 web server sketch
task2-adafruit-io-mqtt/      Adafruit IO / MQTT relay sketch (also used by Task 3)
task3-ifttt-automation/      IFTTT applet configuration
task4-firebase-dashboard/    firmware/ (ESP32 + Firebase, also does Task 5 logging and auto mode)
                             dashboard/ (Firebase Hosting project, index.html as deployed)
task5-logging-automation/    dashboard/public/v2.html (history chart, table, CSV)
docs/                        the portfolio pages (GitHub Pages)
```
