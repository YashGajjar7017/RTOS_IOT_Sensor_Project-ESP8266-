#include "TimeManager.h"

TimeManager::TimeManager() : _isSynchronized(false), _tzOffsetMinutes(0) {
    _timeMutex = xSemaphoreCreateMutex();
}

TimeManager::~TimeManager() {
    if (_timeMutex != NULL) {
        vSemaphoreDelete(_timeMutex);
    }
}

TimeManager& TimeManager::getInstance() {
    static TimeManager instance;
    return instance;
}

void TimeManager::begin() {
    // Set default initial time if not synced (e.g. 2026-01-01 00:00:00)
    struct timeval tv;
    tv.tv_sec = 1767225600; // 2026-01-01 00:00:00 UTC
    tv.tv_usec = 0;
    settimeofday(&tv, NULL);
    _isSynchronized = false;
}

void TimeManager::syncPhoneTime(uint32_t epochSec, int tzOffsetMinutes) {
    if (xSemaphoreTake(_timeMutex, portMAX_DELAY) == pdTRUE) {
        // Adjust UTC epoch with timezone offset to ensure localtime_r reflects exact phone local clock
        uint32_t localEpoch = epochSec + (uint32_t)(tzOffsetMinutes * 60);
        struct timeval tv;
        tv.tv_sec = localEpoch;
        tv.tv_usec = 0;
        settimeofday(&tv, NULL);
        
        _tzOffsetMinutes = tzOffsetMinutes;
        _isSynchronized = true;
        
        Serial.printf("[TimeManager] Phone Time Synced! Local Epoch: %u (UTC: %u, Offset: %d mins)\n", localEpoch, epochSec, tzOffsetMinutes);
        xSemaphoreGive(_timeMutex);
    }
}

void TimeManager::setManualTime(int year, int month, int day, int hour, int min, int sec) {
    if (xSemaphoreTake(_timeMutex, portMAX_DELAY) == pdTRUE) {
        struct tm timeinfo;
        timeinfo.tm_year = year - 1900;
        timeinfo.tm_mon  = month - 1;
        timeinfo.tm_mday = day;
        timeinfo.tm_hour = hour;
        timeinfo.tm_min  = min;
        timeinfo.tm_sec  = sec;
        timeinfo.tm_isdst = -1;

        time_t t = mktime(&timeinfo);
        if (t != (time_t)(-1)) {
            struct timeval tv;
            tv.tv_sec = t;
            tv.tv_usec = 0;
            settimeofday(&tv, NULL);
            _isSynchronized = true;
            Serial.printf("[TimeManager] Manual Time Set: %04d-%02d-%02d %02d:%02d:%02d\n", year, month, day, hour, min, sec);
        }
        xSemaphoreGive(_timeMutex);
    }
}

uint32_t TimeManager::getEpochTime() {
    time_t now;
    time(&now);
    return (uint32_t)now;
}

bool TimeManager::isTimeSynchronized() {
    bool synced = false;
    if (xSemaphoreTake(_timeMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        synced = _isSynchronized;
        xSemaphoreGive(_timeMutex);
    }
    return synced;
}

int TimeManager::getTimezoneOffsetMinutes() {
    int offset = 0;
    if (xSemaphoreTake(_timeMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        offset = _tzOffsetMinutes;
        xSemaphoreGive(_timeMutex);
    }
    return offset;
}

void TimeManager::getFormattedTime(char* buffer, size_t maxLen) {
    time_t now;
    time(&now);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    int h12 = timeinfo.tm_hour % 12;
    if (h12 == 0) h12 = 12;
    const char* ampm = (timeinfo.tm_hour >= 12) ? "PM" : "AM";
    snprintf(buffer, maxLen, "%02d:%02d:%02d %s", h12, timeinfo.tm_min, timeinfo.tm_sec, ampm);
}

void TimeManager::getFormattedDate(char* buffer, size_t maxLen) {
    time_t now;
    time(&now);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    snprintf(buffer, maxLen, "%04d-%02d-%02d", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);
}

void TimeManager::getFormattedDateTime(char* buffer, size_t maxLen) {
    time_t now;
    time(&now);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    snprintf(buffer, maxLen, "%04d-%02d-%02d %02d:%02d:%02d", 
             timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
             timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}

void TimeManager::getTimeBreakdown(int &year, int &month, int &day, int &hour, int &minute, int &second, int &dayOfWeek) {
    time_t now;
    time(&now);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    year = timeinfo.tm_year + 1900;
    month = timeinfo.tm_mon + 1;
    day = timeinfo.tm_mday;
    hour = timeinfo.tm_hour;
    minute = timeinfo.tm_min;
    second = timeinfo.tm_sec;
    dayOfWeek = timeinfo.tm_wday; // 0 = Sunday
}
