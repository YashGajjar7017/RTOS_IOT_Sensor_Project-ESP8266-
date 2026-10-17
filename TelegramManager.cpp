#include "TelegramManager.h"
#include "RelayManager.h"
#include "TimeManager.h"

// Forward declaration of temperature helper
extern float getESP32TemperatureC();

TelegramManager::TelegramManager() :
    _lastPollTick(0),
    _lastStaCheckTick(0),
    _lastUpdateId(0),
    _isConnectingSta(false) {
    _telegramMutex = xSemaphoreCreateMutex();

    _config.enabled = false;
    _config.botToken[0] = '\0';
    _config.chatId[0] = '\0';
    _config.staSsid[0] = '\0';
    _config.staPass[0] = '\0';
    _config.notifyOnToggle = true;
    _config.notifyOnTimer = true;
    _config.notifyOnSchedule = true;
    _config.staConnected = false;
    _config.lastMessageEpoch = 0;
}

TelegramManager::~TelegramManager() {
    if (_telegramMutex != NULL) {
        vSemaphoreDelete(_telegramMutex);
    }
}

TelegramManager& TelegramManager::getInstance() {
    static TelegramManager instance;
    return instance;
}

void TelegramManager::begin() {
    loadFromPreferences();

    _lastPollTick = millis();
    _lastStaCheckTick = millis();

    if (_config.enabled && strlen(_config.staSsid) > 0) {
        connectWiFiStation();
    }
}

void TelegramManager::connectWiFiStation() {
    if (strlen(_config.staSsid) == 0) return;

    Serial.printf("[Telegram] Connecting Wi-Fi Station to SSID: %s\n", _config.staSsid);
    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(_config.staSsid, strlen(_config.staPass) > 0 ? _config.staPass : NULL);
    _isConnectingSta = true;
}

void TelegramManager::updateConfig(bool enabled, const char* botToken, const char* chatId,
                                  const char* staSsid, const char* staPass,
                                  bool notifyToggle, bool notifyTimer, bool notifySchedule) {
    if (xSemaphoreTake(_telegramMutex, portMAX_DELAY) == pdTRUE) {
        bool wifiChanged = false;
        if (staSsid && strcmp(_config.staSsid, staSsid) != 0) {
            strncpy(_config.staSsid, staSsid, sizeof(_config.staSsid) - 1);
            _config.staSsid[sizeof(_config.staSsid) - 1] = '\0';
            wifiChanged = true;
        }
        if (staPass) {
            strncpy(_config.staPass, staPass, sizeof(_config.staPass) - 1);
            _config.staPass[sizeof(_config.staPass) - 1] = '\0';
        }
        if (botToken) {
            strncpy(_config.botToken, botToken, sizeof(_config.botToken) - 1);
            _config.botToken[sizeof(_config.botToken) - 1] = '\0';
        }
        if (chatId) {
            strncpy(_config.chatId, chatId, sizeof(_config.chatId) - 1);
            _config.chatId[sizeof(_config.chatId) - 1] = '\0';
        }

        _config.enabled = enabled;
        _config.notifyOnToggle = notifyToggle;
        _config.notifyOnTimer = notifyTimer;
        _config.notifyOnSchedule = notifySchedule;

        saveToPreferences();

        if (enabled && (wifiChanged || WiFi.status() != WL_CONNECTED)) {
            connectWiFiStation();
        }

        Serial.printf("[Telegram] Config Updated. Enabled: %d | Bot: %s... | Chat: %s | WiFi: %s\n",
                      _config.enabled, 
                      strlen(_config.botToken) > 6 ? String(_config.botToken).substring(0, 6).c_str() : "none",
                      _config.chatId, _config.staSsid);

        xSemaphoreGive(_telegramMutex);
    }
}

