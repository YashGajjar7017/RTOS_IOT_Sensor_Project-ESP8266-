#include <Arduino.h>
#include "Config.h"
#include <DNSServer.h>
#include "TimeManager.h"
#include "RelayManager.h"
#include "PowerManager.h"
#include "TelegramManager.h"
#include "WebPages.h"

// Primary Web Server and Captive Portal DNS Server
WebServer server(WEB_SERVER_PORT);

// Secondary Dedicated OTA Server (Port 500)
WebServer server500(OTA_SERVER_PORT);
DNSServer dnsServer;

// OTA & System State Management
bool isPort500Enabled = false;       // Security: Disabled by default
bool isRebootPending = false;
unsigned long rebootAtMs = 0;

// FreeRTOS Task Handles
TaskHandle_t xRelayTaskHandle = NULL;
TaskHandle_t xPowerTaskHandle = NULL;

// Helper function to blink the status LED
// 3 blinks = OTA update failure
// 5 blinks = OTA update success
void blinkStatusLed(int times, int delayMs = 150) {
    pinMode(STATUS_LED_PIN, OUTPUT);
    for (int i = 0; i < times; i++) {
        digitalWrite(STATUS_LED_PIN, HIGH);
        delay(delayMs);
        digitalWrite(STATUS_LED_PIN, LOW);
        if (i < times - 1) delay(delayMs);
    }
}

