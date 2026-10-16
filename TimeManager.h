#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include <Arduino.h>
#include <time.h>
#include <sys/time.h>
#include "Config.h"

class TimeManager {
public:
    static TimeManager& getInstance();

    void begin();
    
    // Time Synchronization methods
    void syncPhoneTime(uint32_t epochSec, int tzOffsetMinutes = 0);
    void setManualTime(int year, int month, int day, int hour, int min, int sec);
    
    // Getters
    uint32_t getEpochTime();
    bool isTimeSynchronized();
    int getTimezoneOffsetMinutes();
    
    // Formatted output helpers
    void getFormattedTime(char* buffer, size_t maxLen);      // "HH:MM:SS"
    void getFormattedDate(char* buffer, size_t maxLen);      // "YYYY-MM-DD"
    void getFormattedDateTime(char* buffer, size_t maxLen);  // "YYYY-MM-DD HH:MM:SS"
    
    // Breakdown getters
    void getTimeBreakdown(int &year, int &month, int &day, int &hour, int &minute, int &second, int &dayOfWeek);

private:
    TimeManager();
    ~TimeManager();
    
    // Prevent copy
    TimeManager(const TimeManager&) = delete;
    TimeManager& operator=(const TimeManager&) = delete;

    SemaphoreHandle_t _timeMutex;
    bool _isSynchronized;
    int _tzOffsetMinutes;
};

#endif // TIME_MANAGER_H
