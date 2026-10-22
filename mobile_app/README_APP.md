# ESP8266 Wireless Relay Mobile App (Android & iPhone)

This cross-platform mobile application allows you to wirelessly connect to your ESP8266 Quad Relay System from any **Android** device or **iPhone (iOS)**, switch each relay channel with 0ms perceived lag, configure daily schedules, one-shot timers, cyclic loops, and monitor power metrics.

---

## 🚀 How to Run & Install on iPhone (iOS)

### Option A: 10-Second Native Home Screen App (No App Store / Xcode needed)
1. Turn on your ESP8266.
2. On your iPhone, go to **Settings > Wi-Fi** and connect to the ESP's Wi-Fi network:
   - **SSID**: `Yash_RTOS_System's`
   - **Password**: `12345678`
3. Open **Safari** on your iPhone and go to:
   ```
   http://192.168.4.1/
   ```
   *(Or open the `mobile_app/index.html` hosted locally / on any local web server).*
4. Tap the **Share** button (the square with an arrow pointing up at the bottom of Safari).
5. Scroll down and tap **"Add to Home Screen"**.
6. Tap **"Add"** in the top right.
7. **Done!** You now have a standalone app icon on your iPhone home screen with:
   - Full screen display (no Safari address bar or buttons)
   - Real-time relay channel toggles & live power stats
   - Automatic local phone time synchronization

---

## 🤖 How to Run & Install on Android

### Option A: Install via Chrome (PWA Native App)
1. Connect your Android phone to Wi-Fi `Yash_RTOS_System's` (Password: `12345678`).
2. Open **Google Chrome** and browse to:
   ```
   http://192.168.4.1/
   ```
3. Chrome will automatically show an **"Install App"** banner at the bottom (or tap the 3 dots menu &rarr; **"Install app"** / **"Add to Home screen"**).
4. Tap **Install**.
5. The app is installed onto your Android app drawer and home screen just like an APK!

### Option B: Build into an Android APK (using Capacitor)
If you wish to compile a standalone `.apk`:
1. In this directory (`mobile_app`), run:
   ```bash
   npm install
   npx cap add android
   npx cap sync
   npx cap open android
   ```
2. Android Studio will open the native project.
3. Click **Build > Build Bundle(s) / APK(s) > Build APK(s)** to generate your `app-debug.apk`.

---

## ⚡ App Features

1. **Instant Wireless Channel Switching**:
   - 1-tap toggling of Relay 1 (D1), Relay 2 (D2), Relay 3 (D5), Relay 4 (D6).
   - Optimistic tactile response with haptic touch feedback.
   - Master "All ON" and "All OFF" controls.
2. **Fixed Daily Scheduling Engine**:
   - Set 24-hour Start & End times.
   - Select active days of the week (Sun to Sat).
   - Saved directly to ESP8266 LittleFS NVS persistence.
3. **Countdown Timers & Cyclic Automation**:
   - Quick +1m, +5m, +15m, +30m, +1h presets.
   - Repeating duty cycles (ON seconds / OFF seconds / loop count).
4. **Automatic Phone Clock Synchronization**:
   - The app automatically syncs the phone's epoch and timezone offset to the ESP so schedules always fire with second-level accuracy.
5. **Configurable Host / IP**:
   - Tap the top-left connection pill to connect to `192.168.4.1` (direct SoftAP) or any custom IP if your ESP is connected to your home router.
