#include "PowerManager.h"
#include "RelayManager.h"
#include "TelegramManager.h"
#include <math.h>

PowerManager::PowerManager() : 
    _lastActivityTick(0), 
    _stateChangeTick(0), 
    _processTicks(0),
    _ledPhase(0.0f),
    _ledBrightness(0),
    _ledDirection(4) {
    _powerMutex = xSemaphoreCreateMutex();

    _config.enabled = false;
    _config.permanentStayOn = true;       // Default: Permanent Always-On Mode
    _config.isApSleeping = false;
    _config.sleepIntervalMin = 5;         // 5 minutes off
    _config.wakeWindowMin = 2;            // 2 minutes on
    _config.inactivityTimeoutMin = 2;     // 2 minutes no-activity timeout
    _config.nextStateChangeEpoch = 0;

    strncpy(_apSsid, DEFAULT_AP_SSID, sizeof(_apSsid) - 1);
    strncpy(_apPass, DEFAULT_AP_PASS, sizeof(_apPass) - 1);
}

PowerManager::~PowerManager() {
    if (_powerMutex != NULL) {
        vSemaphoreDelete(_powerMutex);
    }
}

PowerManager& PowerManager::getInstance() {
    static PowerManager instance;
    return instance;
}

void PowerManager::begin(const char* apSsid, const char* apPass) {
    pinMode(STATUS_LED_PIN, OUTPUT);
    digitalWrite(STATUS_LED_PIN, LOW);

    if (apSsid && strlen(apSsid) > 0) {
        strncpy(_apSsid, apSsid, sizeof(_apSsid) - 1);
    }
    if (apPass) {
        strncpy(_apPass, apPass, sizeof(_apPass) - 1);
    }

    loadFromPreferences();

    _lastActivityTick = millis();
    _stateChangeTick = millis();
    _processTicks = 0;
    _ledPhase = 0.0f;
    _ledBrightness = 0;
    _ledDirection = 4;
}

void PowerManager::setLowPowerMode(bool enable, uint32_t sleepMin, uint32_t wakeMin) {
    if (xSemaphoreTake(_powerMutex, portMAX_DELAY) == pdTRUE) {
        _config.enabled = enable;
        _config.permanentStayOn = !enable; // Toggling low power mode explicitly updates permanent stay on

        if (sleepMin >= 1) _config.sleepIntervalMin = sleepMin;
        if (wakeMin >= 1) _config.wakeWindowMin = wakeMin;

        if (!enable && _config.isApSleeping) {
            wakeUpAP();
        } else if (enable) {
            _lastActivityTick = millis();
            _stateChangeTick = millis();
        }

        saveToPreferences();
        
        Serial.printf("[PowerManager] Low Power Mode: %s (Sleep: %u min, Wake: %u min, PermanentStayOn: %d)\n", 
                      enable ? "ENABLED" : "DISABLED", _config.sleepIntervalMin, _config.wakeWindowMin, _config.permanentStayOn);
        xSemaphoreGive(_powerMutex);
    }
}

void PowerManager::setPermanentStayOn(bool permanentStayOn) {
    if (xSemaphoreTake(_powerMutex, portMAX_DELAY) == pdTRUE) {
        _config.permanentStayOn = permanentStayOn;
        _config.enabled = !permanentStayOn; // When stay-on is disabled, low power mode is ENABLED

        if (permanentStayOn && _config.isApSleeping) {
            wakeUpAP();
        } else if (!permanentStayOn) {
            _lastActivityTick = millis();
            _stateChangeTick = millis();
        }
        saveToPreferences();

        Serial.printf("[PowerManager] Permanent Stay-On Mode: %s (Low Power Mode: %s)\n", 
                      permanentStayOn ? "ENABLED (Sleep Disabled)" : "DISABLED (Auto-Sleep Enabled)",
                      _config.enabled ? "ENABLED" : "DISABLED");
        xSemaphoreGive(_powerMutex);
    }
}

void PowerManager::registerUserActivity() {
    if (xSemaphoreTake(_powerMutex, pdMS_TO_TICKS(20)) == pdTRUE) {
        _lastActivityTick = millis();
        xSemaphoreGive(_powerMutex);
    }
}

bool PowerManager::isLowPowerEnabled() {
    bool en = false;
    if (xSemaphoreTake(_powerMutex, pdMS_TO_TICKS(20)) == pdTRUE) {
        en = _config.enabled && !_config.permanentStayOn;
        xSemaphoreGive(_powerMutex);
    }
    return en;
}

