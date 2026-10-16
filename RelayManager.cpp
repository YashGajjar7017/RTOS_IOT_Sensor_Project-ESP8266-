#include "RelayManager.h"
#include "TelegramManager.h"

RelayManager::RelayManager() : 
    _logHead(0), 
    _logCount(0), 
    _lastProcessTick(0), 
    _lastRuntimeSaveTick(0), 
    _lastOneSecondTick(0),
    _processTicks(0) {
    _relayMutex = xSemaphoreCreateMutex();

    for (int i = 0; i < NUM_RELAY_CHANNELS; i++) {
        _pulseActive[i] = false;
        _pulseEndTick[i] = 0;
    }

    // Default Channel 1
    _channels[0].id = 1;
    _channels[0].pin = RELAY_1_PIN;
#if defined(ESP8266)
    strncpy(_channels[0].name, "Relay 1 (D1)", sizeof(_channels[0].name) - 1);
#else
    strncpy(_channels[0].name, "Relay 1 (D5)", sizeof(_channels[0].name) - 1);
#endif
    _channels[0].name[sizeof(_channels[0].name) - 1] = '\0';
    _channels[0].state = false;
    _channels[0].activeLow = true;   // Standard relay module active low
    _channels[0].ratedWatts = 100.0f; // 100W default load
    _channels[0].totalOnSeconds = 0;
    _channels[0].sessionOnSeconds = 0;
    _channels[0].powerOnState = POWERON_OFF;
    _channels[0].timer = { false, 0, 0, 0, 0, false, false };
    _channels[0].cycle = { false, 60, 60, 0, 0, 0, false };
    _channels[0].schedule = { false, 8, 0, 0, 18, 0, 0, 0xFF };

    // Default Channel 2
    _channels[1].id = 2;
    _channels[1].pin = RELAY_2_PIN;
#if defined(ESP8266)
    strncpy(_channels[1].name, "Relay 2 (D2)", sizeof(_channels[1].name) - 1);
#else
    strncpy(_channels[1].name, "Relay 2 (D18)", sizeof(_channels[1].name) - 1);
#endif
    _channels[1].name[sizeof(_channels[1].name) - 1] = '\0';
    _channels[1].state = false;
    _channels[1].activeLow = true;
    _channels[1].ratedWatts = 100.0f;
    _channels[1].totalOnSeconds = 0;
    _channels[1].sessionOnSeconds = 0;
    _channels[1].powerOnState = POWERON_OFF;
    _channels[1].timer = { false, 0, 0, 0, 0, false, false };
    _channels[1].cycle = { false, 60, 60, 0, 0, 0, false };
    _channels[1].schedule = { false, 18, 0, 0, 22, 0, 0, 0xFF };

    // Default Channel 3
    _channels[2].id = 3;
    _channels[2].pin = RELAY_3_PIN;
#if defined(ESP8266)
    strncpy(_channels[2].name, "Relay 3 (D5)", sizeof(_channels[2].name) - 1);
#else
    strncpy(_channels[2].name, "Relay 3 (D19)", sizeof(_channels[2].name) - 1);
#endif
    _channels[2].name[sizeof(_channels[2].name) - 1] = '\0';
    _channels[2].state = false;
    _channels[2].activeLow = true;
    _channels[2].ratedWatts = 100.0f;
    _channels[2].totalOnSeconds = 0;
    _channels[2].sessionOnSeconds = 0;
    _channels[2].powerOnState = POWERON_OFF;
    _channels[2].timer = { false, 0, 0, 0, 0, false, false };
    _channels[2].cycle = { false, 60, 60, 0, 0, 0, false };
    _channels[2].schedule = { false, 8, 0, 0, 18, 0, 0, 0xFF };

    // Default Channel 4
    _channels[3].id = 4;
    _channels[3].pin = RELAY_4_PIN;
#if defined(ESP8266)
    strncpy(_channels[3].name, "Relay 4 (D6)", sizeof(_channels[3].name) - 1);
#else
    strncpy(_channels[3].name, "Relay 4 (D21)", sizeof(_channels[3].name) - 1);
#endif
    _channels[3].name[sizeof(_channels[3].name) - 1] = '\0';
    _channels[3].state = false;
    _channels[3].activeLow = true;
    _channels[3].ratedWatts = 100.0f;
    _channels[3].totalOnSeconds = 0;
    _channels[3].sessionOnSeconds = 0;
    _channels[3].powerOnState = POWERON_OFF;
    _channels[3].timer = { false, 0, 0, 0, 0, false, false };
    _channels[3].cycle = { false, 60, 60, 0, 0, 0, false };
    _channels[3].schedule = { false, 18, 0, 0, 22, 0, 0, 0xFF };
}

