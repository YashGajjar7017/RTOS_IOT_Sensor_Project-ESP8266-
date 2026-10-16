# ESP32 Real-Time OS 4-Channel Relay Controller with OTA Firmware Flasher & AP Server

A real-time, FreeRTOS multi-core IoT 4-channel relay controller powered by the ESP32. It creates its own standalone Wi-Fi Access Point (AP), serves a responsive high-contrast Slate/Charcoal Web UI on **Port 80**, provides independent countdown timers, cyclic loop automation, daily clock schedules for **Relay 1 (D5)**, **Relay 2 (D18)**, **Relay 3 (D19)**, and **Relay 4 (D21)**, syncs time directly with your smartphone browser, includes a **Permanent Stay-On System**, an intelligent **Low-Power AP Duty-Cycle Mode**, and a complete **Over-The-Air (OTA) Firmware Flashing Engine** on both **Port 80** and dedicated **Port 500**.

---

## Key Features

1. **Standalone Wi-Fi AP (No Router / No Internet Needed)**:
   - Broadcasts its own secure network: `ESP32-Power-Relay` (Password: `12345678`).
   - Serves the dashboard directly on `http://192.168.4.1:80` with automatic captive portal redirect.
2. **Over-The-Air (OTA) Firmware Flashing & LED Signaling**:
   - Upload compiled `.bin` firmware files directly via **Port 80** or dedicated **Port 500**.
   - **Blue LED Status Blink Indicator** (`GPIO 2`):
     - ❌ **3 Blinks**: OTA Flash Failed (bad binary / partition write error).
     - ✅ **5 Blinks**: OTA Flash Succeeded (followed by automatic system reboot).
   - Live percentage progress bar and status feedback during firmware uploads.
3. **Port 500 Security Shield**:
   - Dedicated OTA Flashing Server on Port 500 (`http://192.168.4.1:500`) is **disabled by default** for maximum security.
   - Can be easily enabled or disabled on-demand via the Web GUI Settings toggle.
4. **Zero-Delay Real-Time Architecture (FreeRTOS Multi-Core)**:
   - **Core 1**: Dedicated high-priority Relay Execution, Cyclic Automation & Timer Scheduler task with 100ms precision across all 4 channels.
   - **Core 0**: Wi-Fi, DNS, Web Server processing, OTA engine, and Power Management.
   - Instantaneous response when toggling relays from your phone without blocking delays.
5. **High-Contrast Slate / Light-Black Theme & Dynamic Footer**:
   - Modern, high-visibility Slate & Charcoal UI with crisp luminous borders, bright typography, and vibrant status badges for outdoor and low-light visibility.
   - **Rich Dynamic Footer**: Shows live firmware version (`v2.0.0-OTA`), pin mappings (`D5, D18, D19, D21`), Port 500 security status, and RTOS status.
6. **Permanent Stay-On Mode (Saved in Flash / NVS)**:
   - Dedicated persistent checkbox to keep the system and AP running 24/7 permanently without sleeping.
   - Stored directly in ESP32 Non-Volatile Storage (`Preferences`) across power resets.
7. **Advanced Relay Automations (Per Channel)**:
   - **Manual Switching**: Instant physical toggle with active-low / active-high polarity configuration.
   - **Countdown Timer Mode**: Start Delay and ON Duration with live progress countdown.
   - **Cyclic Repeat Loop**: ON duration, OFF duration, and loop repeat count (or infinite loop) for water pumps, aerators, hydroponics, and cooling fans.
   - **Daily Schedule with Weekday Filters**: Set Turn-ON time (`HH:MM`) and Turn-OFF time (`HH:MM`) with interactive day-of-week checkboxes (Sun-Sat, Weekdays, Weekends).
   - **Power-On Default State**: Choose whether relays boot up as Always OFF, Always ON, or Restore Previous State after a power cut.
