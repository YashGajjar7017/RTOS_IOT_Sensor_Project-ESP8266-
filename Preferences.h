#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <Arduino.h>

#if defined(ESP8266)
#include <LittleFS.h>
#include <map>

/**
 * Drop-in Preferences (NVS) replacement for ESP8266 using LittleFS.
 * Compatible with ESP8266 Arduino Core 2.7.x through 3.1.x.
 */
class Preferences {
public:
    Preferences() : _readOnly(false), _started(false), _dirty(false) {}
    ~Preferences() { end(); }

    bool begin(const char* name, bool readOnly = false, const char* partition_label = NULL) {
        (void)partition_label;
        _name = (name && strlen(name) > 0) ? name : "default";
        _readOnly = readOnly;
        _dirty = false;

        static bool fsStarted = false;
        if (!fsStarted) {
            if (!LittleFS.begin()) {
                LittleFS.format();
                LittleFS.begin();
            }
            fsStarted = true;
        }

        _started = true;
        _load();
        return true;
    }

    void end() {
        if (_started) {
            if (_dirty) {
                _save();
                _dirty = false;
            }
            _started = false;
        }
    }

    bool clear() {
        if (_readOnly || !_started) return false;
        _data.clear();
        _dirty = true;
        _save();
        _dirty = false;
        return true;
    }

    bool remove(const char* key) {
        if (_readOnly || !_started || !key) return false;
        if (_data.erase(String(key)) > 0) {
            _dirty = true;
            return true;
        }
        return false;
    }

    size_t putBool(const char* key, bool value) {
        return _put(key, value ? "1" : "0");
    }

    size_t putChar(const char* key, int8_t value) {
        return _put(key, String((int)value));
    }

    size_t putUChar(const char* key, uint8_t value) {
        return _put(key, String((unsigned int)value));
    }

    size_t putShort(const char* key, int16_t value) {
        return _put(key, String((int)value));
    }

    size_t putUShort(const char* key, uint16_t value) {
        return _put(key, String((unsigned int)value));
    }

    size_t putInt(const char* key, int32_t value) {
        return _put(key, String((long)value));
    }

    size_t putUInt(const char* key, uint32_t value) {
        return _put(key, String((unsigned long)value));
    }

    size_t putFloat(const char* key, float value) {
        return _put(key, String(value, 4));
    }

    size_t putString(const char* key, const String& value) {
        return _put(key, value);
    }

    size_t putString(const char* key, const char* value) {
        return _put(key, String(value ? value : ""));
    }

    bool getBool(const char* key, bool defaultValue = false) {
        String v;
        if (_get(key, v)) {
            return (v == "1" || v.equalsIgnoreCase("true"));
        }
        return defaultValue;
    }

    int8_t getChar(const char* key, int8_t defaultValue = 0) {
        String v;
        if (_get(key, v)) return (int8_t)v.toInt();
        return defaultValue;
    }

    uint8_t getUChar(const char* key, uint8_t defaultValue = 0) {
        String v;
        if (_get(key, v)) return (uint8_t)v.toInt();
        return defaultValue;
    }

    int16_t getShort(const char* key, int16_t defaultValue = 0) {
        String v;
        if (_get(key, v)) return (int16_t)v.toInt();
        return defaultValue;
    }

    uint16_t getUShort(const char* key, uint16_t defaultValue = 0) {
        String v;
        if (_get(key, v)) return (uint16_t)v.toInt();
        return defaultValue;
    }

    int32_t getInt(const char* key, int32_t defaultValue = 0) {
        String v;
        if (_get(key, v)) return (int32_t)v.toInt();
        return defaultValue;
    }

    uint32_t getUInt(const char* key, uint32_t defaultValue = 0) {
        String v;
        if (_get(key, v)) return (uint32_t)strtoul(v.c_str(), NULL, 10);
        return defaultValue;
    }

    float getFloat(const char* key, float defaultValue = 0.0f) {
        String v;
        if (_get(key, v)) return v.toFloat();
        return defaultValue;
    }

    String getString(const char* key, const String& defaultValue = "") {
        String v;
        if (_get(key, v)) return v;
        return defaultValue;
    }

    String getString(const char* key, const char* defaultValue) {
        return getString(key, String(defaultValue ? defaultValue : ""));
    }

private:
    String _name;
    bool _readOnly;
    bool _started;
    std::map<String, String> _data;

    String _filePath() const {
        return "/prefs_" + _name + ".txt";
    }

    void _load() {
        _data.clear();
        String path = _filePath();
        if (!LittleFS.exists(path)) return;
        File f = LittleFS.open(path, "r");
        if (!f) return;
        while (f.available()) {
            String line = f.readStringUntil('\n');
            line.trim();
            if (line.length() == 0) continue;
            int idx = line.indexOf('=');
            if (idx > 0) {
                String k = line.substring(0, idx);
                String v = line.substring(idx + 1);
                _data[k] = v;
            }
        }
        f.close();
    }

    void _save() {
        if (_readOnly || !_started) return;
        String path = _filePath();
        File f = LittleFS.open(path, "w");
        if (!f) return;
        for (std::map<String, String>::const_iterator it = _data.begin(); it != _data.end(); ++it) {
            f.print(it->first);
            f.print('=');
            f.println(it->second);
        }
        f.close();
    }

    size_t _put(const char* key, const String& val) {
        if (_readOnly || !_started || !key) return 0;
        _data[String(key)] = val;
        _dirty = true;
        return val.length();
    }

    bool _get(const char* key, String& out) {
        if (!_started || !key) return false;
        std::map<String, String>::const_iterator it = _data.find(String(key));
        if (it != _data.end()) {
            out = it->second;
            return true;
        }
        return false;
    }
private:
    bool _dirty;
};

#else
// On ESP32, include native hardware NVS Preferences header
#include_next <Preferences.h>
#endif

#endif // PREFERENCES_H
