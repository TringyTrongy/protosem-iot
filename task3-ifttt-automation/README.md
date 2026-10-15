# Task 3 — IFTTT + Adafruit IO Automation

No new firmware: the Task 2 sketch (`../task2-adafruit-io-mqtt/bulb_v2_adafruitio_relay`) already accepts `ON`/`OFF` (and `1`/`0`).

| Applet | If (trigger) | Then (action) |
|---|---|---|
| Bulb On | Google Assistant → Activate scene, scene name `Bulb On` | Adafruit → Send data to Adafruit IO, feed `bulb-relay`, data `ON` |
| Bulb Off | Google Assistant → Activate scene, scene name `Bulb Off` | Adafruit → Send data to Adafruit IO, feed `bulb-relay`, data `OFF` |

Say "Okay Google, activate Bulb On" / "…Bulb Off".

Screenshots of both applets: see the page, https://tringytrongy.github.io/protosem-iot/task-3.html
