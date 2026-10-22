#include "KernelManager.h"
#include "TimeManager.h"
#include "RelayManager.h"
#include "TelegramManager.h"

KernelManager::KernelManager() :
    _hasActiveFault(false),
    _activeFaultCode(FAULT_NONE),
    _activeFaultSeverity(SEV_INFO),
    _totalExceptionCount(0),
    _exceptionHead(0),
    _exceptionCount(0),
    _lastRelayTaskHeartbeat(0),
    _lastPowerTaskHeartbeat(0),
    _lastLoopCheckTick(0),
    _lastHeapCheckTick(0),
    _processTicks(0) {
    _kernelMutex = xSemaphoreCreateMutex();
}

KernelManager::~KernelManager() {
    if (_kernelMutex != NULL) {
        vSemaphoreDelete(_kernelMutex);
    }
}

KernelManager& KernelManager::getInstance() {
    static KernelManager instance;
    return instance;
}

void KernelManager::begin() {
    _lastRelayTaskHeartbeat = millis();
    _lastPowerTaskHeartbeat = millis();
    _lastLoopCheckTick = millis();
    _lastHeapCheckTick = millis();

    Serial.println("[RTOS Kernel] Initializing Kernel Supervisor & Exception Engine...");

#if defined(ESP8266)
    String resetReason = ESP.getResetReason();
    Serial.printf("[RTOS Kernel] ESP8266 Reset Reason: %s\n", resetReason.c_str());

    // Check if previous reboot was caused by an unhandled panic/watchdog
    if (resetReason.indexOf("Watchdog") != -1) {
        raiseException(FAULT_WATCHDOG_TRIGGERED, ("Prior Reboot: " + resetReason).c_str(), SEV_CRITICAL);
    } else if (resetReason.indexOf("Exception") != -1) {
        raiseException(FAULT_GENERAL_EXCEPTION, ("Prior Reboot: " + resetReason).c_str(), SEV_CRITICAL);
    }
#endif

    uint32_t heap = ESP.getFreeHeap();
    Serial.printf("[RTOS Kernel] Supervisor Started. Initial Heap: %u bytes\n", heap);
}

void KernelManager::processEngine() {
    _processTicks++;
    uint32_t now = millis();

    // 1. Monitor Heap Memory every 2 seconds
    if (now - _lastHeapCheckTick >= 2000) {
        _lastHeapCheckTick = now;
        monitorHeapMemory();
    }

    // 2. Monitor Task Heartbeats every 1 second
    if (now - _lastLoopCheckTick >= 1000) {
        _lastLoopCheckTick = now;
        monitorTaskHeartbeats();
    }

    // 3. Keep watchdog alive
#if defined(ESP8266)
    ESP.wdtFeed();
#endif
}

void KernelManager::registerTaskHeartbeat(const char* taskName) {
    if (!taskName) return;
    uint32_t now = millis();

    if (strcmp(taskName, "RelayTask") == 0) {
        _lastRelayTaskHeartbeat = now;
    } else if (strcmp(taskName, "PowerTask") == 0) {
        _lastPowerTaskHeartbeat = now;
    }
}

void KernelManager::monitorHeapMemory() {
    uint32_t freeHeap = ESP.getFreeHeap();

    // Fatal Threshold: < 4KB (Emergency shutdown to prevent crash while relays are hot)
    if (freeHeap < 4096) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Critical Out of Memory: Heap only %u bytes", freeHeap);
        raiseException(FAULT_LOW_HEAP_MEMORY, buf, SEV_FATAL);
    }
    // Warning Threshold: < 8KB
    else if (freeHeap < 8192 && _activeFaultCode != FAULT_LOW_HEAP_MEMORY) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Low RAM Warning: %u bytes free", freeHeap);
        raiseException(FAULT_LOW_HEAP_MEMORY, buf, SEV_WARNING);
    }
}

void KernelManager::monitorTaskHeartbeats() {
    uint32_t now = millis();

    // Check Relay Execution Engine heartbeat (> 4500ms without heartbeat indicates task deadlock)
    if (now - _lastRelayTaskHeartbeat > 4500) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Relay Engine Stall detected (%u ms gap)", now - _lastRelayTaskHeartbeat);
        raiseException(FAULT_TASK_STALL, buf, SEV_CRITICAL);
        _lastRelayTaskHeartbeat = now; // Prevent re-trigger storm
    }

    // Check Power & Telegram background engine heartbeat
    if (now - _lastPowerTaskHeartbeat > 4500) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Power Engine Stall detected (%u ms gap)", now - _lastPowerTaskHeartbeat);
        raiseException(FAULT_TASK_STALL, buf, SEV_WARNING);
        _lastPowerTaskHeartbeat = now;
    }
}

