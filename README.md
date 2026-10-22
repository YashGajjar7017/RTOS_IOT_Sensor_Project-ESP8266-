# ESP8266 IoT Real-Time 4-Channel Relay Controller with OTA Flasher & Telegram Bot

A robust, real-time IoT 4-channel relay automation system powered by the **ESP8266 (NodeMCU / Wemos D1 Mini / ESP-12E/F)**. It creates its own high-speed standalone Wi-Fi Access Point (AP), serves a responsive glassmorphic Web UI on **Port 80**, provides independent countdown timers, cyclic automation loops, and daily clock schedules in **12-Hour Indian Standard Time (IST)** for all 4 channels, syncs time directly with your smartphone browser, includes a **Permanent Stay-On System**, an intelligent **Low-Power AP Duty-Cycle Mode**, and a complete **Over-The-Air (OTA) Firmware Flashing Engine** on both **Port 80** and dedicated **Port 500**.

---

## ⚡ Key Features

1. **Standalone Wi-Fi AP (No External Router Needed)**:
   - Broadcasts its own secure network: `Yash_RTOS_System's` (Default Password: `12345678`).
   - Serves the dashboard directly on `http://192.168.4.1` with automatic captive portal redirect.
2. **Safe ESP8266 Boot Pin Architecture (Zero Boot Chatter & No Lockups)**:
   - Uses dedicated non-strapping GPIOs: **D1 (GPIO 5)**, **D2 (GPIO 4)**, **D5 (GPIO 14)**, and **D6 (GPIO 12)**.
   - Avoids dangerous strapping pins (GPIO 0, GPIO 2, GPIO 15), guaranteeing 100% reliable boot-ups.
   - **Zero Power-On Chatter**: Relay pins are driven HIGH prior to setting output mode, preventing relays from clicking ON during power-up.
3. **12-Hour Indian Standard Time (IST) Schedules**:
   - Schedule Turn-ON and Turn-OFF times in native **12-hour AM/PM format (IST)**.
   - **Transition-Edge Triggering**: Manual ON/OFF toggling from the Web GUI or Telegram does not get overridden every second by the daily schedule.
   - Interactive day-of-week selection (All Days, Mon-Fri, Weekends).
4. **Smart Low-Power AP Duty Cycle & Uninterrupted Backend Execution**:
   - When Low Power Mode is active, the ESP8266 disconnects (powers down) the Wi-Fi AP channel after user inactivity to save power and lower chip heat.
   - **Continuous Backend Execution**: The relay execution engine, timers, cycle loops, and daily schedules continue running seamlessly in the background even when Wi-Fi is sleeping!
   - Onboard Blue LED displays a silky smooth sinusoidal breathing effect while sleeping.
   - **Permanent Stay-On Mode**: Easily toggle 24/7 Always-On mode directly from the GUI.
5. **Over-The-Air (OTA) Firmware Flashing & LED Signaling**:
   - Upload compiled `.bin` firmware files directly via **Port 80** or isolated **Port 500**.
   - **Onboard Status LED Indicator (D4 / GPIO 2)**:
     - ❌ **3 Blinks**: OTA Flash Failed.
     - ✅ **5 Blinks**: OTA Flash Succeeded (followed by automatic system reboot).
   - Live percentage progress bar and status feedback during firmware uploads.
6. **Telegram Bot Integration & Remote Control**:
   - Connects to home/office Wi-Fi station in simultaneous `WIFI_AP_STA` mode.
   - Receive instant push notifications upon relay state changes, timer completions, and schedule triggers.
   - Full remote 2-way control (`/r1_on`, `/r1_off`, `/status`, `/all_off`, etc.).
7. **Sticky Navbar & Mobile-Optimized Glassmorphic UI**:
   - Full-width sticky top navigation bar with backdrop blur that never jitters or obstructs content during scrolling.
   - Live relay state indicator badges (🟢 ON / ⚪ OFF) visible on collapsed cards.
   - Real-time energy monitoring (kWh), total lifetime runtime, and connected wattage.