// Helper to read internal chip temperature
#if defined(ESP32)
#ifdef __cplusplus
extern "C" {
#endif
uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif
#endif

float getESP32TemperatureC() {
#if defined(ESP8266)
    return 32.0f; // ESP8266 has no internal die temperature sensor
#elif defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 3)
    return temperatureRead();
#else
    return (temprature_sens_read() - 32) / 1.8f;
#endif
}

// =====================================================================
// REST API & WEB SERVER HANDLERS
// =====================================================================

void handleRoot() {
    PowerManager::getInstance().registerUserActivity();
    server.send_P(200, "text/html", INDEX_HTML);
}

void handleStatus() {
    // Note: Do not registerUserActivity on background status polling so auto-sleep timer can count down accurately

    RelayChannel r1 = RelayManager::getInstance().getChannel(1);
    RelayChannel r2 = RelayManager::getInstance().getChannel(2);
    RelayChannel r3 = RelayManager::getInstance().getChannel(3);
    RelayChannel r4 = RelayManager::getInstance().getChannel(4);
    LowPowerConfig pwr = PowerManager::getInstance().getConfig();
    TelegramConfig tg = TelegramManager::getInstance().getConfig();

    char timeBuf[16];
    char dateBuf[16];
    TimeManager::getInstance().getFormattedTime(timeBuf, sizeof(timeBuf));
    TimeManager::getInstance().getFormattedDate(dateBuf, sizeof(dateBuf));

    float tempC = getESP32TemperatureC();
    float tempF = (tempC * 1.8f) + 32.0f;
    uint32_t freeHeap = ESP.getFreeHeap();
    uint32_t uptime = millis() / 1000;
    uint8_t clients = WiFi.softAPgetStationNum();

    // Fetch Event logs
    EventLogEntry logs[MAX_EVENT_LOGS];
    int logCount = RelayManager::getInstance().getEventLogs(logs, MAX_EVENT_LOGS);

    String json;
    json.reserve(1500);
    json = "{";

    // Time object
    json += "\"time\":{";
    json += "\"clock\":\"" + String(timeBuf) + "\",";
    json += "\"date\":\"" + String(dateBuf) + "\",";
    json += "\"epoch\":" + String(TimeManager::getInstance().getEpochTime()) + ",";
    json += "\"synced\":" + String(TimeManager::getInstance().isTimeSynchronized() ? "true" : "false");
    json += "},";

    // Helper lambda to append relay JSON
    auto appendRelayJson = [](String& outJson, const char* key, const RelayChannel& r) {
        outJson += "\"" + String(key) + "\":{";
        outJson += "\"name\":\"" + String(r.name) + "\",";
        outJson += "\"state\":" + String(r.state ? "true" : "false") + ",";
        outJson += "\"act_low\":" + String(r.activeLow ? "true" : "false") + ",";
        outJson += "\"watts\":" + String(r.ratedWatts, 1) + ",";
        outJson += "\"total_sec\":" + String(r.totalOnSeconds) + ",";
        outJson += "\"pwr_on\":" + String(r.powerOnState) + ",";
        outJson += "\"timer\":{";
        outJson += "\"active\":" + String(r.timer.isCountingDown ? "true" : "false") + ",";
        outJson += "\"in_delay\":" + String(r.timer.inDelayPhase ? "true" : "false") + ",";
        outJson += "\"delay_sec\":" + String(r.timer.startDelaySec) + ",";
        outJson += "\"dur_sec\":" + String(r.timer.durationSec) + ",";
        outJson += "\"remaining\":" + String(r.timer.remainingSec);
        outJson += "},";
        outJson += "\"cycle\":{";
        outJson += "\"active\":" + String(r.cycle.enabled ? "true" : "false") + ",";
        outJson += "\"in_on\":" + String(r.cycle.inOnPhase ? "true" : "false") + ",";
        outJson += "\"on_sec\":" + String(r.cycle.onSec) + ",";
        outJson += "\"off_sec\":" + String(r.cycle.offSec) + ",";
        outJson += "\"total_cycles\":" + String(r.cycle.totalCycles) + ",";
        outJson += "\"current_cycle\":" + String(r.cycle.currentCycle) + ",";
        outJson += "\"remaining\":" + String(r.cycle.remainingSec);
        outJson += "},";
        outJson += "\"schedule\":{";
        outJson += "\"enabled\":" + String(r.schedule.enabled ? "true" : "false") + ",";
        outJson += "\"start_h\":" + String(r.schedule.startHour) + ",";
        outJson += "\"start_m\":" + String(r.schedule.startMinute) + ",";
        outJson += "\"end_h\":" + String(r.schedule.endHour) + ",";
        outJson += "\"end_m\":" + String(r.schedule.endMinute) + ",";
        outJson += "\"days\":" + String(r.schedule.daysActive);
        outJson += "}";
        outJson += "},";
    };

    appendRelayJson(json, "r1", r1);
    appendRelayJson(json, "r2", r2);
    appendRelayJson(json, "r3", r3);
    appendRelayJson(json, "r4", r4);

    // Power saver object
    json += "\"power\":{";
    json += "\"enabled\":" + String(pwr.enabled ? "true" : "false") + ",";
    json += "\"stay_on\":" + String(pwr.permanentStayOn ? "true" : "false") + ",";
    json += "\"sleeping\":" + String(pwr.isApSleeping ? "true" : "false") + ",";
    json += "\"sleep_min\":" + String(pwr.sleepIntervalMin) + ",";
    json += "\"wake_min\":" + String(pwr.wakeWindowMin) + ",";
    json += "\"next_state_sec\":" + String(PowerManager::getInstance().getSecondsUntilNextState());
    json += "},";

    // Telegram Bot object
    json += "\"tg\":{";
    json += "\"enabled\":" + String(tg.enabled ? "true" : "false") + ",";
    json += "\"connected\":" + String(tg.staConnected ? "true" : "false") + ",";
    json += "\"bot_set\":" + String(strlen(tg.botToken) > 0 ? "true" : "false") + ",";
    json += "\"chat_id\":\"" + String(tg.chatId) + "\",";
    json += "\"sta_ssid\":\"" + String(tg.staSsid) + "\",";
    json += "\"n_tog\":" + String(tg.notifyOnToggle ? "true" : "false") + ",";
    json += "\"n_tim\":" + String(tg.notifyOnTimer ? "true" : "false") + ",";
    json += "\"n_sch\":" + String(tg.notifyOnSchedule ? "true" : "false");
    json += "},";

    // Telemetry object
    json += "\"telem\":{";
    json += "\"temp_c\":" + String(tempC, 1) + ",";
    json += "\"temp_f\":" + String(tempF, 1) + ",";
    json += "\"free_heap\":" + String(freeHeap) + ",";
    json += "\"uptime_sec\":" + String(uptime) + ",";
    json += "\"clients\":" + String(clients) + ",";
    json += "\"rtos_relay_ticks\":" + String(RelayManager::getInstance().getTickCount()) + ",";
    json += "\"rtos_power_ticks\":" + String(PowerManager::getInstance().getTickCount());
    json += "},";

    // Event Logs Array
    json += "\"logs\":[";
    for (int i = 0; i < logCount; i++) {
        json += "{\"time\":\"" + String(logs[i].timestamp) + "\",\"msg\":\"" + String(logs[i].message) + "\"}";
        if (i < logCount - 1) json += ",";
    }
    json += "],";

    // System Firmware & Security Status
    json += "\"fw_ver\":\"" + String(FIRMWARE_VERSION) + "\",";
    json += "\"port500\":" + String(isPort500Enabled ? "true" : "false");

    json += "}";

    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", json);
}

void handleRelayToggle() {
    PowerManager::getInstance().registerUserActivity();
    server.sendHeader("Access-Control-Allow-Origin", "*");

    if (server.hasArg("id")) {
        uint8_t id = server.arg("id").toInt();
        if (server.hasArg("state")) {
            bool state = (server.arg("state").toInt() == 1);
            RelayManager::getInstance().setRelayState(id, state);
        } else {
            RelayManager::getInstance().toggleRelay(id);
        }
        server.send(200, "text/plain", "OK");
    } else {
        server.send(400, "text/plain", "Missing id");
    }
}

void handleTimerSet() {
    PowerManager::getInstance().registerUserActivity();
    server.sendHeader("Access-Control-Allow-Origin", "*");

    if (server.hasArg("id")) {
        uint8_t id = server.arg("id").toInt();
        bool enable = (server.arg("enable").toInt() == 1);
        uint32_t delaySec = server.hasArg("start_delay") ? server.arg("start_delay").toInt() : 0;
        uint32_t durSec = server.hasArg("duration") ? server.arg("duration").toInt() : 0;

        RelayManager::getInstance().setCountdownTimer(id, delaySec, durSec, enable);
        server.send(200, "text/plain", "Timer Updated");
    } else {
        server.send(400, "text/plain", "Missing id");
    }
}

void handleCycleSet() {
    PowerManager::getInstance().registerUserActivity();
    server.sendHeader("Access-Control-Allow-Origin", "*");

    if (server.hasArg("id")) {
        uint8_t id = server.arg("id").toInt();
        bool enable = (server.arg("enable").toInt() == 1);
        uint32_t onSec = server.hasArg("on_sec") ? server.arg("on_sec").toInt() : 60;
        uint32_t offSec = server.hasArg("off_sec") ? server.arg("off_sec").toInt() : 60;
        uint32_t cycles = server.hasArg("cycles") ? server.arg("cycles").toInt() : 0;

        RelayManager::getInstance().setCycleAutomation(id, onSec, offSec, cycles, enable);
        server.send(200, "text/plain", "Cycle Updated");
    } else {
        server.send(400, "text/plain", "Missing id");
    }
}

/* Commented out for now
void handlePulseSet() {
    PowerManager::getInstance().registerUserActivity();
    server.sendHeader("Access-Control-Allow-Origin", "*");

    if (server.hasArg("id")) {
        uint8_t id = server.arg("id").toInt();
        uint32_t ms = server.hasArg("ms") ? server.arg("ms").toInt() : 1000;
        // RelayManager::getInstance().triggerPulse(id, ms);
        server.send(200, "text/plain", "Pulse Triggered");
    } else {
        server.send(400, "text/plain", "Missing id");
    }
}
*/

void handleScheduleSet() {
    PowerManager::getInstance().registerUserActivity();
    server.sendHeader("Access-Control-Allow-Origin", "*");

    if (server.hasArg("id")) {
        uint8_t id = server.arg("id").toInt();
        uint8_t startH = server.arg("start_h").toInt();
        uint8_t startM = server.arg("start_m").toInt();
        uint8_t endH = server.arg("end_h").toInt();
        uint8_t endM = server.arg("end_m").toInt();
        uint8_t days = server.hasArg("days") ? server.arg("days").toInt() : 0xFF;
        bool enable = (server.arg("enable").toInt() == 1);

        RelayManager::getInstance().setSchedule(id, startH, startM, endH, endM, days, enable);
        server.send(200, "text/plain", "Schedule Updated");
    } else {
        server.send(400, "text/plain", "Missing id");
    }
}

void handleTimeSync() {
    PowerManager::getInstance().registerUserActivity();
    server.sendHeader("Access-Control-Allow-Origin", "*");

    if (server.hasArg("epoch")) {
        uint32_t epoch = (uint32_t)strtoul(server.arg("epoch").c_str(), NULL, 10);
        int tzOffset = server.hasArg("tz_offset") ? server.arg("tz_offset").toInt() : 0;
        TimeManager::getInstance().syncPhoneTime(epoch, tzOffset);
        server.send(200, "text/plain", "Time Synced");
    } else {
        server.send(400, "text/plain", "Missing epoch");
    }
}

void handleManualTime() {
    PowerManager::getInstance().registerUserActivity();
    server.sendHeader("Access-Control-Allow-Origin", "*");

    if (server.hasArg("year") && server.hasArg("month") && server.hasArg("day")) {
        int y = server.arg("year").toInt();
        int m = server.arg("month").toInt();
        int d = server.arg("day").toInt();
        int h = server.arg("hour").toInt();
        int min = server.arg("min").toInt();
        int s = server.arg("sec").toInt();

        TimeManager::getInstance().setManualTime(y, m, d, h, min, s);
        server.send(200, "text/plain", "Manual Time Set");
    } else {
        server.send(400, "text/plain", "Incomplete parameters");
    }
}

void handlePowerConfig() {
    PowerManager::getInstance().registerUserActivity();
    server.sendHeader("Access-Control-Allow-Origin", "*");

    uint32_t sleepMin = server.hasArg("sleep_min") ? server.arg("sleep_min").toInt() : 5;
    uint32_t wakeMin = server.hasArg("wake_min") ? server.arg("wake_min").toInt() : 2;

    if (server.hasArg("enable")) {
        bool en = (server.arg("enable").toInt() == 1);
        PowerManager::getInstance().setLowPowerMode(en, sleepMin, wakeMin);
        server.send(200, "text/plain", "Power Config Updated");
    } else if (server.hasArg("stay_on")) {
        bool stayOn = (server.arg("stay_on").toInt() == 1);
        PowerManager::getInstance().setLowPowerMode(!stayOn, sleepMin, wakeMin);
        server.send(200, "text/plain", "Power Config Updated");
    } else {
        server.send(400, "text/plain", "Missing enable or stay_on parameter");
    }
}

void handleSettingsUpdate() {
    PowerManager::getInstance().registerUserActivity();
    server.sendHeader("Access-Control-Allow-Origin", "*");

    for (int ch = 1; ch <= NUM_RELAY_CHANNELS; ch++) {
        String prefix = "r" + String(ch) + "_";
        if (server.hasArg(prefix + "name")) {
            String name = server.arg(prefix + "name");
            float watts = server.hasArg(prefix + "watts") ? server.arg(prefix + "watts").toFloat() : 100.0f;
            bool actLow = server.hasArg(prefix + "actlow") ? (server.arg(prefix + "actlow").toInt() == 1) : true;
            RelayManager::getInstance().updateChannelConfig(ch, name.c_str(), actLow, watts);
        }
        if (server.hasArg(prefix + "pwron")) {
            uint8_t pwrOn = server.arg(prefix + "pwron").toInt();
            RelayManager::getInstance().setPowerOnBehavior(ch, pwrOn);
        }
    }

    server.send(200, "text/plain", "Settings Saved");
}

void handleAllOn() {
    PowerManager::getInstance().registerUserActivity();
    RelayManager::getInstance().setAllOn();
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", "ALL RELAYS ON");
}

void handleEmergencyAllOff() {
    PowerManager::getInstance().registerUserActivity();
    RelayManager::getInstance().setAllOff();
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", "ALL RELAYS OFF");
}

void handleTelegramConfig() {
    PowerManager::getInstance().registerUserActivity();
    server.sendHeader("Access-Control-Allow-Origin", "*");

    bool enabled = server.hasArg("enable") ? (server.arg("enable").toInt() == 1) : false;
    String token = server.hasArg("token") ? server.arg("token") : "";
    String chat = server.hasArg("chat_id") ? server.arg("chat_id") : "";
    String ssid = server.hasArg("sta_ssid") ? server.arg("sta_ssid") : "";
    String pass = server.hasArg("sta_pass") ? server.arg("sta_pass") : "";
    bool nTog = server.hasArg("n_tog") ? (server.arg("n_tog").toInt() == 1) : true;
    bool nTim = server.hasArg("n_tim") ? (server.arg("n_tim").toInt() == 1) : true;
    bool nSch = server.hasArg("n_sch") ? (server.arg("n_sch").toInt() == 1) : true;

    TelegramManager::getInstance().updateConfig(enabled, token.c_str(), chat.c_str(),
                                                ssid.c_str(), pass.c_str(),
                                                nTog, nTim, nSch);
    server.send(200, "text/plain", "Telegram Config Saved");
}

void handleTelegramTest() {
    PowerManager::getInstance().registerUserActivity();
    server.sendHeader("Access-Control-Allow-Origin", "*");

    bool ok = TelegramManager::getInstance().sendTestMessage();
    if (ok) {
        server.send(200, "text/plain", "Test Message Sent");
    } else {
        server.send(500, "text/plain", "Test message failed. Verify Wi-Fi Station & Bot credentials.");
    }
}

void handleCaptivePortalRedirect() {
    server.sendHeader("Location", String("http://192.168.4.1/"), true);
    server.send(302, "text/plain", "");
}

// =====================================================================
// OVER-THE-AIR (OTA) FIRMWARE UPDATE HANDLERS
// =====================================================================

// Port 80 OTA Upload Completion Handler
void handlePort80OtaUploadEnd() {
    PowerManager::getInstance().registerUserActivity();
    server.sendHeader("Connection", "close");
    server.sendHeader("Access-Control-Allow-Origin", "*");

    if (!Update.hasError()) {
        server.send(200, "text/plain", "OK");
        RelayManager::getInstance().logEvent("OTA: Flash Success (Port 80)");
        Serial.println("[OTA] Firmware Flashed Successfully via Port 80! Blinking LED 5 times...");
        blinkStatusLed(5, 150);
        isRebootPending = true;
        rebootAtMs = millis() + 1000;
    } else {
#if defined(ESP8266)
        server.send(500, "text/plain", "Update Failed: " + String(Update.getError()));
        RelayManager::getInstance().logEvent("OTA: Flash Failed (Port 80)");
        Serial.printf("[OTA] Flash Failed! Error: %d. Blinking LED 3 times...\n", Update.getError());
#else
        server.send(500, "text/plain", "Update Failed: " + String(Update.errorString()));
        RelayManager::getInstance().logEvent("OTA: Flash Failed (Port 80)");
        Serial.printf("[OTA] Flash Failed! Error: %s. Blinking LED 3 times...\n", Update.errorString());
#endif
        blinkStatusLed(3, 150);
    }
}

// Port 80 OTA Chunk Processor
void handlePort80OtaChunk() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
        Serial.printf("[OTA] Port 80 Update Start: %s\n", upload.filename.c_str());
        RelayManager::getInstance().logEvent("OTA: Upload Started (Port 80)");
#if defined(ESP8266)
        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
        if (!Update.begin(maxSketchSpace, U_FLASH)) {
            Update.printError(Serial);
        }
#else
        if (!Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH)) {
            Update.printError(Serial);
        }
