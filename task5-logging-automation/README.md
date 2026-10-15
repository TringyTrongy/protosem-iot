# Task 5 — Firebase Logging, Automation & Data Export

- Firmware: `../task4-firebase-dashboard/firmware/bulb_v2_firebase/` — `pushLog()` adds a timestamped row to `/logs` every 30 s; `syncControl()` runs auto mode (bulb on below `threshold − 100`, off above `threshold + 100`).
- Dashboard v2: `dashboard/public/v2.html` — live values, online indicator, history chart (Chart.js), recent-entries table and CSV download. To deploy, copy `v2.html` into the Task 4 Hosting folder (`../task4-firebase-dashboard/dashboard/public/`) and run `firebase deploy` there, so `index.html` stays live alongside it. Live at https://bulb-v2.web.app/v2.html once deployed.

Page: https://tringytrongy.github.io/protosem-iot/task-5.html