bool PowerManager::isPermanentStayOn() {
    bool p = true;
    if (xSemaphoreTake(_powerMutex, pdMS_TO_TICKS(20)) == pdTRUE) {
        p = _config.permanentStayOn;
        xSemaphoreGive(_powerMutex);
    }
    return p;
}

bool PowerManager::isApSleeping() {
    bool sleeping = false;
    if (xSemaphoreTake(_powerMutex, pdMS_TO_TICKS(20)) == pdTRUE) {
        sleeping = _config.isApSleeping;
        xSemaphoreGive(_powerMutex);
    }
    return sleeping;
}

LowPowerConfig PowerManager::getConfig() {
    LowPowerConfig copy;
    if (xSemaphoreTake(_powerMutex, pdMS_TO_TICKS(20)) == pdTRUE) {
        copy = _config;
        xSemaphoreGive(_powerMutex);
    }
    return copy;
}

uint32_t PowerManager::getSecondsUntilNextState() {
    uint32_t remaining = 0;
    if (xSemaphoreTake(_powerMutex, pdMS_TO_TICKS(20)) == pdTRUE) {
        if (_config.permanentStayOn || !_config.enabled) {
            remaining = 0;
        } else {
            uint32_t now = millis();
            if (_config.isApSleeping) {
                uint32_t elapsed = now - _stateChangeTick;
                uint32_t targetMs = _config.sleepIntervalMin * 60 * 1000;
                remaining = (elapsed < targetMs) ? ((targetMs - elapsed) / 1000) : 0;
            } else {
                uint32_t actElapsed = now - _lastActivityTick;
                uint32_t targetMs = _config.wakeWindowMin * 60 * 1000;
                remaining = (actElapsed < targetMs) ? ((targetMs - actElapsed) / 1000) : 0;
            }
        }
        xSemaphoreGive(_powerMutex);
    }
    return remaining;
}

uint32_t PowerManager::getTickCount() {
    return _processTicks;
}

void PowerManager::wakeUpImmediately() {
    if (xSemaphoreTake(_powerMutex, portMAX_DELAY) == pdTRUE) {
        _lastActivityTick = millis();
        _stateChangeTick = millis();
        if (_config.isApSleeping) {
            wakeUpAP();
        }
        xSemaphoreGive(_powerMutex);
    }
}

void PowerManager::updateSleepLed() {
    // CRITICAL FIX: Do NOT use analogWrite/PWM on ESP8266 in sleep mode!
    // Rapid PWM modulation on STATUS_LED_PIN (GPIO 2 / D4) induces switching and clicks on relays
    // attached to or adjacent to GPIO 2 on 4-channel relay shields.
    // Keep STATUS_LED_PIN safely non-pulsing.
}

void PowerManager::wakeUpAP() {
    Serial.println("[PowerManager] Waking up Wi-Fi AP...");
    TelegramConfig tg = TelegramManager::getInstance().getConfig();
    if (tg.enabled && strlen(tg.staSsid) > 0) {
        WiFi.mode(WIFI_AP_STA);
    } else {
        WiFi.mode(WIFI_AP);
    }
    
    IPAddress local_IP(AP_IP_OCTET_1, AP_IP_OCTET_2, AP_IP_OCTET_3, AP_IP_OCTET_4);
    IPAddress gateway(AP_IP_OCTET_1, AP_IP_OCTET_2, AP_IP_OCTET_3, AP_IP_OCTET_4);
    IPAddress subnet(255, 255, 255, 0);
    
    WiFi.softAPConfig(local_IP, gateway, subnet);
    WiFi.softAP(_apSsid, strlen(_apPass) > 0 ? _apPass : NULL, DEFAULT_AP_CHANNEL, 0, DEFAULT_AP_MAX_CONN);

    _config.isApSleeping = false;
    _lastActivityTick = millis();
    _stateChangeTick = millis();
    
    Serial.printf("[PowerManager] AP Awake: %s (IP: 192.168.4.1)\n", _apSsid);
}