#endif
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
            Update.printError(Serial);
        }
    } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) {
            Serial.printf("[OTA] Port 80 Update Size: %u bytes\n", upload.totalSize);
        } else {
            Update.printError(Serial);
        }
    } else if (upload.status == UPLOAD_FILE_ABORTED) {
        Update.end();
        Serial.println("[OTA] Port 80 Update Aborted");
    }
}

// Port 500 Security Toggle Handler
void handlePort500Toggle() {
    PowerManager::getInstance().registerUserActivity();
    server.sendHeader("Access-Control-Allow-Origin", "*");

    if (server.hasArg("enable")) {
        bool enable = (server.arg("enable").toInt() == 1);
        if (enable) {
            if (!isPort500Enabled) {
                server500.begin();
                isPort500Enabled = true;
                RelayManager::getInstance().logEvent("Port 500 OTA Server: ENABLED");
                Serial.println("[WebServer] Port 500 OTA Server Started");
            }
            server.send(200, "text/plain", "Port 500 Enabled");
        } else {
            if (isPort500Enabled) {
                server500.stop();
                isPort500Enabled = false;
                RelayManager::getInstance().logEvent("Port 500 OTA Server: DISABLED");
                Serial.println("[WebServer] Port 500 OTA Server Stopped");
            }
            server.send(200, "text/plain", "Port 500 Disabled");
        }
    } else {
        server.send(400, "text/plain", "Missing enable param");
    }
}