8. **Smart Time Synchronization & 12-Hour Display**:
   - **12-Hour Format**: Displays live clock in standard 12-hour format with AM/PM indicators.
   - **1-Click Phone Sync**: Automatically grabs exact timestamp and timezone from mobile browser on load.
   - **Manual Time Setting**: Set custom date and time via the built-in datetime picker in settings.
9. **Live Real-Time Activity Event Stream**:
   - In-memory event stream tracking operations (OTA flashes, Port 500 toggles, timer finishes, schedules, manual clicks, boots) directly on the dashboard.
10. **Telemetry & Total Energy Consumption Metering**:
    - Real-time ESP32 chip temperature (`°C` and `°F`), Free RAM heap, System Uptime, Connected clients, and FreeRTOS Core 0 / Core 1 execution tick counters.
    - Total Combined Energy Consumption summary widget (**kWh**) aggregated across all 4 channels, total runtime, and live connected wattage.
    - Emergency Master Kill Switch ("🚨 EMERGENCY ALL OFF").

---

## Hardware Wiring Diagram

```
 +-------------------------------------------------------+
 |                    ESP32 DEVKIT V1                   |
 |                                                       |
 |  [ VIN ] -------------> Relay Board VCC (+5V Power)   |
 |  [ GND ] -------------> Relay Board GND (Ground)      |
 |  [ D5  / GPIO 5  ] ---> Relay 1 IN1                   |
 |  [ D18 / GPIO 18 ] ---> Relay 2 IN2                   |
 |  [ D19 / GPIO 19 ] ---> Relay 3 IN3                   |
 |  [ D21 / GPIO 21 ] ---> Relay 4 IN4                   |
 |  [ D2  / GPIO 2  ] ---> Onboard Blue Status / OTA LED |
 +-------------------------------------------------------+

 +-------------------------------------------------------+
 |                 4-CHANNEL RELAY MODULE                |
 |                                                       |
 |  VCC  --------------> ESP32 VIN (5V)                  |
 |  GND  --------------> ESP32 GND                       |
 |  IN1  --------------> ESP32 GPIO 5  (D5)              |
 |  IN2  --------------> ESP32 GPIO 18 (D18)             |
 |  IN3  --------------> ESP32 GPIO 19 (D19)             |
 |  IN4  --------------> ESP32 GPIO 21 (D21)             |
 |                                                       |
 |  Relay 1 COM/NO ----> Appliance 1 (e.g. Lights)       |
 |  Relay 2 COM/NO ----> Appliance 2 (e.g. Water Pump)   |
 |  Relay 3 COM/NO ----> Appliance 3 (e.g. Fan / Aerator)|
 |  Relay 4 COM/NO ----> Appliance 4 (e.g. Heater)       |
 +-------------------------------------------------------+
```

### Pin Mapping Table

| Logical Pin | ESP32 GPIO | Description | Default Active Polarity |
| :--- | :--- | :--- | :--- |
| **D5** | `GPIO 5` | Relay 1 Control Pin | Active LOW (Configurable) |
| **D18** | `GPIO 18` | Relay 2 Control Pin | Active LOW (Configurable) |
| **D19** | `GPIO 19` | Relay 3 Control Pin | Active LOW (Configurable) |
| **D21** | `GPIO 21` | Relay 4 Control Pin | Active LOW (Configurable) |
| **LED** | `GPIO 2` | Onboard Blue Status / OTA Blink Indicator | 3 blinks (Fail) / 5 blinks (Success) |

---

## Installation & Flashing Instructions

### Requirements
- **Arduino IDE** (v1.8.19+ or v2.x) OR **PlatformIO** / **Arduino CLI**.
- **ESP32 Arduino Board Package** (v2.0.x or v3.x).
- Standard USB-to-MicroUSB / Type-C cable.