TelegramConfig TelegramManager::getConfig() {
    TelegramConfig copy;
    if (xSemaphoreTake(_telegramMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        copy = _config;
        copy.staConnected = (WiFi.status() == WL_CONNECTED);
        xSemaphoreGive(_telegramMutex);
    }
    return copy;
}

bool TelegramManager::isConnected() {
    return (WiFi.status() == WL_CONNECTED);
}

bool TelegramManager::isEnabled() {
    bool en = false;
    if (xSemaphoreTake(_telegramMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        en = _config.enabled && (strlen(_config.botToken) > 0) && (strlen(_config.chatId) > 0);
        xSemaphoreGive(_telegramMutex);
    }
    return en;
}

String TelegramManager::urlEncode(const String& str) {
    String encoded = "";
    char c;
    char code0;
    char code1;
    for (size_t i = 0; i < str.length(); i++) {
        c = str.charAt(i);
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded += c;
        } else if (c == ' ') {
            encoded += '+';
        } else {
            code1 = (c & 0xf) + '0';
            if ((c & 0xf) > 9) {
                code1 = (c & 0xf) - 10 + 'A';
            }
            c = (c >> 4) & 0xf;
            code0 = c + '0';
            if (c > 9) {
                code0 = c - 10 + 'A';
            }
            encoded += '%';
            encoded += code0;
            encoded += code1;
        }
    }
    return encoded;
}

bool TelegramManager::sendMessage(const String& message) {
    if (!isEnabled() || WiFi.status() != WL_CONNECTED) {
        return false;
    }

    WiFiClientSecure client;
    client.setInsecure(); // Bypass SSL cert validation for low RAM footprint & high speed
    client.setTimeout(4000);

    HTTPClient https;
    String url = "https://api.telegram.org/bot" + String(_config.botToken) + 
                 "/sendMessage?chat_id=" + String(_config.chatId) + 
                 "&text=" + urlEncode(message) + 
                 "&parse_mode=HTML";

    bool success = false;
    if (https.begin(client, url)) {
        int httpCode = https.GET();
        if (httpCode == HTTP_CODE_OK) {
            success = true;
            _config.lastMessageEpoch = TimeManager::getInstance().getEpochTime();
            Serial.println("[Telegram] Message sent successfully!");
        } else {
            Serial.printf("[Telegram] Send failed! HTTP code: %d\n", httpCode);
        }
        https.end();
    }
    return success;
}

bool TelegramManager::sendTestMessage() {
    if (strlen(_config.botToken) == 0 || strlen(_config.chatId) == 0) {
        return false;
    }

    char timeBuf[16];
    TimeManager::getInstance().getFormattedTime(timeBuf, sizeof(timeBuf));

    String testMsg = "🚀 <b>ESP32 Real-Time OS Relay System</b>\n";
    testMsg += "━━━━━━━━━━━━━━━━━━━\n";
    testMsg += "✅ <b>Telegram Bot Connected Successfully!</b>\n";
    testMsg += "🕒 Time: <code>" + String(timeBuf) + "</code>\n";
    testMsg += "📶 AP IP: <code>192.168.4.1</code>\n";
    testMsg += "🌐 STA IP: <code>" + WiFi.localIP().toString() + "</code>\n";
    testMsg += "💡 <i>Send /help to see all available commands.</i>";

    return sendMessage(testMsg);
}

void TelegramManager::notifyRelayStateChange(uint8_t channelId, const char* channelName, bool state, const char* triggerType) {
    if (!_config.enabled || !_config.notifyOnToggle) return;

    char timeBuf[16];
    TimeManager::getInstance().getFormattedTime(timeBuf, sizeof(timeBuf));

    String msg = state ? "🟢 " : "🔴 ";
    msg += "<b>Relay " + String(channelId) + " (" + String(channelName) + "): " + (state ? "ON" : "OFF") + "</b>\n";
    msg += "⚡ Trigger: <i>" + String(triggerType) + "</i>\n";
    msg += "🕒 Time: <code>" + String(timeBuf) + "</code>";

    sendMessage(msg);
}

void TelegramManager::notifyTimerCompleted(uint8_t channelId, const char* channelName, bool delayPhaseEnded) {
    if (!_config.enabled || !_config.notifyOnTimer) return;

    char timeBuf[16];
    TimeManager::getInstance().getFormattedTime(timeBuf, sizeof(timeBuf));

    String msg = "⏱️ <b>Timer Alert: " + String(channelName) + "</b>\n";
    if (delayPhaseEnded) {
        msg += "🟢 Delay finished -> Relay switched <b>ON</b>\n";
    } else {
        msg += "🔴 Duration finished -> Relay switched <b>OFF</b>\n";
    }
    msg += "🕒 Time: <code>" + String(timeBuf) + "</code>";

    sendMessage(msg);
}

void TelegramManager::notifyCycleCompleted(uint8_t channelId, const char* channelName, uint32_t completedCycles) {
    if (!_config.enabled || !_config.notifyOnTimer) return;

    String msg = "🔁 <b>Cycle Loop Alert: " + String(channelName) + "</b>\n";
    msg += "Completed all " + String(completedCycles) + " cycles.\n";
    msg += "Relay turned <b>OFF</b>.";

    sendMessage(msg);
}

void TelegramManager::notifyScheduleTriggered(uint8_t channelId, const char* channelName, bool state) {
    if (!_config.enabled || !_config.notifyOnSchedule) return;

    char timeBuf[16];
    TimeManager::getInstance().getFormattedTime(timeBuf, sizeof(timeBuf));

    String msg = "📅 <b>Schedule Triggered: " + String(channelName) + "</b>\n";
    msg += (state ? "🟢 Switched <b>ON</b>" : "🔴 Switched <b>OFF</b>") + String("\n");
    msg += "🕒 Time: <code>" + String(timeBuf) + "</code>";

    sendMessage(msg);
}

void TelegramManager::handleCommand(const String& cmd, const String& fromChatId) {
    String cleanCmd = cmd;
    cleanCmd.trim();
    cleanCmd.toLowerCase();

    // Strip bot username if present (e.g. /status@MyBot -> /status)
    int atIdx = cleanCmd.indexOf('@');
    if (atIdx != -1) {
        cleanCmd = cleanCmd.substring(0, atIdx);
    }

    Serial.printf("[Telegram] Received Command: %s\n", cleanCmd.c_str());

    char timeBuf[16];
    TimeManager::getInstance().getFormattedTime(timeBuf, sizeof(timeBuf));

    if (cleanCmd == "/start" || cleanCmd == "/help") {
        String reply = "⚡ <b>ESP32 RTOS 4-Channel Relay Bot</b>\n";
        reply += "━━━━━━━━━━━━━━━━━━━\n";
        reply += "Available Commands:\n";
        reply += "📊 <code>/status</code> - System & Relay Status\n";
        reply += "⚡ <code>/r1_on</code> | <code>/r1_off</code> | <code>/r1_toggle</code>\n";
        reply += "⚡ <code>/r2_on</code> | <code>/r2_off</code> | <code>/r2_toggle</code>\n";
        reply += "⚡ <code>/r3_on</code> | <code>/r3_off</code> | <code>/r3_toggle</code>\n";
        reply += "⚡ <code>/r4_on</code> | <code>/r4_off</code> | <code>/r4_toggle</code>\n";
        reply += "🟢 <code>/all_on</code> - Turn all relays ON\n";
        reply += "🚨 <code>/all_off</code> - Emergency all relays OFF\n";
        reply += "🕒 Time: <code>" + String(timeBuf) + "</code>";
        sendMessage(reply);
    } else if (cleanCmd == "/status") {
        RelayChannel r1 = RelayManager::getInstance().getChannel(1);
        RelayChannel r2 = RelayManager::getInstance().getChannel(2);
        RelayChannel r3 = RelayManager::getInstance().getChannel(3);
        RelayChannel r4 = RelayManager::getInstance().getChannel(4);

        float tempC = getESP32TemperatureC();
        uint32_t uptime = millis() / 1000;
        uint32_t up_h = uptime / 3600;
        uint32_t up_m = (uptime % 3600) / 60;

        float totalKwh = ((r1.totalOnSeconds / 3600.0f) * (r1.ratedWatts / 1000.0f)) +
                         ((r2.totalOnSeconds / 3600.0f) * (r2.ratedWatts / 1000.0f)) +
                         ((r3.totalOnSeconds / 3600.0f) * (r3.ratedWatts / 1000.0f)) +
                         ((r4.totalOnSeconds / 3600.0f) * (r4.ratedWatts / 1000.0f));

        String reply = "📊 <b>ESP32 RTOS System Status</b>\n";
        reply += "━━━━━━━━━━━━━━━━━━━\n";
        reply += (r1.state ? "🟢" : "🔴") + String(" R1 (D5): <b>") + (r1.state ? "ON" : "OFF") + "</b> (" + String(r1.name) + ")\n";
        reply += (r2.state ? "🟢" : "🔴") + String(" R2 (D18): <b>") + (r2.state ? "ON" : "OFF") + "</b> (" + String(r2.name) + ")\n";
        reply += (r3.state ? "🟢" : "🔴") + String(" R3 (D19): <b>") + (r3.state ? "ON" : "OFF") + "</b> (" + String(r3.name) + ")\n";
        reply += (r4.state ? "🟢" : "🔴") + String(" R4 (D21): <b>") + (r4.state ? "ON" : "OFF") + "</b> (" + String(r4.name) + ")\n";
        reply += "━━━━━━━━━━━━━━━━━━━\n";
        reply += "⚡ Total Energy: <code>" + String(totalKwh, 3) + " kWh</code>\n";
        reply += "🌡️ Temp: <code>" + String(tempC, 1) + " °C</code>\n";
        reply += "⏱️ Uptime: <code>" + String(up_h) + "h " + String(up_m) + "m</code>\n";
        reply += "🕒 Time: <code>" + String(timeBuf) + "</code>";
        sendMessage(reply);
    } else if (cleanCmd == "/r1_on") {
        RelayManager::getInstance().setRelayState(1, true);
    } else if (cleanCmd == "/r1_off") {
        RelayManager::getInstance().setRelayState(1, false);
    } else if (cleanCmd == "/r1_toggle") {
        RelayManager::getInstance().toggleRelay(1);
    } else if (cleanCmd == "/r2_on") {
        RelayManager::getInstance().setRelayState(2, true);
    } else if (cleanCmd == "/r2_off") {
        RelayManager::getInstance().setRelayState(2, false);
    } else if (cleanCmd == "/r2_toggle") {
        RelayManager::getInstance().toggleRelay(2);
    } else if (cleanCmd == "/r3_on") {
        RelayManager::getInstance().setRelayState(3, true);
    } else if (cleanCmd == "/r3_off") {
        RelayManager::getInstance().setRelayState(3, false);
    } else if (cleanCmd == "/r3_toggle") {
        RelayManager::getInstance().toggleRelay(3);
    } else if (cleanCmd == "/r4_on") {
        RelayManager::getInstance().setRelayState(4, true);
    } else if (cleanCmd == "/r4_off") {
        RelayManager::getInstance().setRelayState(4, false);
    } else if (cleanCmd == "/r4_toggle") {
        RelayManager::getInstance().toggleRelay(4);
    } else if (cleanCmd == "/all_on") {
        for (int i = 1; i <= NUM_RELAY_CHANNELS; i++) {
            RelayManager::getInstance().setRelayState(i, true);
        }
        sendMessage("🟢 <b>All 4 Relays Switched ON!</b>");
    } else if (cleanCmd == "/all_off") {
        RelayManager::getInstance().setAllOff();
        sendMessage("🚨 <b>EMERGENCY: All Relays Switched OFF!</b>");
    } else {
        sendMessage("❓ Unknown command. Send <code>/help</code> for available commands.");
    }
}

void TelegramManager::checkIncomingMessages() {
    if (!isEnabled() || WiFi.status() != WL_CONNECTED) {
        return;
    }

    WiFiClientSecure client;
    client.setInsecure();
    client.setTimeout(4000);

    HTTPClient https;
    String url = "https://api.telegram.org/bot" + String(_config.botToken) + 
                 "/getUpdates?offset=" + String(_lastUpdateId + 1) + 
                 "&limit=5&timeout=0";

    if (https.begin(client, url)) {
        int httpCode = https.GET();
        if (httpCode == HTTP_CODE_OK) {
            String payload = https.getString();
            
            // Simple robust JSON extraction for update_id, chat_id and text
            int updateIdx = 0;
            while ((updateIdx = payload.indexOf("\"update_id\":", updateIdx)) != -1) {
                int idStart = updateIdx + 12;
                int idEnd = payload.indexOf(',', idStart);
                if (idEnd != -1) {
                    int32_t updId = payload.substring(idStart, idEnd).toInt();
                    if (updId > _lastUpdateId) {
                        _lastUpdateId = updId;
                    }
                }

                int textIdx = payload.indexOf("\"text\":\"", updateIdx);
                if (textIdx != -1) {
                    int textStart = textIdx + 8;
                    int textEnd = payload.indexOf('"', textStart);
                    if (textEnd != -1) {
                        String cmd = payload.substring(textStart, textEnd);
                        
                        // Check if from configured chat ID
                        int chatIdx = payload.indexOf("\"chat\":{\"id\":", updateIdx);
                        String fromChat = "";
                        if (chatIdx != -1) {
                            int cStart = chatIdx + 13;
                            int cEnd = payload.indexOf(',', cStart);
                            if (cEnd != -1) {
                                fromChat = payload.substring(cStart, cEnd);
                            }
                        }

                        if (fromChat.length() == 0 || fromChat == String(_config.chatId)) {
                            handleCommand(cmd, fromChat);
                        }
                    }
                }
                updateIdx += 12;
            }
        }
        https.end();
    }
}

void TelegramManager::processEngine() {
    uint32_t now = millis();

    // 1. Check Wi-Fi Station status every 10s
    if (now - _lastStaCheckTick >= 10000) {
        _lastStaCheckTick = now;
        if (_config.enabled && strlen(_config.staSsid) > 0) {
            if (WiFi.status() != WL_CONNECTED && !_isConnectingSta) {
                connectWiFiStation();
            } else if (WiFi.status() == WL_CONNECTED) {
                _isConnectingSta = false;
            }
        }
    }

    // 2. Poll for incoming Telegram bot commands every 6 seconds
    if (_config.enabled && WiFi.status() == WL_CONNECTED) {
        if (now - _lastPollTick >= 6000) {
            _lastPollTick = now;
            checkIncomingMessages();
        }
    }
}

void TelegramManager::loadFromPreferences() {
    if (_prefs.begin("tg_cfg", true)) {
        _config.enabled = _prefs.getBool("tg_en", false);
        
        String tok = _prefs.getString("tg_tok", "");
        strncpy(_config.botToken, tok.c_str(), sizeof(_config.botToken) - 1);
        _config.botToken[sizeof(_config.botToken) - 1] = '\0';

        String cid = _prefs.getString("tg_cid", "");
        strncpy(_config.chatId, cid.c_str(), sizeof(_config.chatId) - 1);
        _config.chatId[sizeof(_config.chatId) - 1] = '\0';

        String ssid = _prefs.getString("tg_ssid", "");
        strncpy(_config.staSsid, ssid.c_str(), sizeof(_config.staSsid) - 1);
        _config.staSsid[sizeof(_config.staSsid) - 1] = '\0';

        String pass = _prefs.getString("tg_pass", "");
        strncpy(_config.staPass, pass.c_str(), sizeof(_config.staPass) - 1);
        _config.staPass[sizeof(_config.staPass) - 1] = '\0';

        _config.notifyOnToggle = _prefs.getBool("tg_n_tog", true);
        _config.notifyOnTimer = _prefs.getBool("tg_n_tim", true);
        _config.notifyOnSchedule = _prefs.getBool("tg_n_sch", true);

        _prefs.end();
        Serial.printf("[Telegram] Preferences loaded. Enabled: %d | SSID: %s\n", _config.enabled, _config.staSsid);
    }
}

void TelegramManager::saveToPreferences() {
    if (_prefs.begin("tg_cfg", false)) {
        _prefs.putBool("tg_en", _config.enabled);
        _prefs.putString("tg_tok", _config.botToken);
        _prefs.putString("tg_cid", _config.chatId);
        _prefs.putString("tg_ssid", _config.staSsid);
        _prefs.putString("tg_pass", _config.staPass);
        _prefs.putBool("tg_n_tog", _config.notifyOnToggle);
        _prefs.putBool("tg_n_tim", _config.notifyOnTimer);
        _prefs.putBool("tg_n_sch", _config.notifyOnSchedule);
        _prefs.end();
        Serial.println("[Telegram] Preferences saved permanently.");
    }
}