void PowerManager::putAPToSleep() {
    if (_config.permanentStayOn) {
        return; // Inhibit sleep if permanent stay on is active
    }

    // CRITICAL: Never enter sleep mode while any relay is ON, or countdown timer is counting down, or cycle is running!
    if (RelayManager::getInstance().isAnyRelayOn() || 
        RelayManager::getInstance().isAnyTimerActive() || 
        RelayManager::getInstance().isAnyCycleActive()) {
        _lastActivityTick = millis();
        Serial.println("[PowerManager] Inhibit sleep: Relay or automation is currently active.");
        return;
    }

    Serial.println("[PowerManager] Disconnecting Wi-Fi AP channel (Power Saver Mode)... Backend relay operations continue uninterrupted.");
    WiFi.softAPdisconnect(true);
    
    // If Telegram is configured, keep Station interface active; otherwise shut off radio
    TelegramConfig tg = TelegramManager::getInstance().getConfig();
    if (tg.enabled && strlen(tg.staSsid) > 0) {
        WiFi.mode(WIFI_STA);
    } else {
        WiFi.mode(WIFI_OFF);
    }

    _config.isApSleeping = true;
    _stateChangeTick = millis();
    Serial.printf("[PowerManager] Wi-Fi AP disconnected (Sleeping). Next AP wake window in %u minutes.\n", _config.sleepIntervalMin);
}

void PowerManager::processEngine() {
    _processTicks++;
    if (xSemaphoreTake(_powerMutex, pdMS_TO_TICKS(30)) == pdTRUE) {
        updateSleepLed();

        // If permanent stay on is enabled, ensure AP remains awake
        if (_config.permanentStayOn) {
            if (_config.isApSleeping) {
                wakeUpAP();
            }
        } else if (_config.enabled) {
            uint32_t now = millis();

            if (_config.isApSleeping) {
                // If any relay is ON or countdown timer/cycle is active, immediately wake up ESP!
                if (RelayManager::getInstance().isAnyRelayOn() || 
                    RelayManager::getInstance().isAnyTimerActive() || 
                    RelayManager::getInstance().isAnyCycleActive()) {
                    Serial.println("[PowerManager] Relay/Timer active during sleep -> Raising/Waking up ESP immediately!");
                    wakeUpAP();
                } else {
                    // Check regular sleep interval
                    uint32_t sleepIntervalMs = _config.sleepIntervalMin * 60 * 1000;
                    if (now - _stateChangeTick >= sleepIntervalMs) {
                        wakeUpAP();
                    }
                }
            } else {
                // If awake, check if relay or automation is active -> Keep refreshing activity tick so AP stays awake
                if (RelayManager::getInstance().isAnyRelayOn() || 
                    RelayManager::getInstance().isAnyTimerActive() || 
                    RelayManager::getInstance().isAnyCycleActive()) {
                    _lastActivityTick = now;
                } else {
                    uint32_t activeTimeoutMs = _config.wakeWindowMin * 60 * 1000;
                    if (now - _lastActivityTick >= activeTimeoutMs) {
                        putAPToSleep();
                    }
                }
            }
        }
        xSemaphoreGive(_powerMutex);
    }
}

void PowerManager::loadFromPreferences() {
    if (_prefs.begin("pwr_cfg", true)) {
        _config.permanentStayOn = _prefs.getBool("lp_stay_on", true); // Default: Stay on permanently
        _config.enabled = _prefs.getBool("lp_en", !_config.permanentStayOn);
        // Ensure consistency between permanentStayOn and enabled
        if (_config.permanentStayOn) {
            _config.enabled = false;
        } else {
            _config.enabled = true;
        }
        _config.sleepIntervalMin = _prefs.getUInt("lp_sleep", 5);
        _config.wakeWindowMin = _prefs.getUInt("lp_wake", 2);
        if (_config.sleepIntervalMin < 1) _config.sleepIntervalMin = 5;
        if (_config.wakeWindowMin < 1) _config.wakeWindowMin = 2;
        _prefs.end();
        Serial.printf("[PowerManager] Preferences loaded: PermanentStayOn=%d, LowPowerEn=%d, Sleep=%u min, Wake=%u min\n", 
                      _config.permanentStayOn, _config.enabled, _config.sleepIntervalMin, _config.wakeWindowMin);
    }
}

void PowerManager::saveToPreferences() {
    if (_prefs.begin("pwr_cfg", false)) {
        _prefs.putBool("lp_stay_on", _config.permanentStayOn);
        _prefs.putBool("lp_en", _config.enabled);
        _prefs.putUInt("lp_sleep", _config.sleepIntervalMin);
        _prefs.putUInt("lp_wake", _config.wakeWindowMin);
        _prefs.end();
        Serial.printf("[PowerManager] Preferences saved: PermanentStayOn=%d, LowPowerEn=%d, Sleep=%u min, Wake=%u min\n",
                      _config.permanentStayOn, _config.enabled, _config.sleepIntervalMin, _config.wakeWindowMin);
    }
}