// Configure Dedicated Port 500 Web Server Endpoints
void setupServer500() {
    // Dedicated OTA Landing Portal
    server500.on("/", HTTP_GET, []() {
        PowerManager::getInstance().registerUserActivity();
        server500.send_P(200, "text/html", OTA_HTML);
    });

    // Dedicated OTA Upload Endpoint on Port 500
    server500.on("/update", HTTP_POST, []() {
        PowerManager::getInstance().registerUserActivity();
        server500.sendHeader("Connection", "close");
        server500.sendHeader("Access-Control-Allow-Origin", "*");

        if (!Update.hasError()) {
            server500.send(200, "text/plain", "OK");
            RelayManager::getInstance().logEvent("OTA: Flash Success (Port 500)");
            Serial.println("[OTA] Firmware Flashed Successfully via Port 500! Blinking LED 5 times...");
            blinkStatusLed(5, 150);
            isRebootPending = true;
            rebootAtMs = millis() + 1000;
        } else {
#if defined(ESP8266)
            server500.send(500, "text/plain", "Update Failed: " + String(Update.getError()));
            RelayManager::getInstance().logEvent("OTA: Flash Failed (Port 500)");
            Serial.printf("[OTA] Flash Failed! Error: %d. Blinking LED 3 times...\n", Update.getError());
#else
            server500.send(500, "text/plain", "Update Failed: " + String(Update.errorString()));
            RelayManager::getInstance().logEvent("OTA: Flash Failed (Port 500)");
            Serial.printf("[OTA] Flash Failed! Error: %s. Blinking LED 3 times...\n", Update.errorString());
#endif
            blinkStatusLed(3, 150);
        }
    }, []() {
        HTTPUpload& upload = server500.upload();
        if (upload.status == UPLOAD_FILE_START) {
            Serial.printf("[OTA] Port 500 Update Start: %s\n", upload.filename.c_str());
            RelayManager::getInstance().logEvent("OTA: Upload Started (Port 500)");
#if defined(ESP8266)
            uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
            if (!Update.begin(maxSketchSpace, U_FLASH)) {
                Update.printError(Serial);
            }
#else
            if (!Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH)) {
                Update.printError(Serial);
            }
#endif
        } else if (upload.status == UPLOAD_FILE_WRITE) {
            if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
                Update.printError(Serial);
            }
        } else if (upload.status == UPLOAD_FILE_END) {
            if (Update.end(true)) {
                Serial.printf("[OTA] Port 500 Update Size: %u bytes\n", upload.totalSize);
            } else {
                Update.printError(Serial);
            }
        } else if (upload.status == UPLOAD_FILE_ABORTED) {
            Update.end();
            Serial.println("[OTA] Port 500 Update Aborted");
        }
    });
}