---

## 🔌 Hardware Pinout & Wiring Connections

### ESP8266 NodeMCU / D1 Mini Pin Mapping

| Relay Channel | ESP8266 Pin | GPIO Number | Function / Safe Boot Role | Connected Appliance Example |
| :--- | :--- | :--- | :--- | :--- |
| **Relay 1** | **D1** | `GPIO 5` | Safe Boot, General GPIO (I2C SCL) | **Appliance 1** (e.g. Room Lights) |
| **Relay 2** | **D2** | `GPIO 4` | Safe Boot, General GPIO (I2C SDA) | **Appliance 2** (e.g. Ceiling Fan / Exhaust) |
| **Relay 3** | **D5** | `GPIO 14` | Safe Boot, General GPIO (SPI SCK) | **Appliance 3** (e.g. Water Pump / Geyser) |
| **Relay 4** | **D6** | `GPIO 12` | Safe Boot, General GPIO (SPI MISO) | **Appliance 4** (e.g. Cooler / Heater) |
| **Status LED** | **D4** | `GPIO 2` | Onboard Blue LED (Active LOW) | OTA 3/5 Blinks & Sleep Breathing Light |

> [!IMPORTANT]
> **Why GPIO 5, 4, 14, 12?**
> On ESP8266, pins `D3` (GPIO 0), `D4` (GPIO 2), and `D8` (GPIO 15) determine the boot mode (UART download vs Flash boot). Connecting relay coils to those strapping pins can pull them high/low at boot, causing the ESP8266 to hang in flash download mode or fail to boot. Using `D1`, `D2`, `D5`, and `D6` completely eliminates this issue.

---

### Complete Wiring Diagram

```
 +-------------------------------------------------------------+
 |                   ESP8266 NODEMCU / D1 MINI                |
 |                                                             |
 |  [ VIN / 5V ] ----------> Relay Board VCC (+5V Power)       |
 |  [ GND ] ---------------> Relay Board GND (Ground)          |
 |  [ D1  / GPIO 5  ] -----> Relay 1 IN1                       |
 |  [ D2  / GPIO 4  ] -----> Relay 2 IN2                       |
 |  [ D5  / GPIO 14 ] -----> Relay 3 IN3                       |
 |  [ D6  / GPIO 12 ] -----> Relay 4 IN4                       |
 |  [ D4  / GPIO 2  ] -----> Onboard Blue LED (Built-in)       |
 +-------------------------------------------------------------+

 +-------------------------------------------------------------+
 |                   4-CHANNEL 5V RELAY MODULE                 |
 |                                                             |
 |  VCC  ------------------> ESP8266 VIN (5V Supply)           |
 |  GND  ------------------> ESP8266 GND                       |
 |  IN1  ------------------> ESP8266 D1 (GPIO 5)               |
 |  IN2  ------------------> ESP8266 D2 (GPIO 4)               |
 |  IN3  ------------------> ESP8266 D5 (GPIO 14)              |
 |  IN4  ------------------> ESP8266 D6 (GPIO 12)              |
 |                                                             |
 |  Relay 1 [COM & NO] ----> Load 1 (e.g. Living Room Lights)  |
 |  Relay 2 [COM & NO] ----> Load 2 (e.g. Exhaust Fan / Cooler)|
 |  Relay 3 [COM & NO] ----> Load 3 (e.g. Submersible Pump)    |
 |  Relay 4 [COM & NO] ----> Load 4 (e.g. Water Geyser/Heater) |
 +-------------------------------------------------------------+
```

---

## 🛠️ Arduino IDE Setup & Flashing

