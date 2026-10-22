#ifndef KERNEL_MANAGER_H
#define KERNEL_MANAGER_H

#include "Config.h"

// =====================================================================
// RTOS KERNEL FAULT & EXCEPTION DEFINITIONS
// =====================================================================

enum FaultSeverity {
    SEV_INFO = 0,
    SEV_WARNING,
    SEV_CRITICAL,
    SEV_FATAL
};

enum KernelFaultCode {
    FAULT_NONE = 0,
    FAULT_LOW_HEAP_MEMORY,          // Free heap below safety threshold (<8KB)
    FAULT_TASK_STALL,               // Core RTOS task took too long / missed heartbeat
    FAULT_WIFI_DISCONNECT,          // Unexpected Wi-Fi disconnect or AP crash
    FAULT_HARDWARE_PIN_MISMATCH,    // Physical GPIO readback mismatch
    FAULT_PREFERENCES_CORRUPT,      // LittleFS / NVS I/O failure
    FAULT_WATCHDOG_TRIGGERED,       // Hardware / Software WDT reset detected
    FAULT_TELEGRAM_API_FAIL,        // Telegram HTTPS error or SSL failure
    FAULT_INVALID_COMMAND,          // Malformed command or out-of-range relay index
    FAULT_THERMAL_ALERT,            // Abnormal temperature threshold reached
    FAULT_GENERAL_EXCEPTION         // Unhandled kernel exception
};

#define MAX_KERNEL_EXCEPTIONS 16

struct KernelExceptionRecord {
    uint32_t epoch;
    char timestamp[16];
    KernelFaultCode faultCode;
    FaultSeverity severity;
    char message[64];
    uint32_t freeHeap;
};

// =====================================================================
// RTOS KERNEL SUPERVISOR & EXCEPTION ENGINE
// =====================================================================

class KernelManager {
public:
    static KernelManager& getInstance();

    void begin();
    void processEngine();

    // Exception handling & raising
    void raiseException(KernelFaultCode faultCode, const char* message, FaultSeverity severity = SEV_WARNING);
    void clearExceptions();

    // Health & status query
    bool hasActiveFault() const;
    KernelFaultCode getActiveFaultCode() const;
    FaultSeverity getActiveFaultSeverity() const;
    KernelExceptionRecord getLastException() const;
    int getExceptionHistory(KernelExceptionRecord* outRecords, int maxCount);
    uint32_t getTotalExceptionCount() const;

    // Task Heartbeats & Watchdog
    void registerTaskHeartbeat(const char* taskName);

    // Helpers
    static const char* getFaultCodeName(KernelFaultCode code);
    static const char* getSeverityName(FaultSeverity sev);

private:
    KernelManager();
    ~KernelManager();

    KernelManager(const KernelManager&) = delete;
    KernelManager& operator=(const KernelManager&) = delete;

    void monitorHeapMemory();
    void monitorTaskHeartbeats();
    void executeEmergencyFailsafe(const char* reason);

    SemaphoreHandle_t _kernelMutex;
    bool _hasActiveFault;
    KernelFaultCode _activeFaultCode;
    FaultSeverity _activeFaultSeverity;
    uint32_t _totalExceptionCount;

    // Exception Ring Buffer
    KernelExceptionRecord _exceptions[MAX_KERNEL_EXCEPTIONS];
    uint8_t _exceptionHead;
    uint8_t _exceptionCount;

    // Task Heartbeats
    uint32_t _lastRelayTaskHeartbeat;
    uint32_t _lastPowerTaskHeartbeat;
    uint32_t _lastLoopCheckTick;
    uint32_t _lastHeapCheckTick;
    uint32_t _processTicks;
};

#endif // KERNEL_MANAGER_H