// =====================================================================
// FREERTOS TASKS (ESP32 Multi-Core)
// =====================================================================
#if defined(ESP32)
// Core 1 Task: Dedicated Relay Control & Accurate Timer/Cycle/Schedule Engine
void vRelayTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(100); // Check every 100ms

    for (;;) {
        RelayManager::getInstance().processEngine();
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

// Core 0 Task: Low Power AP Sleep/Wake Cycle, Smooth Breathing LED & Telegram Background Engine
void vPowerTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(25); // Check every 25ms for smooth sleep LED breathing & responsive events

    for (;;) {
        PowerManager::getInstance().processEngine();
        TelegramManager::getInstance().processEngine();
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
#endif

// =====================================================================
// SETUP & MAIN LOOP
// =====================================================================

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n=======================================================");
    Serial.println("   ESP32 Real-Time Quad Relay & Telegram Control System");
    Serial.println("=======================================================");

    // 1. Initialize Subsystems
    TimeManager::getInstance().begin();
    RelayManager::getInstance().begin();
    PowerManager::getInstance().begin(DEFAULT_AP_SSID, DEFAULT_AP_PASS);
    TelegramManager::getInstance().begin();

    // 2. Start Wi-Fi Access Point
    WiFi.mode(WIFI_AP);
    IPAddress local_IP(AP_IP_OCTET_1, AP_IP_OCTET_2, AP_IP_OCTET_3, AP_IP_OCTET_4);
    IPAddress gateway(AP_IP_OCTET_1, AP_IP_OCTET_2, AP_IP_OCTET_3, AP_IP_OCTET_4);
    IPAddress subnet(255, 255, 255, 0);

    WiFi.softAPConfig(local_IP, gateway, subnet);
    WiFi.softAP(DEFAULT_AP_SSID, DEFAULT_AP_PASS, DEFAULT_AP_CHANNEL, 0, DEFAULT_AP_MAX_CONN);

    Serial.printf("[WiFi] AP Started: SSID: %s | Password: %s\n", DEFAULT_AP_SSID, DEFAULT_AP_PASS);
    Serial.print("[WiFi] IP Address: ");
    Serial.println(WiFi.softAPIP());

    // 3. Start Captive Portal DNS Server
    dnsServer.start(DNS_PORT, "*", local_IP);

    // 4. Configure Web Server Endpoints
    server.on("/", HTTP_GET, handleRoot);
    server.on("/api/status", HTTP_GET, handleStatus);
    server.on("/api/relay", HTTP_POST, handleRelayToggle);
    server.on("/api/all_on", HTTP_POST, handleAllOn);
    server.on("/api/all_off", HTTP_POST, handleEmergencyAllOff);
    server.on("/api/timer", HTTP_POST, handleTimerSet);
    server.on("/api/cycle", HTTP_POST, handleCycleSet);
    // server.on("/api/pulse", HTTP_POST, handlePulseSet);
    server.on("/api/schedule", HTTP_POST, handleScheduleSet);
    server.on("/api/time/sync", HTTP_POST, handleTimeSync);
    server.on("/api/time/manual", HTTP_POST, handleManualTime);
    server.on("/api/power", HTTP_POST, handlePowerConfig);
    server.on("/api/settings", HTTP_POST, handleSettingsUpdate);
    server.on("/api/telegram/config", HTTP_POST, handleTelegramConfig);
    server.on("/api/telegram/test", HTTP_POST, handleTelegramTest);

    // OTA Firmware Upload & Port 500 Security endpoints
    server.on("/api/ota/upload", HTTP_POST, handlePort80OtaUploadEnd, handlePort80OtaChunk);
    server.on("/api/ota/port500", HTTP_POST, handlePort500Toggle);

    // Initialize Port 500 endpoints (server starts when enabled from GUI)
    setupServer500();

    // Captive portal probes
    server.on("/generate_204", HTTP_GET, handleCaptivePortalRedirect);
    server.on("/hotspot-detect.html", HTTP_GET, handleCaptivePortalRedirect);
    server.on("/canonical.html", HTTP_GET, handleCaptivePortalRedirect);
    server.on("/connecttest.txt", HTTP_GET, handleCaptivePortalRedirect);
    server.on("/ncsi.txt", HTTP_GET, handleCaptivePortalRedirect);

    // Catch-all redirect
    server.onNotFound([]() {
        if (!server.hostHeader().equalsIgnoreCase(WiFi.softAPIP().toString())) {
            server.sendHeader("Location", String("http://192.168.4.1/"), true);
            server.send(302, "text/plain", "");
        } else {
            handleRoot();
        }
    });

    server.begin();
    Serial.println("[WebServer] HTTP Server started on Port 80");

#if defined(ESP32)
    // 5. Spawn FreeRTOS Tasks
    xTaskCreatePinnedToCore(
        vRelayTask,
        "RelayTask",
        RELAY_TASK_STACK,
        NULL,
        RELAY_TASK_PRIORITY,
        &xRelayTaskHandle,
        RELAY_TASK_CORE
    );

    xTaskCreatePinnedToCore(
        vPowerTask,
        "PowerTask",
        POWER_TASK_STACK,
        NULL,
        POWER_TASK_PRIORITY,
        &xPowerTaskHandle,
        POWER_TASK_CORE
    );

    Serial.println("[System] FreeRTOS Tasks running on Core 0 & Core 1. System Ready!");
#else
    Serial.println("[System] ESP8266 Cooperative Event Engine initialized. System Ready!");
#endif
}

void loop() {
#if defined(ESP8266)
    // ESP8266 Single-Core Cooperative Dispatcher
    static unsigned long lastRelayTick = 0;
    static unsigned long lastPowerTick = 0;
    unsigned long now = millis();

    // Dedicated Relay Execution engine (100ms precision)
    if (now - lastRelayTick >= 100) {
        lastRelayTick = now;
        RelayManager::getInstance().processEngine();
    }

    // Power, LED breathing & Telegram engine (25ms interval)
    if (now - lastPowerTick >= 25) {
        lastPowerTick = now;
        PowerManager::getInstance().processEngine();
        TelegramManager::getInstance().processEngine();
    }
#endif

    // Process DNS queries & HTTP clients if AP is active
    if (!PowerManager::getInstance().isApSleeping()) {
        dnsServer.processNextRequest();
        server.handleClient();
        if (isPort500Enabled) {
            server500.handleClient();
        }
    }

    // Process delayed system reboot after successful OTA update
    if (isRebootPending && millis() >= rebootAtMs) {
        Serial.println("[System] Rebooting ESP into new firmware...");
        delay(100);
        ESP.restart();
    }

    delay(2); // Short yield for Wi-Fi stack & scheduler
}