RelayManager::~RelayManager() {
    if (_relayMutex != NULL) {
        vSemaphoreDelete(_relayMutex);
    }
}

RelayManager& RelayManager::getInstance() {
    static RelayManager instance;
    return instance;
}

void RelayManager::begin() {
    // Configure GPIOs for all channels
    for (int i = 0; i < NUM_RELAY_CHANNELS; i++) {
        pinMode(_channels[i].pin, OUTPUT);
    }
    pinMode(STATUS_LED_PIN, OUTPUT);
    digitalWrite(STATUS_LED_PIN, LOW);

    loadFromPreferences();

    // Set initial physical state based on Power-On preference
    for (int i = 0; i < NUM_RELAY_CHANNELS; i++) {
        if (_channels[i].powerOnState == POWERON_ON) {
            _channels[i].state = true;
        } else if (_channels[i].powerOnState == POWERON_OFF) {
            _channels[i].state = false;
        } // if RESTORE_LAST, state was already restored in loadFromPreferences()
        
        applyPhysicalPin(i + 1);
    }

    _lastProcessTick = millis();
    _lastRuntimeSaveTick = millis();
    _lastOneSecondTick = millis();
    _processTicks = 0;

    logEvent("System Booted & RTOS Initialized");
    Serial.printf("[RelayManager] Initialized with 4 channels on GPIOs: D5(%d), D18(%d), D19(%d), D21(%d)\n", 
                  _channels[0].pin, _channels[1].pin, _channels[2].pin, _channels[3].pin);
}

void RelayManager::logEvent(const char* msg) {
    if (!msg) return;
    
    char timeBuf[12] = "00:00:00";
    if (TimeManager::getInstance().isTimeSynchronized()) {
        TimeManager::getInstance().getFormattedTime(timeBuf, sizeof(timeBuf));
    } else {
        uint32_t s = millis() / 1000;
        uint32_t sec = s % 60;
        uint32_t min = (s / 60) % 60;
        uint32_t hr = (s / 3600);
        snprintf(timeBuf, sizeof(timeBuf), "+%02u:%02u:%02u", hr, min, sec);
    }

    strncpy(_logs[_logHead].timestamp, timeBuf, sizeof(_logs[_logHead].timestamp) - 1);
    _logs[_logHead].timestamp[sizeof(_logs[_logHead].timestamp) - 1] = '\0';

    strncpy(_logs[_logHead].message, msg, sizeof(_logs[_logHead].message) - 1);
    _logs[_logHead].message[sizeof(_logs[_logHead].message) - 1] = '\0';

    _logHead = (_logHead + 1) % MAX_EVENT_LOGS;
    if (_logCount < MAX_EVENT_LOGS) {
        _logCount++;
    }
}