void KernelManager::raiseException(KernelFaultCode faultCode, const char* message, FaultSeverity severity) {
    if (xSemaphoreTake(_kernelMutex, portMAX_DELAY) == pdTRUE) {
        _hasActiveFault = true;
        _activeFaultCode = faultCode;
        _activeFaultSeverity = severity;
        _totalExceptionCount++;

        KernelExceptionRecord& rec = _exceptions[_exceptionHead];
        rec.epoch = TimeManager::getInstance().getEpochTime();
        rec.faultCode = faultCode;
        rec.severity = severity;
        rec.freeHeap = ESP.getFreeHeap();

        if (TimeManager::getInstance().isTimeSynchronized()) {
            TimeManager::getInstance().getFormattedTime(rec.timestamp, sizeof(rec.timestamp));
        } else {
            uint32_t sec = millis() / 1000;
            snprintf(rec.timestamp, sizeof(rec.timestamp), "+%02u:%02u:%02u", sec / 3600, (sec / 60) % 60, sec % 60);
        }

        if (message && strlen(message) > 0) {
            strncpy(rec.message, message, sizeof(rec.message) - 1);
            rec.message[sizeof(rec.message) - 1] = '\0';
        } else {
            strncpy(rec.message, getFaultCodeName(faultCode), sizeof(rec.message) - 1);
            rec.message[sizeof(rec.message) - 1] = '\0';
        }

        _exceptionHead = (_exceptionHead + 1) % MAX_KERNEL_EXCEPTIONS;
        if (_exceptionCount < MAX_KERNEL_EXCEPTIONS) {
            _exceptionCount++;
        }

        Serial.printf("[RTOS Kernel] ⚠️ EXCEPTION RAISED: [%s] Code: %d (%s) - %s (Heap: %u)\n",
                      getSeverityName(severity), faultCode, getFaultCodeName(faultCode), rec.message, rec.freeHeap);

        // Record in Relay Manager Event Log
        char logBuf[72];
        snprintf(logBuf, sizeof(logBuf), "KERNEL EXCEPTION: %s", rec.message);
        RelayManager::getInstance().logEvent(logBuf);

        // Notify Telegram on Critical or Fatal exceptions
        if (severity >= SEV_CRITICAL) {
            String tgAlert = "🚨 <b>RTOS KERNEL EXCEPTION ALERT</b>\n";
            tgAlert += "Severity: <b>" + String(getSeverityName(severity)) + "</b>\n";
            tgAlert += "Fault: <code>" + String(getFaultCodeName(faultCode)) + "</code>\n";
            tgAlert += "Details: <i>" + String(rec.message) + "</i>\n";
            tgAlert += "Free RAM: <code>" + String(rec.freeHeap) + " bytes</code>\n";
            tgAlert += "Timestamp: <code>" + String(rec.timestamp) + "</code>";
            TelegramManager::getInstance().sendMessage(tgAlert);
        }

        // Execute emergency fail-safe if fatal
        if (severity == SEV_FATAL) {
            executeEmergencyFailsafe(rec.message);
        }

        xSemaphoreGive(_kernelMutex);
    }
}

void KernelManager::executeEmergencyFailsafe(const char* reason) {
    Serial.println("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    Serial.printf("[RTOS Kernel PANIC] EMERGENCY FAILSAFE: %s\n", reason ? reason : "Unknown");
    Serial.println("  -> Disconnecting all 4 relay channels immediately!");
    Serial.println("  -> Rebooting system in 1.5 seconds...");
    Serial.println("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

    // Force all physical relay channels OFF immediately to prevent damage / fire
    RelayManager::getInstance().setAllOff();

    delay(1500);
#if defined(ESP8266)
    ESP.restart();
#endif
}

void KernelManager::clearExceptions() {
    if (xSemaphoreTake(_kernelMutex, portMAX_DELAY) == pdTRUE) {
        _hasActiveFault = false;
        _activeFaultCode = FAULT_NONE;
        _activeFaultSeverity = SEV_INFO;
        RelayManager::getInstance().logEvent("Kernel Exceptions Acknowledged/Cleared");
        xSemaphoreGive(_kernelMutex);
    }
}

bool KernelManager::hasActiveFault() const {
    return _hasActiveFault;
}

KernelFaultCode KernelManager::getActiveFaultCode() const {
    return _activeFaultCode;
}

FaultSeverity KernelManager::getActiveFaultSeverity() const {
    return _activeFaultSeverity;
}

KernelExceptionRecord KernelManager::getLastException() const {
    KernelExceptionRecord copy = {0, "00:00:00", FAULT_NONE, SEV_INFO, "None", 0};
    if (_exceptionCount > 0) {
        int idx = (_exceptionHead - 1 + MAX_KERNEL_EXCEPTIONS) % MAX_KERNEL_EXCEPTIONS;
        copy = _exceptions[idx];
    }
    return copy;
}

int KernelManager::getExceptionHistory(KernelExceptionRecord* outRecords, int maxCount) {
    if (!outRecords || maxCount <= 0) return 0;
    int count = (_exceptionCount < maxCount) ? _exceptionCount : maxCount;
    for (int i = 0; i < count; i++) {
        int idx = (_exceptionHead - 1 - i + MAX_KERNEL_EXCEPTIONS) % MAX_KERNEL_EXCEPTIONS;
        outRecords[i] = _exceptions[idx];
    }
    return count;
}

uint32_t KernelManager::getTotalExceptionCount() const {
    return _totalExceptionCount;
}

const char* KernelManager::getFaultCodeName(KernelFaultCode code) {
    switch (code) {
        case FAULT_NONE: return "NONE";
        case FAULT_LOW_HEAP_MEMORY: return "LOW_HEAP_MEMORY";
        case FAULT_TASK_STALL: return "TASK_STALL";
        case FAULT_WIFI_DISCONNECT: return "WIFI_DISCONNECT";
        case FAULT_HARDWARE_PIN_MISMATCH: return "PIN_MISMATCH";
        case FAULT_PREFERENCES_CORRUPT: return "NVS_STORAGE_ERROR";
        case FAULT_WATCHDOG_TRIGGERED: return "WATCHDOG_TRIGGERED";
        case FAULT_TELEGRAM_API_FAIL: return "TELEGRAM_FAIL";
        case FAULT_INVALID_COMMAND: return "INVALID_COMMAND";
        case FAULT_THERMAL_ALERT: return "THERMAL_ALERT";
        default: return "GENERAL_EXCEPTION";
    }
}

const char* KernelManager::getSeverityName(FaultSeverity sev) {
    switch (sev) {
        case SEV_INFO: return "INFO";
        case SEV_WARNING: return "WARNING";
        case SEV_CRITICAL: return "CRITICAL";
        case SEV_FATAL: return "FATAL";
        default: return "UNKNOWN";
    }
}
