#pragma once

#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <sstream>

class Prefs {
public:
    static void SetBool(const std::string& key, bool value);
    static bool GetBool(const std::string& key, bool defaultValue = false);

    static void SetInt(const std::string& key, int value);
    static int GetInt(const std::string& key, int defaultValue = 0);

    static void SetFloat(const std::string& key, float value);
    static float GetFloat(const std::string& key, float defaultValue = 0.0f);

    static void SetString(const std::string& key, const std::string& value);
    static std::string GetString(const std::string& key, const std::string& defaultValue = "");

    static void Save();
    static void Load();

private:
    enum ValueType { BOOL, INT, FLOAT, STRING };

    struct Value {
        ValueType type;
        union {
            bool boolValue;
            int intValue;
            float floatValue;
        };
        std::string stringValue;

        Value() {}
        Value(bool value) : type(BOOL), boolValue(value) {}
        Value(int value) : type(INT), intValue(value) {}
        Value(float value) : type(FLOAT), floatValue(value) {}
        Value(const std::string& value) : type(STRING), stringValue(value) {}
    };

    static std::unordered_map<std::string, Value> values;
    static const std::string fileName;
};