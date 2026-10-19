#ifndef RELAY_MANAGER_H
#define RELAY_MANAGER_H

#include <Arduino.h>
#include "Config.h"
#include "Preferences.h"
#include "TimeManager.h"

class RelayManager {
public:
    static RelayManager& getInstance();

    void begin();
    
    // Core Relay Actions (Thread-safe)
    void setRelayState(uint8_t channelId, bool state);
    void toggleRelay(uint8_t channelId);
    void setAllOn();
    void setAllOff();
    
    // Inching / Pulse Trigger (0.5s - 10s momentary pulse) - Commented out for now
    // void triggerPulse(uint8_t channelId, uint32_t pulseDurationMs);

    // Getters
    RelayChannel getChannel(uint8_t channelId);
    bool getRelayState(uint8_t channelId);
    
    // Countdown Timer Management
    void setCountdownTimer(uint8_t channelId, uint32_t startDelaySec, uint32_t durationSec, bool enable);
    void cancelCountdownTimer(uint8_t channelId);
    
    // Cyclic Repeat Automation Management
    void setCycleAutomation(uint8_t channelId, uint32_t onSec, uint32_t offSec, uint32_t totalCycles, bool enable);
    void cancelCycleAutomation(uint8_t channelId);

    // Daily Schedule Management
    void setSchedule(uint8_t channelId, uint8_t startH, uint8_t startM, uint8_t endH, uint8_t endM, uint8_t daysActive, bool enable);
    void cancelSchedule(uint8_t channelId);
    
    // Configuration & Metadata
    void updateChannelConfig(uint8_t channelId, const char* name, bool activeLow, float ratedWatts);
    void setPowerOnBehavior(uint8_t channelId, uint8_t powerOnState);
    
    // Real-Time Event Logging & Telemetry
    void logEvent(const char* msg);
    int getEventLogs(EventLogEntry* outLogs, int maxCount);
    uint32_t getTickCount();

    // FreeRTOS Execution Engine (called every ~100ms from Core 1 Task)
    void processEngine();

    // NVS Persistence
    void loadFromPreferences();
    void saveToPreferences();
    void saveRuntime();

private:
    RelayManager();
    ~RelayManager();
    
    RelayManager(const RelayManager&) = delete;
    RelayManager& operator=(const RelayManager&) = delete;

    void applyPhysicalPin(uint8_t channelId);
    void checkTimers();
    void checkCycles();
    void checkSchedules();
    void updateRuntimeMetrics();

    RelayChannel _channels[NUM_RELAY_CHANNELS];
    SemaphoreHandle_t _relayMutex;
    Preferences _prefs;

    // Pulse state helper
    uint32_t _pulseEndTick[NUM_RELAY_CHANNELS];
    bool _pulseActive[NUM_RELAY_CHANNELS];

    // Transition tracking for Daily Schedules (prevents manual override fights)
    bool _inScheduleWindow[NUM_RELAY_CHANNELS];
    bool _scheduleInitialized[NUM_RELAY_CHANNELS];

    // Event Log Ring Buffer
    EventLogEntry _logs[MAX_EVENT_LOGS];
    uint8_t _logHead;
    uint8_t _logCount;

    uint32_t _lastProcessTick;
    uint32_t _lastRuntimeSaveTick;
    uint32_t _lastOneSecondTick;
    uint32_t _processTicks;
};

#endif // RELAY_MANAGER_H
