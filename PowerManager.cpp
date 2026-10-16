#include "PowerManager.h"
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

void PowerManager::updateSleepLed() {
    if (_config.isApSleeping) {
        // Silky smooth sinusoidal breathing / fading blue light effect in sleep mode (smooth high to low to high)
        _ledPhase += 0.052f; // ~120 steps across 25ms ticks -> ~3.0s smooth breathing cycle
        if (_ledPhase >= 2.0f * (float)PI) {
            _ledPhase -= 2.0f * (float)PI;
        }

        // Sine wave mapped 0.0 to 1.0
        float sineVal = (sinf(_ledPhase) + 1.0f) * 0.5f;

        // Quadratic gamma perception curve (smooth gradual transition near dark and bright)
        float gammaVal = sineVal * sineVal;

        // Brightness smoothly transitions between gentle low (2) and bright high (255)
        _ledBrightness = (int16_t)(gammaVal * 253.0f + 2.0f);
        if (_ledBrightness > 255) _ledBrightness = 255;
        if (_ledBrightness < 0) _ledBrightness = 0;

        analogWrite(STATUS_LED_PIN, (uint8_t)_ledBrightness);
    } else {
        _ledPhase = 0.0f;
        _ledBrightness = 0;
        analogWrite(STATUS_LED_PIN, 0);
        digitalWrite(STATUS_LED_PIN, LOW);
    }
}

void PowerManager::wakeUpAP() {
    Serial.println("[PowerManager] Waking up Wi-Fi AP...");
    WiFi.mode(WIFI_AP);
    
    IPAddress local_IP(AP_IP_OCTET_1, AP_IP_OCTET_2, AP_IP_OCTET_3, AP_IP_OCTET_4);
    IPAddress gateway(AP_IP_OCTET_1, AP_IP_OCTET_2, AP_IP_OCTET_3, AP_IP_OCTET_4);
    IPAddress subnet(255, 255, 255, 0);
    
    WiFi.softAPConfig(local_IP, gateway, subnet);
    WiFi.softAP(_apSsid, strlen(_apPass) > 0 ? _apPass : NULL, DEFAULT_AP_CHANNEL, 0, DEFAULT_AP_MAX_CONN);

    _config.isApSleeping = false;
    _lastActivityTick = millis();
    _stateChangeTick = millis();
    
    // Turn OFF sleep breathing LED when awake
    _ledPhase = 0.0f;
    _ledBrightness = 0;
    analogWrite(STATUS_LED_PIN, 0);
    digitalWrite(STATUS_LED_PIN, LOW);
    
    Serial.printf("[PowerManager] AP Awake: %s (IP: 192.168.4.1)\n", _apSsid);
}

void PowerManager::putAPToSleep() {
    if (_config.permanentStayOn) {
        return; // Inhibit sleep if permanent stay on is active
    }
    Serial.println("[PowerManager] Putting Wi-Fi AP to sleep (Power Saving Mode)...");
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);

    _config.isApSleeping = true;
    _stateChangeTick = millis();
    _ledPhase = 0.0f;
    _ledBrightness = 0;
    Serial.printf("[PowerManager] Wi-Fi AP sleeping. Blue LED breathing smoothly. Next wake in %u minutes.\n", _config.sleepIntervalMin);
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

            if (!_config.isApSleeping) {
                // Check inactivity timeout
                uint32_t activeTimeoutMs = _config.wakeWindowMin * 60 * 1000;
                if (now - _lastActivityTick >= activeTimeoutMs) {
                    // Inactivity timeout expired -> Enter sleep mode with smooth breathing LED
                    putAPToSleep();
                }
            } else {
                // AP is sleeping, check if sleep interval has elapsed
                uint32_t sleepIntervalMs = _config.sleepIntervalMin * 60 * 1000;
                if (now - _stateChangeTick >= sleepIntervalMs) {
                    // Sleep period finished -> Wake up AP for discovery/connection window
                    wakeUpAP();
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
