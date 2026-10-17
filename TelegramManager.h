#ifndef TELEGRAM_MANAGER_H
#define TELEGRAM_MANAGER_H

#include <Arduino.h>
#include "Config.h"
#include "Preferences.h"

class TelegramManager {
public:
    static TelegramManager& getInstance();

    void begin();
    
    // Configuration
    void updateConfig(bool enabled, const char* botToken, const char* chatId, 
                      const char* staSsid, const char* staPass,
                      bool notifyToggle, bool notifyTimer, bool notifySchedule);
    
    TelegramConfig getConfig();
    bool isConnected();
    bool isEnabled();

    // Message Sending
    bool sendMessage(const String& message);
    bool sendTestMessage();
    
    // Event Notification Hooks
    void notifyRelayStateChange(uint8_t channelId, const char* channelName, bool state, const char* triggerType);
    void notifyTimerCompleted(uint8_t channelId, const char* channelName, bool delayPhaseEnded);
    void notifyCycleCompleted(uint8_t channelId, const char* channelName, uint32_t completedCycles);
    void notifyScheduleTriggered(uint8_t channelId, const char* channelName, bool state);

    // FreeRTOS Execution Engine
    void processEngine();

    // Persistence
    void loadFromPreferences();
    void saveToPreferences();

private:
    TelegramManager();
    ~TelegramManager();

    TelegramManager(const TelegramManager&) = delete;
    TelegramManager& operator=(const TelegramManager&) = delete;

    void connectWiFiStation();
    void checkIncomingMessages();
    void handleCommand(const String& cmd, const String& fromChatId);
    String urlEncode(const String& str);

    TelegramConfig _config;
    SemaphoreHandle_t _telegramMutex;
    Preferences _prefs;

    uint32_t _lastPollTick;
    uint32_t _lastStaCheckTick;
    int32_t _lastUpdateId;
    bool _isConnectingSta;
};

#endif // TELEGRAM_MANAGER_H
