#ifndef POWER_MANAGER_H
#define POWER_MANAGER_H

#include <Arduino.h>
#include "Config.h"
#include "Preferences.h"

class PowerManager {
public:
    static PowerManager& getInstance();

    void begin(const char* apSsid, const char* apPass);
    
    // Low Power Control & Permanent Stay-On
    void setLowPowerMode(bool enable, uint32_t sleepMin = 5, uint32_t wakeMin = 2);
    void setPermanentStayOn(bool permanentStayOn);
    void registerUserActivity();
    
    // Getters
    bool isLowPowerEnabled();
    bool isPermanentStayOn();
    bool isApSleeping();
    LowPowerConfig getConfig();
    uint32_t getSecondsUntilNextState();
    uint32_t getTickCount();

    // Engine loop for FreeRTOS task on Core 0
    void processEngine();

    // Preferences
    void loadFromPreferences();
    void saveToPreferences();

private:
    PowerManager();
    ~PowerManager();

    PowerManager(const PowerManager&) = delete;
    PowerManager& operator=(const PowerManager&) = delete;

    void wakeUpAP();
    void putAPToSleep();
    void updateSleepLed();

    LowPowerConfig _config;
    SemaphoreHandle_t _powerMutex;
    Preferences _prefs;

    char _apSsid[32];
    char _apPass[64];

    uint32_t _lastActivityTick;
    uint32_t _stateChangeTick;
    uint32_t _processTicks;
    float _ledPhase;
    int16_t _ledBrightness;
    int8_t _ledDirection;
};

#endif // POWER_MANAGER_H