### Step-by-Step Guide
1. Open the Arduino IDE.
2. Open [`IOT_RTOS_Control_System.ino`](file:///c:/Data/Coding/IOT_Code/IOT_RTOS_Control_System/IOT_RTOS_Control_System.ino).
3. In **Tools**, select:
   - **Board**: `ESP32 Dev Module` (or your specific ESP32 variant)
   - **Flash Frequency**: `80MHz`
   - **Upload Speed**: `921600` (or `115200`)
   - **Port**: Select your ESP32 COM port
4. Click **Upload** (Arrow icon).

---

| Method | Endpoint | Parameters | Description |
| :--- | :--- | :--- | :--- |
| `GET` | `/api/status` | None | Returns full JSON system state, 4 relays, Telegram status, timers, power cycles, telemetry, and logs |
| `POST` | `/api/relay` | `id=1&state=1` | Turn ON/OFF/Toggle Relay 1, 2, 3, or 4 |
| `POST` | `/api/all_on` | None | Switch all 4 relays ON simultaneously |
| `POST` | `/api/all_off` | None | Emergency kill switch - immediately forces all 4 relays OFF |
| `POST` | `/api/timer` | `id=1&start_delay=0&duration=600&enable=1` | Start or cancel countdown timer (Input 1: Start Delay, Input 2: ON Duration) |
| `POST` | `/api/cycle` | `id=1&on_sec=30&off_sec=30&cycles=0&enable=1` | Start or cancel cyclic automation loop |
| `POST` | `/api/schedule` | `id=1&start_h=8&start_m=0&end_h=18&end_m=0&days=127&enable=1` | Set daily start/end schedule with weekday bitmask |
| `POST` | `/api/time/sync` | `epoch=1725100000&tz_offset=330` | Sync software RTC with UNIX timestamp & timezone |
| `POST` | `/api/time/manual` | `year=2026&month=8&day=31&hour=16&min=30&sec=0` | Set RTC date and time manually |
| `POST` | `/api/power` | `stay_on=1` OR `enable=1&sleep_min=5&wake_min=2` | Configure Permanent Stay-On or Auto-Sleep Duty Cycle |
| `POST` | `/api/settings` | `r1_name=Lights&r1_watts=100&r1_actlow=1&r1_pwron=0...` | Configure relay names, wattage, active-low polarity, and power-on state |
| `POST` | `/api/telegram/config` | `enable=1&token=...&chat_id=...&sta_ssid=...&sta_pass=...&n_tog=1&n_tim=1&n_sch=1` | Save Telegram Bot credentials, Wi-Fi Station settings, and alert preferences |
| `POST` | `/api/telegram/test` | None | Send a live test message to your Telegram chat |
| `POST` | `/api/ota/upload` | Multipart file `update` (.bin) | Upload & flash compiled firmware via Port 80 (3 blinks on fail, 5 blinks on success) |
| `POST` | `/api/ota/port500` | `enable=1` or `enable=0` | Security switch to enable/disable Port 500 OTA server |
| `GET` | `http://<IP>:500/` | None | Standalone dedicated OTA Flashing Station Web Portal (when Port 500 enabled) |
| `POST` | `http://<IP>:500/update` | Multipart file `update` (.bin) | Upload & flash compiled firmware via Port 500 |

---

## Telegram Bot Integration & Remote Commands

When configured with your home/office Wi-Fi router credentials, the ESP32 operates in simultaneous **AP + Station Mode** (`WIFI_AP_STA`), enabling full bidirectional communication with Telegram:

### Available Telegram Commands:
- `/status` — Receive formatted real-time status of all 4 relays, load power, total energy (kWh), chip temperature, and system uptime.
- `/r1_on`, `/r1_off`, `/r1_toggle` — Control Relay 1 (D5)
- `/r2_on`, `/r2_off`, `/r2_toggle` — Control Relay 2 (D18)
- `/r3_on`, `/r3_off`, `/r3_toggle` — Control Relay 3 (D19)
- `/r4_on`, `/r4_off`, `/r4_toggle` — Control Relay 4 (D21)
- `/all_on` — Turn all relays ON
- `/all_off` — Emergency kill switch (All relays OFF)
- `/help` or `/start` — Display the interactive command guide

---

## License & Credits
Developed for ESP32 Dual-Core Embedded Real-Time IoT Systems. Free and open source under MIT License.