int RelayManager::getEventLogs(EventLogEntry* outLogs, int maxCount) {
    if (!outLogs || maxCount <= 0) return 0;
    
    int count = 0;
    if (xSemaphoreTake(_relayMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        count = (_logCount < maxCount) ? _logCount : maxCount;
        for (int i = 0; i < count; i++) {
            // Read newest first
            int idx = (_logHead - 1 - i + MAX_EVENT_LOGS) % MAX_EVENT_LOGS;
            outLogs[i] = _logs[idx];
        }
        xSemaphoreGive(_relayMutex);
    }
    return count;
}

uint32_t RelayManager::getTickCount() {
    return _processTicks;
}

void RelayManager::applyPhysicalPin(uint8_t channelId) {
    if (channelId < 1 || channelId > NUM_RELAY_CHANNELS) return;
    uint8_t idx = channelId - 1;
    
    // Active LOW vs Active HIGH calculation
    uint8_t level = _channels[idx].activeLow ? (_channels[idx].state ? LOW : HIGH) : (_channels[idx].state ? HIGH : LOW);
    digitalWrite(_channels[idx].pin, level);
}

void RelayManager::setRelayState(uint8_t channelId, bool state) {
    if (channelId < 1 || channelId > NUM_RELAY_CHANNELS) return;
    
    if (xSemaphoreTake(_relayMutex, portMAX_DELAY) == pdTRUE) {
        uint8_t idx = channelId - 1;
        if (_channels[idx].state != state) {
            _channels[idx].state = state;
            applyPhysicalPin(channelId);
            
            char buf[64];
            snprintf(buf, sizeof(buf), "%s manual -> %s", _channels[idx].name, state ? "ON" : "OFF");
            logEvent(buf);
            
            // Notify Telegram
            TelegramManager::getInstance().notifyRelayStateChange(channelId, _channels[idx].name, state, "Manual Web/Shortcut");

            // If manual toggle occurred, cancel any one-shot pulse
            _pulseActive[idx] = false;
            
            saveToPreferences();
            Serial.printf("[RelayManager] Relay %d state -> %s\n", channelId, state ? "ON" : "OFF");
        }
        xSemaphoreGive(_relayMutex);
    }
}

void RelayManager::toggleRelay(uint8_t channelId) {
    if (channelId < 1 || channelId > NUM_RELAY_CHANNELS) return;
    setRelayState(channelId, !_channels[channelId - 1].state);
}

void RelayManager::setAllOn() {
    if (xSemaphoreTake(_relayMutex, portMAX_DELAY) == pdTRUE) {
        for (int i = 0; i < NUM_RELAY_CHANNELS; i++) {
            _channels[i].state = true;
            applyPhysicalPin(i + 1);
        }
        logEvent("⚡ All Relays Switched ON");
        TelegramManager::getInstance().sendMessage("🟢 <b>All 4 Relays Switched ON via Web GUI!</b>");
        saveToPreferences();
        Serial.println("[RelayManager] ALL ON triggered!");
        xSemaphoreGive(_relayMutex);
    }
}

void RelayManager::setAllOff() {
    if (xSemaphoreTake(_relayMutex, portMAX_DELAY) == pdTRUE) {
        for (int i = 0; i < NUM_RELAY_CHANNELS; i++) {
            _channels[i].state = false;
            _channels[i].timer.enabled = false;
            _channels[i].timer.isCountingDown = false;
            _channels[i].cycle.enabled = false;
            _pulseActive[i] = false;
            applyPhysicalPin(i + 1);
        }
        logEvent("🚨 EMERGENCY: All Relays Forced OFF");
        TelegramManager::getInstance().sendMessage("🚨 <b>EMERGENCY: All Relays Forced OFF via Web GUI!</b>");
        saveToPreferences();
        Serial.println("[RelayManager] Emergency ALL OFF triggered!");
        xSemaphoreGive(_relayMutex);
    }
}

RelayChannel RelayManager::getChannel(uint8_t channelId) {
    RelayChannel copy;
    if (channelId < 1 || channelId > NUM_RELAY_CHANNELS) return copy;
    
    if (xSemaphoreTake(_relayMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        copy = _channels[channelId - 1];
        xSemaphoreGive(_relayMutex);
    }
    return copy;
}

bool RelayManager::getRelayState(uint8_t channelId) {
    if (channelId < 1 || channelId > NUM_RELAY_CHANNELS) return false;
    bool state = false;
    if (xSemaphoreTake(_relayMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        state = _channels[channelId - 1].state;
        xSemaphoreGive(_relayMutex);
    }
    return state;
}

void RelayManager::setCountdownTimer(uint8_t channelId, uint32_t startDelaySec, uint32_t durationSec, bool enable) {
    if (channelId < 1 || channelId > NUM_RELAY_CHANNELS) return;
    
    if (xSemaphoreTake(_relayMutex, portMAX_DELAY) == pdTRUE) {
        uint8_t idx = channelId - 1;
        _channels[idx].timer.enabled = enable;
        _channels[idx].timer.startDelaySec = startDelaySec;
        _channels[idx].timer.durationSec = durationSec;
        _channels[idx].timer.timerStartEpoch = TimeManager::getInstance().getEpochTime();
        
        // Stop cycle mode if countdown timer starts
        if (enable) {
            _channels[idx].cycle.enabled = false;
            _channels[idx].timer.isCountingDown = true;
            if (startDelaySec > 0) {
                _channels[idx].timer.inDelayPhase = true;
                _channels[idx].timer.remainingSec = startDelaySec;
                _channels[idx].state = false; // Stay OFF while waiting
            } else {
                _channels[idx].timer.inDelayPhase = false;
                _channels[idx].timer.remainingSec = durationSec;
                _channels[idx].state = true;  // Turn ON immediately
            }
            applyPhysicalPin(channelId);
            
            char buf[64];
            snprintf(buf, sizeof(buf), "%s Timer: Delay %us, ON %us", _channels[idx].name, startDelaySec, durationSec);
            logEvent(buf);
            Serial.printf("[RelayManager] Relay %d Timer Started: Delay=%u s, Duration=%u s\n", channelId, startDelaySec, durationSec);
        } else {
            _channels[idx].timer.isCountingDown = false;
            char buf[64];
            snprintf(buf, sizeof(buf), "%s Timer Cancelled", _channels[idx].name);
            logEvent(buf);
            Serial.printf("[RelayManager] Relay %d Timer Cancelled\n", channelId);
        }
        xSemaphoreGive(_relayMutex);
    }
}

void RelayManager::cancelCountdownTimer(uint8_t channelId) {
    setCountdownTimer(channelId, 0, 0, false);
}

void RelayManager::setCycleAutomation(uint8_t channelId, uint32_t onSec, uint32_t offSec, uint32_t totalCycles, bool enable) {
    if (channelId < 1 || channelId > NUM_RELAY_CHANNELS) return;
    if (onSec == 0) onSec = 10;
    if (offSec == 0) offSec = 10;

    if (xSemaphoreTake(_relayMutex, portMAX_DELAY) == pdTRUE) {
        uint8_t idx = channelId - 1;
        _channels[idx].cycle.enabled = enable;
        _channels[idx].cycle.onSec = onSec;
        _channels[idx].cycle.offSec = offSec;
        _channels[idx].cycle.totalCycles = totalCycles;
        _channels[idx].cycle.currentCycle = 0;

        if (enable) {
            // Cancel any one-shot timer
            _channels[idx].timer.enabled = false;
            _channels[idx].timer.isCountingDown = false;

            _channels[idx].cycle.inOnPhase = true;
            _channels[idx].cycle.remainingSec = onSec;
            _channels[idx].state = true;
            applyPhysicalPin(channelId);

            char buf[64];
            snprintf(buf, sizeof(buf), "%s Cycle Loop Started (ON %us / OFF %us)", _channels[idx].name, onSec, offSec);
            logEvent(buf);
            Serial.printf("[RelayManager] Relay %d Cycle Started: ON=%u s, OFF=%u s, Cycles=%u\n", channelId, onSec, offSec, totalCycles);
        } else {
            char buf[64];
            snprintf(buf, sizeof(buf), "%s Cycle Loop Stopped", _channels[idx].name);
            logEvent(buf);
        }
        xSemaphoreGive(_relayMutex);
    }
}

void RelayManager::cancelCycleAutomation(uint8_t channelId) {
    setCycleAutomation(channelId, 0, 0, 0, false);
}

void RelayManager::setSchedule(uint8_t channelId, uint8_t startH, uint8_t startM, uint8_t endH, uint8_t endM, uint8_t daysActive, bool enable) {
    if (channelId < 1 || channelId > NUM_RELAY_CHANNELS) return;
    
    if (xSemaphoreTake(_relayMutex, portMAX_DELAY) == pdTRUE) {
        uint8_t idx = channelId - 1;
        _channels[idx].schedule.enabled = enable;
        _channels[idx].schedule.startHour = startH;
        _channels[idx].schedule.startMinute = startM;
        _channels[idx].schedule.startSecond = 0;
        _channels[idx].schedule.endHour = endH;
        _channels[idx].schedule.endMinute = endM;
        _channels[idx].schedule.endSecond = 0;
        _channels[idx].schedule.daysActive = daysActive;

        char buf[64];
        snprintf(buf, sizeof(buf), "%s Schedule %s (%02d:%02d-%02d:%02d)", _channels[idx].name, enable ? "Enabled" : "Disabled", startH, startM, endH, endM);
        logEvent(buf);

        saveToPreferences();
        Serial.printf("[RelayManager] Relay %d Schedule: %02d:%02d to %02d:%02d (Enabled: %d)\n", channelId, startH, startM, endH, endM, enable);
        xSemaphoreGive(_relayMutex);
    }
}

void RelayManager::cancelSchedule(uint8_t channelId) {
    if (channelId < 1 || channelId > NUM_RELAY_CHANNELS) return;
    if (xSemaphoreTake(_relayMutex, portMAX_DELAY) == pdTRUE) {
        _channels[channelId - 1].schedule.enabled = false;
        saveToPreferences();
        xSemaphoreGive(_relayMutex);
    }
}

void RelayManager::updateChannelConfig(uint8_t channelId, const char* name, bool activeLow, float ratedWatts) {
    if (channelId < 1 || channelId > NUM_RELAY_CHANNELS) return;
    
    if (xSemaphoreTake(_relayMutex, portMAX_DELAY) == pdTRUE) {
        uint8_t idx = channelId - 1;
        if (name && strlen(name) > 0) {
            strncpy(_channels[idx].name, name, sizeof(_channels[idx].name) - 1);
            _channels[idx].name[sizeof(_channels[idx].name) - 1] = '\0';
        }
        _channels[idx].activeLow = activeLow;
        _channels[idx].ratedWatts = (ratedWatts > 0.0f) ? ratedWatts : 100.0f;
        
        applyPhysicalPin(channelId);
        saveToPreferences();
        xSemaphoreGive(_relayMutex);
    }
}

void RelayManager::setPowerOnBehavior(uint8_t channelId, uint8_t powerOnState) {
    if (channelId < 1 || channelId > NUM_RELAY_CHANNELS) return;
    if (xSemaphoreTake(_relayMutex, portMAX_DELAY) == pdTRUE) {
        _channels[channelId - 1].powerOnState = powerOnState;
        saveToPreferences();
        xSemaphoreGive(_relayMutex);
    }
}

void RelayManager::processEngine() {
    _processTicks++;
    uint32_t now = millis();

    // 1-second interval execution for Timers, Cycles & Schedules
    if (now - _lastOneSecondTick >= 1000) {
        _lastOneSecondTick = now;

        if (xSemaphoreTake(_relayMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
            checkTimers();
            checkCycles();
            checkSchedules();
            updateRuntimeMetrics();
            xSemaphoreGive(_relayMutex);
        }
    }

    // Auto-save runtime every 5 minutes
    if (now - _lastRuntimeSaveTick >= 300000) {
        _lastRuntimeSaveTick = now;
        saveRuntime();
    }
}

void RelayManager::checkTimers() {
    for (int i = 0; i < NUM_RELAY_CHANNELS; i++) {
        if (_channels[i].timer.enabled && _channels[i].timer.isCountingDown) {
            if (_channels[i].timer.remainingSec > 0) {
                _channels[i].timer.remainingSec--;
            }

            if (_channels[i].timer.remainingSec == 0) {
                if (_channels[i].timer.inDelayPhase) {
                    // Delay phase ended -> Start ON phase
                    _channels[i].timer.inDelayPhase = false;
                    _channels[i].timer.remainingSec = _channels[i].timer.durationSec;
                    _channels[i].state = true;
                    applyPhysicalPin(i + 1);
                    
                    char buf[64];
                    snprintf(buf, sizeof(buf), "%s Timer Delay Done -> Turned ON", _channels[i].name);
                    logEvent(buf);
                    TelegramManager::getInstance().notifyTimerCompleted(i + 1, _channels[i].name, true);

                    Serial.printf("[RelayManager] Timer delay elapsed. Relay %d turned ON for %u s\n", i + 1, _channels[i].timer.durationSec);
                    
                    if (_channels[i].timer.durationSec == 0) {
                        // If duration is 0, stay ON permanently
                        _channels[i].timer.isCountingDown = false;
                        _channels[i].timer.enabled = false;
                    }
                } else {
                    // ON phase duration ended -> Turn OFF
                    _channels[i].state = false;
                    _channels[i].timer.isCountingDown = false;
                    _channels[i].timer.enabled = false;
                    applyPhysicalPin(i + 1);
                    
                    char buf[64];
                    snprintf(buf, sizeof(buf), "%s Timer Duration Ended -> OFF", _channels[i].name);
                    logEvent(buf);
                    TelegramManager::getInstance().notifyTimerCompleted(i + 1, _channels[i].name, false);

                    Serial.printf("[RelayManager] Timer duration finished. Relay %d turned OFF\n", i + 1);
                }
            }
        }
    }
}

void RelayManager::checkCycles() {
    for (int i = 0; i < NUM_RELAY_CHANNELS; i++) {
        if (_channels[i].cycle.enabled) {
            if (_channels[i].cycle.remainingSec > 0) {
                _channels[i].cycle.remainingSec--;
            }

            if (_channels[i].cycle.remainingSec == 0) {
                if (_channels[i].cycle.inOnPhase) {
                    // Switch to OFF phase
                    _channels[i].cycle.inOnPhase = false;
                    _channels[i].cycle.remainingSec = _channels[i].cycle.offSec;
                    _channels[i].state = false;
                    applyPhysicalPin(i + 1);

                    char buf[64];
                    snprintf(buf, sizeof(buf), "%s Cycle OFF Phase (%us)", _channels[i].name, _channels[i].cycle.offSec);
                    logEvent(buf);
                } else {
                    // OFF phase ended -> increment cycle
                    _channels[i].cycle.currentCycle++;
                    
                    // Check if max cycles reached
                    if (_channels[i].cycle.totalCycles > 0 && _channels[i].cycle.currentCycle >= _channels[i].cycle.totalCycles) {
                        _channels[i].cycle.enabled = false;
                        _channels[i].state = false;
                        applyPhysicalPin(i + 1);

                        char buf[64];
                        snprintf(buf, sizeof(buf), "%s All %u Cycles Completed", _channels[i].name, _channels[i].cycle.totalCycles);
                        logEvent(buf);
                        TelegramManager::getInstance().notifyCycleCompleted(i + 1, _channels[i].name, _channels[i].cycle.totalCycles);
                    } else {
                        // Switch to ON phase
                        _channels[i].cycle.inOnPhase = true;
                        _channels[i].cycle.remainingSec = _channels[i].cycle.onSec;
                        _channels[i].state = true;
                        applyPhysicalPin(i + 1);

                        char buf[64];
                        snprintf(buf, sizeof(buf), "%s Cycle %u ON Phase (%us)", _channels[i].name, _channels[i].cycle.currentCycle + 1, _channels[i].cycle.onSec);
                        logEvent(buf);
                    }
                }
            }
        }
    }
}

void RelayManager::checkSchedules() {
    if (!TimeManager::getInstance().isTimeSynchronized()) {
        return; // Need synchronized clock to evaluate daily schedules
    }

    int year, month, day, hour, minute, second, dayOfWeek;
    TimeManager::getInstance().getTimeBreakdown(year, month, day, hour, minute, second, dayOfWeek);

    uint32_t currentSec = hour * 3600 + minute * 60 + second;
    uint8_t dayBit = (1 << dayOfWeek); // Sunday is bit 0, Monday bit 1...

    for (int i = 0; i < NUM_RELAY_CHANNELS; i++) {
        // Countdown timer & Cycle loop have priority over schedule while active
        if ((_channels[i].timer.enabled && _channels[i].timer.isCountingDown) || _channels[i].cycle.enabled) {
            continue;
        }

        if (_channels[i].schedule.enabled) {
            bool isDayActive = ((_channels[i].schedule.daysActive & dayBit) != 0) || (_channels[i].schedule.daysActive == 0xFF);
            bool shouldBeOn = false;

            if (isDayActive) {
                uint32_t startSec = _channels[i].schedule.startHour * 3600 + _channels[i].schedule.startMinute * 60 + _channels[i].schedule.startSecond;
                uint32_t endSec   = _channels[i].schedule.endHour * 3600 + _channels[i].schedule.endMinute * 60 + _channels[i].schedule.endSecond;

                if (startSec < endSec) {
                    // Standard same-day window (e.g. 08:00 to 18:00)
                    shouldBeOn = (currentSec >= startSec && currentSec < endSec);
                } else if (startSec > endSec) {
                    // Overnight window spanning midnight (e.g. 20:00 to 06:00)
                    shouldBeOn = (currentSec >= startSec || currentSec < endSec);
                }
            }

            if (_channels[i].state != shouldBeOn) {
                _channels[i].state = shouldBeOn;
                applyPhysicalPin(i + 1);
                
                char buf[64];
                snprintf(buf, sizeof(buf), "%s Schedule Trigger -> %s", _channels[i].name, shouldBeOn ? "ON" : "OFF");
                logEvent(buf);
                TelegramManager::getInstance().notifyScheduleTriggered(i + 1, _channels[i].name, shouldBeOn);

                Serial.printf("[RelayManager] Schedule triggered: Relay %d -> %s\n", i + 1, shouldBeOn ? "ON" : "OFF");
            }
        }
    }
}

void RelayManager::updateRuntimeMetrics() {
    for (int i = 0; i < NUM_RELAY_CHANNELS; i++) {
        if (_channels[i].state) {
            _channels[i].totalOnSeconds++;
            _channels[i].sessionOnSeconds++;
        }
    }
}

void RelayManager::loadFromPreferences() {
    if (_prefs.begin("relay_cfg", true)) { // Read-only mode
        for (int i = 0; i < NUM_RELAY_CHANNELS; i++) {
            String prefix = "r" + String(i + 1) + "_";
            String name = _prefs.getString((prefix + "name").c_str(), _channels[i].name);
            strncpy(_channels[i].name, name.c_str(), sizeof(_channels[i].name) - 1);
            _channels[i].name[sizeof(_channels[i].name) - 1] = '\0';
            
            _channels[i].activeLow = _prefs.getBool((prefix + "actlow").c_str(), _channels[i].activeLow);
            _channels[i].ratedWatts = _prefs.getFloat((prefix + "watts").c_str(), _channels[i].ratedWatts);
            _channels[i].totalOnSeconds = _prefs.getUInt((prefix + "totsec").c_str(), 0);
            _channels[i].powerOnState = _prefs.getUChar((prefix + "pwr_on").c_str(), POWERON_OFF);
            bool lastState = _prefs.getBool((prefix + "last_st").c_str(), false);
            if (_channels[i].powerOnState == POWERON_RESTORE_LAST) {
                _channels[i].state = lastState;
            }

            _channels[i].schedule.enabled = _prefs.getBool((prefix + "sch_en").c_str(), false);
            _channels[i].schedule.startHour = _prefs.getUChar((prefix + "sch_sh").c_str(), 8);
            _channels[i].schedule.startMinute = _prefs.getUChar((prefix + "sch_sm").c_str(), 0);
            _channels[i].schedule.endHour = _prefs.getUChar((prefix + "sch_eh").c_str(), 18);
            _channels[i].schedule.endMinute = _prefs.getUChar((prefix + "sch_em").c_str(), 0);
            _channels[i].schedule.daysActive = _prefs.getUChar((prefix + "sch_days").c_str(), 0xFF);
        }
        _prefs.end();
        Serial.println("[RelayManager] Preferences loaded successfully.");
    }
}

void RelayManager::saveToPreferences() {
    if (_prefs.begin("relay_cfg", false)) { // Read-Write mode
        for (int i = 0; i < NUM_RELAY_CHANNELS; i++) {
            String prefix = "r" + String(i + 1) + "_";
            _prefs.putString((prefix + "name").c_str(), _channels[i].name);
            _prefs.putBool((prefix + "actlow").c_str(), _channels[i].activeLow);
            _prefs.putFloat((prefix + "watts").c_str(), _channels[i].ratedWatts);
            _prefs.putUChar((prefix + "pwr_on").c_str(), _channels[i].powerOnState);
            _prefs.putBool((prefix + "last_st").c_str(), _channels[i].state);
            
            _prefs.putBool((prefix + "sch_en").c_str(), _channels[i].schedule.enabled);
            _prefs.putUChar((prefix + "sch_sh").c_str(), _channels[i].schedule.startHour);
            _prefs.putUChar((prefix + "sch_sm").c_str(), _channels[i].schedule.startMinute);
            _prefs.putUChar((prefix + "sch_eh").c_str(), _channels[i].schedule.endHour);
            _prefs.putUChar((prefix + "sch_em").c_str(), _channels[i].schedule.endMinute);
            _prefs.putUChar((prefix + "sch_days").c_str(), _channels[i].schedule.daysActive);
        }
        _prefs.end();
    }
}

void RelayManager::saveRuntime() {
    if (_prefs.begin("relay_cfg", false)) {
        for (int i = 0; i < NUM_RELAY_CHANNELS; i++) {
            String prefix = "r" + String(i + 1) + "_";
            _prefs.putUInt((prefix + "totsec").c_str(), _channels[i].totalOnSeconds);
            _prefs.putBool((prefix + "last_st").c_str(), _channels[i].state);
        }
        _prefs.end();
    }
}
