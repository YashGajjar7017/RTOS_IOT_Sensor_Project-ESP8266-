#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESP8266WebServer.h>
  #include <ESP8266HTTPClient.h>
  #include <WiFiClientSecure.h>
  #include <Updater.h>
  typedef ESP8266WebServer WebServer;

  // FreeRTOS compatibility stubs for single-core ESP8266
  typedef void* TaskHandle_t;
  typedef void* SemaphoreHandle_t;
  typedef uint32_t TickType_t;
  #ifndef pdTRUE
    #define pdTRUE 1
  #endif
  #ifndef pdFALSE
    #define pdFALSE 0
  #endif
  #ifndef portMAX_DELAY
    #define portMAX_DELAY 0xFFFFFFFFUL
  #endif
  #ifndef pdMS_TO_TICKS
    #define pdMS_TO_TICKS(ms) (ms)
  #endif
  #define xSemaphoreCreateMutex() ((SemaphoreHandle_t)1)
  #define vSemaphoreDelete(s) do {} while(0)
  #define xSemaphoreTake(s, t) (pdTRUE)
  #define xSemaphoreGive(s) (pdTRUE)
  #define xTaskGetTickCount() millis()
#else
  #include <WiFi.h>
  #include <WebServer.h>
  #include <HTTPClient.h>
  #include <WiFiClientSecure.h>
  #include <Update.h>
#endif

// =====================================================================
// FIRMWARE VERSION
// =====================================================================
#define FIRMWARE_VERSION "v2.0.0-OTA"

// =====================================================================
// HARDWARE PIN DEFINITIONS
// =====================================================================
#if defined(ESP8266)
// ESP8266 (NodeMCU / Wemos D1 Mini) Pinout:
// Relay 1 -> D1 (GPIO 5)
// Relay 2 -> D2 (GPIO 4)
// Relay 3 -> D5 (GPIO 14)
// Relay 4 -> D6 (GPIO 12)
// Status LED -> D4 (GPIO 2, Onboard Blue LED, Active LOW)
#define RELAY_1_PIN 5   // D1 / GPIO 5
#define RELAY_2_PIN 4   // D2 / GPIO 4
#define RELAY_3_PIN 14  // D5 / GPIO 14
#define RELAY_4_PIN 12  // D6 / GPIO 12
#define STATUS_LED_PIN 2 // D4 / GPIO 2 (Onboard Blue LED)
#else
// ESP32 DevKit 4-Channel Relay Pinout:
#define RELAY_1_PIN 5   // D5 / GPIO 5
#define RELAY_2_PIN 18  // D18 / GPIO 18
#define RELAY_3_PIN 19  // D19 / GPIO 19
#define RELAY_4_PIN 21  // D21 / GPIO 21
#define STATUS_LED_PIN 2 // Onboard Blue LED
#endif

// =====================================================================
// DEFAULT ACCESS POINT (AP) CONFIGURATION
// =====================================================================
#define DEFAULT_AP_SSID "Yash_RTOS_System's"
#define DEFAULT_AP_PASS "12345678" // Minimum 8 chars, or "" for open AP
#define DEFAULT_AP_CHANNEL 1
#define DEFAULT_AP_MAX_CONN 4

#define AP_IP_OCTET_1 192
#define AP_IP_OCTET_2 168
#define AP_IP_OCTET_3 4
#define AP_IP_OCTET_4 1

#define WEB_SERVER_PORT 80
#define OTA_SERVER_PORT 500
#define DNS_PORT 53

// =====================================================================
// LOW POWER / AP DUTY CYCLE CONFIGURATION
// =====================================================================
#define DEFAULT_AP_INACTIVITY_TIMEOUT_MS                                       \
  (2 * 60 * 1000) // 2 minutes before AP shuts down in Low Power Mode
#define DEFAULT_AP_SLEEP_INTERVAL_MS                                           \
  (5 * 60 * 1000) // 5 minutes sleep interval before AP wakes up again
#define DEFAULT_AP_WAKE_WINDOW_MS                                              \
  (2 * 60 * 1000) // 2 minutes discovery/connect window upon wake up

// =====================================================================
// FREERTOS CONFIGURATION
// =====================================================================
#define RELAY_TASK_CORE 1     // Run time-critical Relay logic on Core 1
#define RELAY_TASK_PRIORITY 2 // High priority for zero-delay accuracy
#define RELAY_TASK_STACK 4096

#define SERVER_TASK_CORE 0 // Run Web Server & Wi-Fi on Core 0
#define SERVER_TASK_PRIORITY 1
#define SERVER_TASK_STACK 8192

#define POWER_TASK_CORE 0 // Run AP Power Cycle manager on Core 0
#define POWER_TASK_PRIORITY 1
#define POWER_TASK_STACK 3072

// Maximum event log entries stored in RAM
#define MAX_EVENT_LOGS 16

// =====================================================================
// DATA STRUCTURES
// =====================================================================