### 1. Board Package & Settings
- Install **ESP8266 by ESP8266 Community** via Arduino IDE Boards Manager (`v3.0.x` or `v3.1.x`).
- In **Tools** menu, select:
  - **Board**: `NodeMCU 1.0 (ESP-12E Module)` OR `LOLIN(WEMOS) D1 R2 & mini`
  - **CPU Frequency**: `80 MHz` (or `160 MHz` for maximum network performance)
  - **Flash Size**: `4MB (FS: 2MB)` or `4MB (FS: 1MB OTA:~1019KB)`
  - **Upload Speed**: `921600` (or `115200` if upload fails)
  - **Port**: Select your USB Serial COM Port (e.g. `COM3` on Windows)

### 2. Flashing
1. Open [`ESP8266.ino`](file:///a:/Coding/IOT_Code/IOT_RealTime_OS/ESP8266/ESP8266.ino).
2. Click **Verify / Compile** (Checkmark icon).
3. Click **Upload** (Arrow icon).

---

## 🌐 Web Dashboard & REST API Endpoints

Once powered on, connect to Wi-Fi SSID `Yash_RTOS_System's` (Password: `12345678`) and open `http://192.168.4.1` in your browser.

| Method | Endpoint | Query / Body Parameters | Action Summary |
| :--- | :--- | :--- | :--- |
| `GET` | `/api/status` | None | Full JSON telemetry, 4 relays, power mode, IST clock, event logs |
| `POST` | `/api/relay` | `id=1&state=1` (or `0`) | Switch Relay 1, 2, 3, or 4 ON/OFF (or toggle if state omitted) |
| `POST` | `/api/all_on` | None | Turn all 4 relays ON simultaneously |
| `POST` | `/api/all_off` | None | Emergency master kill switch — immediately shuts off all relays |
| `POST` | `/api/timer` | `id=1&start_delay=0&duration=600&enable=1` | Start countdown timer (Delay + ON duration) |
| `POST` | `/api/cycle` | `id=1&on_sec=30&off_sec=30&cycles=0&enable=1` | Start cyclic repeat loop (pumps/aerators) |
| `POST` | `/api/schedule` | `id=1&start_h=8&start_m=0&end_h=18&end_m=0&days=127&enable=1` | Set daily IST schedule with weekday bitmask |
| `POST` | `/api/time/sync` | `epoch=1725100000&tz_offset=330` | Sync time from phone browser with IST timezone (+330 min) |
| `POST` | `/api/time/manual` | `year=2026&month=10&day=19&hour=16&min=30&sec=0` | Manually configure RTC date & time |
| `POST` | `/api/power` | `stay_on=1` OR `enable=1&sleep_min=5&wake_min=2` | Toggle 24/7 Always-On vs Auto-Sleep AP Duty Cycle |
| `POST` | `/api/settings` | `r1_name=Lights&r1_watts=100&r1_actlow=1&r1_pwron=0...` | Update channel names, wattages, and boot behaviors |
| `POST` | `/api/telegram/config` | `enable=1&token=...&chat_id=...&sta_ssid=...&sta_pass=...` | Save Telegram bot token, chat ID, and router Wi-Fi credentials |
| `POST` | `/api/telegram/test` | None | Trigger a test message to your Telegram account |
| `POST` | `/api/ota/upload` | Multipart file `update` (`.bin`) | Flash firmware over Port 80 (3 blinks fail, 5 blinks success) |
| `POST` | `/api/ota/port500` | `enable=1` or `0` | Enable or disable isolated Port 500 OTA server |
| `GET` | `http://<IP>:500/` | None | Dedicated standalone OTA flasher UI (when enabled) |

---

## ✈️ Telegram Bot Commands

When connected to an internet Wi-Fi router, the ESP8266 supports bidirectional Telegram control:

- `/status` — Get live report of all 4 relays, power usage (kWh), chip stats, and uptime.
- `/r1_on`, `/r1_off`, `/r1_toggle` — Control Relay 1 (D1)
- `/r2_on`, `/r2_off`, `/r2_toggle` — Control Relay 2 (D2)
- `/r3_on`, `/r3_off`, `/r3_toggle` — Control Relay 3 (D5)
- `/r4_on`, `/r4_off`, `/r4_toggle` — Control Relay 4 (D6)
- `/all_on` — Turn all 4 relays ON
- `/all_off` — Emergency kill switch (All relays forced OFF)
- `/help` — Show command instructions

---

## 🔒 Power-On State Options

Stored persistently in flash memory (`LittleFS Preferences`):
1. **Always Boot OFF (Default & Recommended)**: Safest setting. Relays stay deactivated when mains electricity turns on.
2. **Always Boot ON**: Relays automatically turn ON immediately when powered.
3. **Restore Last Known State**: Remembers the state before power loss and restores it after reboot.

---

## 🛡️ RTOS Kernel & Exception Subsystem

The firmware features an integrated **RTOS Kernel Supervisor** (`KernelManager.h` & `KernelManager.cpp`) that continuously guards the system against instability, memory leaks, and hardware stalls without disrupting relay states:

1. **Task Heartbeat Watchdogs**:
   - Monitors `RelayTask` (100ms precision) and `PowerTask` (25ms precision).
   - If any core RTOS task fails to report within 5,000ms, the kernel logs a `FAULT_TASK_STALL` exception.
2. **Dynamic Heap Memory Guard**:
   - Continuously samples free DRAM. If free heap falls below 4.5 KB, a `FAULT_LOW_HEAP_MEMORY` warning is raised.
3. **Hardware GPIO Verifier**:
   - Confirms that active output pins match the expected logical states. Any pin float or high-impedance state raises `FAULT_HARDWARE_PIN_MISMATCH`.
4. **Exception Ring Buffer & Safe Fail-Safe**:
   - Retains the last 16 timestamped exceptions with severity tags (`SEV_INFO`, `SEV_WARNING`, `SEV_CRITICAL`, `SEV_FATAL`).
   - If a `SEV_FATAL` fault triggers, the kernel automatically enters a safe fail-safe state, shutting off all relay loads to protect connected appliances before performing a clean watchdog restart.
5. **Real-Time UI & App Exception Banners**:
   - Faults are streamed via `/api/status` under the `"kernel"` object and displayed prominently in both the Web UI and Mobile App with a one-touch **Acknowledge / Clear** button (`POST /api/kernel/clear`).

---

## 📱 Mobile App (Android Native + iPhone iOS)

Located in the [`mobile_app/`](mobile_app/) directory, this application allows wireless control of all 4 channels without internet or cloud servers.

### Android Native App
- **Native Android Studio Project**: Located in `mobile_app/android/`.
- **Preconfigured for Local Wi-Fi**: Includes `usesCleartextTraffic="true"` in `AndroidManifest.xml` to allow direct communication with `http://192.168.4.1`.
- **Haptic Feedback & Low Latency**: Fast, optimistic UI transitions with physical vibration confirmation on relay toggles.
- **Build / Run**:
  ```bash
  cd mobile_app/android
  .\gradlew.bat assembleDebug
  ```
  The compiled APK will be generated at:
  `mobile_app/android/app/build/outputs/apk/debug/app-debug.apk`

### iPhone / iOS PWA
- Optimized for Apple Safari as a Progressive Web App (PWA).
- **Standalone Mode**: Configured with `apple-mobile-web-app-capable`, `apple-mobile-web-app-status-bar-style: black-translucent`, and iOS touch icons.
- **How to Install on iPhone**:
  1. Connect your iPhone to the ESP8266 Wi-Fi AP (`Yash_RTOS_System's`).
  2. Open Safari and navigate to `http://192.168.4.1` (or host the `mobile_app/www` files).
  3. Tap the **Share** button (box with upward arrow) and select **"Add to Home Screen"**.
  4. The app launches full-screen with native gestures and haptic switches.

