#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include "Config.h"
#include "TimeManager.h"
#include "RelayManager.h"
#include "PowerManager.h"
#include "WebPages.h"

// Web Server and Captive Portal DNS Server
WebServer server(WEB_SERVER_PORT);
DNSServer dnsServer;

// FreeRTOS Task Handles
TaskHandle_t xRelayTaskHandle = NULL;
TaskHandle_t xPowerTaskHandle = NULL;

// Helper to read internal ESP32 chip temperature
#ifdef __cplusplus
extern "C" {
#endif
uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif

float getESP32TemperatureC() {
    #if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 3)
        return temperatureRead();
    #else
        // Fallback for ESP32 Core v2.x
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
    PowerManager::getInstance().registerUserActivity();

    RelayChannel r1 = RelayManager::getInstance().getChannel(1);
    RelayChannel r2 = RelayManager::getInstance().getChannel(2);
    LowPowerConfig pwr = PowerManager::getInstance().getConfig();

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

    String json = "{";

    // Time object
    json += "\"time\":{";
    json += "\"clock\":\"" + String(timeBuf) + "\",";
    json += "\"date\":\"" + String(dateBuf) + "\",";
    json += "\"epoch\":" + String(TimeManager::getInstance().getEpochTime()) + ",";
    json += "\"synced\":" + String(TimeManager::getInstance().isTimeSynchronized() ? "true" : "false");
    json += "},";

    // Relay 1 object
    json += "\"r1\":{";
    json += "\"name\":\"" + String(r1.name) + "\",";
    json += "\"state\":" + String(r1.state ? "true" : "false") + ",";
    json += "\"act_low\":" + String(r1.activeLow ? "true" : "false") + ",";
    json += "\"watts\":" + String(r1.ratedWatts, 1) + ",";
    json += "\"total_sec\":" + String(r1.totalOnSeconds) + ",";
    json += "\"pwr_on\":" + String(r1.powerOnState) + ",";
    json += "\"timer\":{";
    json += "\"active\":" + String(r1.timer.isCountingDown ? "true" : "false") + ",";
    json += "\"in_delay\":" + String(r1.timer.inDelayPhase ? "true" : "false") + ",";
    json += "\"delay_sec\":" + String(r1.timer.startDelaySec) + ",";
    json += "\"dur_sec\":" + String(r1.timer.durationSec) + ",";
    json += "\"remaining\":" + String(r1.timer.remainingSec);
    json += "},";
    json += "\"cycle\":{";
    json += "\"active\":" + String(r1.cycle.enabled ? "true" : "false") + ",";
    json += "\"in_on\":" + String(r1.cycle.inOnPhase ? "true" : "false") + ",";
    json += "\"on_sec\":" + String(r1.cycle.onSec) + ",";
    json += "\"off_sec\":" + String(r1.cycle.offSec) + ",";
    json += "\"total_cycles\":" + String(r1.cycle.totalCycles) + ",";
    json += "\"current_cycle\":" + String(r1.cycle.currentCycle) + ",";
    json += "\"remaining\":" + String(r1.cycle.remainingSec);
    json += "},";
    json += "\"schedule\":{";
    json += "\"enabled\":" + String(r1.schedule.enabled ? "true" : "false") + ",";
    json += "\"start_h\":" + String(r1.schedule.startHour) + ",";
    json += "\"start_m\":" + String(r1.schedule.startMinute) + ",";
    json += "\"end_h\":" + String(r1.schedule.endHour) + ",";
    json += "\"end_m\":" + String(r1.schedule.endMinute) + ",";
    json += "\"days\":" + String(r1.schedule.daysActive);
    json += "}";
    json += "},";

    // Relay 2 object
    json += "\"r2\":{";
    json += "\"name\":\"" + String(r2.name) + "\",";
    json += "\"state\":" + String(r2.state ? "true" : "false") + ",";
    json += "\"act_low\":" + String(r2.activeLow ? "true" : "false") + ",";
    json += "\"watts\":" + String(r2.ratedWatts, 1) + ",";
    json += "\"total_sec\":" + String(r2.totalOnSeconds) + ",";
    json += "\"pwr_on\":" + String(r2.powerOnState) + ",";
    json += "\"timer\":{";
    json += "\"active\":" + String(r2.timer.isCountingDown ? "true" : "false") + ",";
    json += "\"in_delay\":" + String(r2.timer.inDelayPhase ? "true" : "false") + ",";
    json += "\"delay_sec\":" + String(r2.timer.startDelaySec) + ",";
    json += "\"dur_sec\":" + String(r2.timer.durationSec) + ",";
    json += "\"remaining\":" + String(r2.timer.remainingSec);
    json += "},";
    json += "\"cycle\":{";
    json += "\"active\":" + String(r2.cycle.enabled ? "true" : "false") + ",";
    json += "\"in_on\":" + String(r2.cycle.inOnPhase ? "true" : "false") + ",";
    json += "\"on_sec\":" + String(r2.cycle.onSec) + ",";
    json += "\"off_sec\":" + String(r2.cycle.offSec) + ",";
    json += "\"total_cycles\":" + String(r2.cycle.totalCycles) + ",";
    json += "\"current_cycle\":" + String(r2.cycle.currentCycle) + ",";
    json += "\"remaining\":" + String(r2.cycle.remainingSec);
    json += "},";
    json += "\"schedule\":{";
    json += "\"enabled\":" + String(r2.schedule.enabled ? "true" : "false") + ",";
    json += "\"start_h\":" + String(r2.schedule.startHour) + ",";
    json += "\"start_m\":" + String(r2.schedule.startMinute) + ",";
    json += "\"end_h\":" + String(r2.schedule.endHour) + ",";
    json += "\"end_m\":" + String(r2.schedule.endMinute) + ",";
    json += "\"days\":" + String(r2.schedule.daysActive);
    json += "}";
    json += "},";

    // Power saver object
    json += "\"power\":{";
    json += "\"enabled\":" + String(pwr.enabled ? "true" : "false") + ",";
    json += "\"stay_on\":" + String(pwr.permanentStayOn ? "true" : "false") + ",";
    json += "\"sleeping\":" + String(pwr.isApSleeping ? "true" : "false") + ",";
    json += "\"sleep_min\":" + String(pwr.sleepIntervalMin) + ",";
    json += "\"wake_min\":" + String(pwr.wakeWindowMin) + ",";
    json += "\"next_state_sec\":" + String(PowerManager::getInstance().getSecondsUntilNextState());
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
    json += "]";

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

    if (server.hasArg("stay_on")) {
        bool stayOn = (server.arg("stay_on").toInt() == 1);
        PowerManager::getInstance().setPermanentStayOn(stayOn);
        server.send(200, "text/plain", "Permanent Stay-On Config Updated");
        return;
    }

    if (server.hasArg("enable")) {
        bool en = (server.arg("enable").toInt() == 1);
        uint32_t sleepMin = server.hasArg("sleep_min") ? server.arg("sleep_min").toInt() : 5;
        uint32_t wakeMin = server.hasArg("wake_min") ? server.arg("wake_min").toInt() : 2;

        PowerManager::getInstance().setLowPowerMode(en, sleepMin, wakeMin);
        server.send(200, "text/plain", "Power Config Updated");
    } else {
        server.send(400, "text/plain", "Missing enable/stay_on parameter");
    }
}

void handleSettingsUpdate() {
    PowerManager::getInstance().registerUserActivity();
    server.sendHeader("Access-Control-Allow-Origin", "*");

    if (server.hasArg("r1_name")) {
        String r1Name = server.arg("r1_name");
        float r1Watts = server.arg("r1_watts").toFloat();
        bool r1ActLow = (server.arg("r1_actlow").toInt() == 1);
        RelayManager::getInstance().updateChannelConfig(1, r1Name.c_str(), r1ActLow, r1Watts);
    }
    if (server.hasArg("r1_pwron")) {
        uint8_t pwrOn = server.arg("r1_pwron").toInt();
        RelayManager::getInstance().setPowerOnBehavior(1, pwrOn);
    }

    if (server.hasArg("r2_name")) {
        String r2Name = server.arg("r2_name");
        float r2Watts = server.arg("r2_watts").toFloat();
        bool r2ActLow = (server.arg("r2_actlow").toInt() == 1);
        RelayManager::getInstance().updateChannelConfig(2, r2Name.c_str(), r2ActLow, r2Watts);
    }
    if (server.hasArg("r2_pwron")) {
        uint8_t pwrOn = server.arg("r2_pwron").toInt();
        RelayManager::getInstance().setPowerOnBehavior(2, pwrOn);
    }

    server.send(200, "text/plain", "Settings Saved");
}

void handleEmergencyAllOff() {
    RelayManager::getInstance().setAllOff();
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", "ALL RELAYS OFF");
}

void handleCaptivePortalRedirect() {
    server.sendHeader("Location", String("http://192.168.4.1/"), true);
    server.send(302, "text/plain", "");
}

// =====================================================================
// FREERTOS TASKS
// =====================================================================

// Core 1 Task: Dedicated Relay Control & Accurate Timer/Cycle/Schedule Engine
void vRelayTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(100); // Check every 100ms

    for (;;) {
        RelayManager::getInstance().processEngine();
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

// Core 0 Task: Low Power AP Sleep/Wake Cycle & Permanent Stay-On Manager
void vPowerTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(500); // Check every 500ms

    for (;;) {
        PowerManager::getInstance().processEngine();
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

// =====================================================================
// SETUP & MAIN LOOP
// =====================================================================

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n=======================================================");
    Serial.println("   ESP32 Real-Time Dual Relay Controller & AP Server   ");
    Serial.println("=======================================================");

    // 1. Initialize Subsystems
    TimeManager::getInstance().begin();
    RelayManager::getInstance().begin();
    PowerManager::getInstance().begin(DEFAULT_AP_SSID, DEFAULT_AP_PASS);

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
    server.on("/api/timer", HTTP_POST, handleTimerSet);
    server.on("/api/cycle", HTTP_POST, handleCycleSet);
    // server.on("/api/pulse", HTTP_POST, handlePulseSet);
    server.on("/api/schedule", HTTP_POST, handleScheduleSet);
    server.on("/api/time/sync", HTTP_POST, handleTimeSync);
    server.on("/api/time/manual", HTTP_POST, handleManualTime);
    server.on("/api/power", HTTP_POST, handlePowerConfig);
    server.on("/api/settings", HTTP_POST, handleSettingsUpdate);
    server.on("/api/all_off", HTTP_POST, handleEmergencyAllOff);

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
}

void loop() {
    // Process DNS queries & HTTP clients if AP is active
    if (!PowerManager::getInstance().isApSleeping()) {
        dnsServer.processNextRequest();
        server.handleClient();
    }
    delay(2); // Short yield for FreeRTOS scheduler
}
