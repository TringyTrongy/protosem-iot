// Copy this file to secrets.h (same folder) and fill in your own values.
// secrets.h is git-ignored, so it never gets pushed.
#pragma once

#define WIFI_SSID      "YOUR_WIFI_NAME"        // 2.4 GHz network (the ESP32 can't join 5 GHz)
#define WIFI_PASSWORD  "YOUR_WIFI_PASSWORD"

// Firebase console > Project settings > General > Web API key
#define API_KEY        "YOUR_FIREBASE_WEB_API_KEY"
// Firebase console > Realtime Database (the URL at the top of the Data tab)
#define DATABASE_URL   "https://bulb-v2-default-rtdb.asia-southeast1.firebasedatabase.app"

// The Email/Password user from Firebase console > Authentication > Users
// (the same login you use on the dashboard)
#define USER_EMAIL     "YOUR_LOGIN_EMAIL"
#define USER_PASSWORD  "YOUR_LOGIN_PASSWORD"