// Power-On Default State Behavior
enum PowerOnState : uint8_t {
  POWERON_OFF = 0,         // Always boot up OFF (Safest)
  POWERON_ON = 1,          // Always boot up ON
  POWERON_RESTORE_LAST = 2 // Remember & restore last state before power off
};

// Relay Countdown Timer Configuration
struct RelayTimerConfig {
  bool enabled;             // Is countdown timer active?
  uint32_t startDelaySec;   // Delay before turning ON (seconds)
  uint32_t durationSec;     // Time to stay ON before turning OFF (seconds)
  uint32_t timerStartEpoch; // Epoch when timer was initiated
  uint32_t remainingSec;    // Current remaining seconds for UI
  bool isCountingDown;      // Currently in delay or active ON phase
  bool inDelayPhase; // True if waiting to turn ON, false if ON and timing
                     // duration
};

// Relay Cycle / Repeat Automation (e.g. pumps, aerators, hydroponics, cooling)
struct RelayCycleConfig {
  bool enabled;          // Is cycle mode running?
  uint32_t onSec;        // Duration to stay ON per cycle
  uint32_t offSec;       // Duration to stay OFF per cycle
  uint32_t totalCycles;  // 0 for infinite repeat, or specific count (e.g. 5)
  uint32_t currentCycle; // Current completed cycle index
  uint32_t remainingSec; // Remaining seconds in current phase
  bool inOnPhase;        // True if currently in ON phase, false if OFF
};

// Relay Daily Schedule Configuration (24h Clock)
struct RelayScheduleConfig {
  bool enabled;        // Is daily schedule active?
  uint8_t startHour;   // 0-23
  uint8_t startMinute; // 0-59
  uint8_t startSecond; // 0-59
  uint8_t endHour;     // 0-23
  uint8_t endMinute;   // 0-59
  uint8_t endSecond;   // 0-59
  uint8_t daysActive;  // Bitmask: bit 0=Sun, 1=Mon, ..., 6=Sat, 7=All
};

// Full Relay Channel State & Settings
struct RelayChannel {
  uint8_t id;       // 1 or 2
  uint8_t pin;      // GPIO pin
  char name[24];    // Friendly channel name
  bool state;       // Current physical output state (true = ON)
  bool activeLow;   // True if relay triggers on LOW (most modules)
  float ratedWatts; // Connected load power in Watts (for kWh calculation)
  uint32_t totalOnSeconds;   // Total lifetime ON runtime in seconds
  uint32_t sessionOnSeconds; // Current session ON runtime
  uint8_t powerOnState;      // Default state on boot (POWERON_OFF, POWERON_ON,
                             // POWERON_RESTORE_LAST)

  RelayTimerConfig timer;       // Countdown timer
  RelayCycleConfig cycle;       // Cyclic loop timer
  RelayScheduleConfig schedule; // Daily schedule
};

// Low Power Mode Settings
struct LowPowerConfig {
  bool enabled;         // Low power AP duty-cycling enabled?
  bool permanentStayOn; // When true: Keep AP & System always ON permanently
                        // (Disables sleep)
  bool isApSleeping;    // Is AP currently shut off?
  uint32_t sleepIntervalMin;     // Interval between wake-ups (e.g., 15-20 min)
  uint32_t wakeWindowMin;        // Discovery window (e.g., 3 min)
  uint32_t inactivityTimeoutMin; // Timeout with no clients before sleeping
  uint32_t nextStateChangeEpoch; // Epoch of next sleep/wake transition
};

// Telemetry & System Diagnostics
struct SystemTelemetry {
  float internalTempC;     // ESP32 Chip temperature in Celsius
  float internalTempF;     // In Fahrenheit
  uint32_t freeHeapBytes;  // Free RAM
  uint32_t uptimeSeconds;  // System uptime
  uint8_t wifiClients;     // Connected stations on AP
  bool timeSynchronized;   // Has time been synced with phone/manual?
  uint32_t rtosRelayTicks; // Real-Time task tick count
  uint32_t rtosPowerTicks; // Power manager tick count
};

// Real-Time System Event Log Entry
struct EventLogEntry {
  char timestamp[12]; // "HH:MM:SS"
  char message[64];   // Event summary
};

// Telegram Bot Configuration
struct TelegramConfig {
  bool enabled;          // Telegram notifications & remote commands enabled?
  char botToken[64];     // Telegram Bot Token (e.g. "123456789:ABCdef...")
  char chatId[32];       // Telegram Target Chat / User ID
  char staSsid[32];      // Home/Office Wi-Fi SSID for internet access
  char staPass[64];      // Home/Office Wi-Fi Password
  bool notifyOnToggle;   // Notify when relay is switched ON/OFF
  bool notifyOnTimer;    // Notify when timer completes
  bool notifyOnSchedule; // Notify when schedule triggers
  bool staConnected;     // Is Wi-Fi Station connected to internet?
  uint32_t lastMessageEpoch; // Last message timestamp
};

#endif // CONFIG_H
