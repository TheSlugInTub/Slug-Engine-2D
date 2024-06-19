#include "Prefs.h"

std::unordered_map<std::string, Prefs::Value> Prefs::values;
const std::string Prefs::fileName = "Prefs.dat";

void Prefs::SetBool(const std::string& key, bool value) {
    values[key] = Value(value);
}

bool Prefs::GetBool(const std::string& key, bool defaultValue) {
    auto it = values.find(key);
    if (it != values.end() && it->second.type == BOOL) {
        return it->second.boolValue;
    }
    return defaultValue;
}

void Prefs::SetInt(const std::string& key, int value) {
    values[key] = Value(value);
}

int Prefs::GetInt(const std::string& key, int defaultValue) {
    auto it = values.find(key);
    if (it != values.end() && it->second.type == INT) {
        return it->second.intValue;
    }
    return defaultValue;
}

void Prefs::SetFloat(const std::string& key, float value) {
    values[key] = Value(value);
}

float Prefs::GetFloat(const std::string& key, float defaultValue) {
    auto it = values.find(key);
    if (it != values.end() && it->second.type == FLOAT) {
        return it->second.floatValue;
    }
    return defaultValue;
}

void Prefs::SetString(const std::string& key, const std::string& value) {
    values[key] = Value(value);
}

std::string Prefs::GetString(const std::string& key, const std::string& defaultValue) {
    auto it = values.find(key);
    if (it != values.end() && it->second.type == STRING) {
        return it->second.stringValue;
    }
    return defaultValue;
}

void Prefs::Save() {
    std::ofstream file(fileName, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for saving: " << fileName << std::endl;
        return;
    }

    for (const auto& pair : values) {
        const std::string& key = pair.first;
        const Value& value = pair.second;

        size_t keySize = key.size();
        file.write(reinterpret_cast<const char*>(&keySize), sizeof(keySize));
        file.write(key.c_str(), keySize);

        file.write(reinterpret_cast<const char*>(&value.type), sizeof(value.type));
        switch (value.type) {
        case BOOL:
            file.write(reinterpret_cast<const char*>(&value.boolValue), sizeof(value.boolValue));
            break;
        case INT:
            file.write(reinterpret_cast<const char*>(&value.intValue), sizeof(value.intValue));
            break;
        case FLOAT:
            file.write(reinterpret_cast<const char*>(&value.floatValue), sizeof(value.floatValue));
            break;
        case STRING:
            size_t stringSize;
            stringSize = value.stringValue.size();
            file.write(reinterpret_cast<const char*>(&stringSize), sizeof(stringSize));
            file.write(value.stringValue.c_str(), stringSize);
            break;
        }
    }

    file.close();
}

void Prefs::Load() {
    std::ifstream file(fileName, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for loading: " << fileName << std::endl;
        return;
    }

    while (file) {
        size_t keySize;
        file.read(reinterpret_cast<char*>(&keySize), sizeof(keySize));
        if (file.eof()) break;
        std::string key(keySize, '\0');
        file.read(&key[0], keySize);

        ValueType type;
        file.read(reinterpret_cast<char*>(&type), sizeof(type));

        Value value;
        value.type = type;
        switch (type) {
        case BOOL:
            file.read(reinterpret_cast<char*>(&value.boolValue), sizeof(value.boolValue));
            break;
        case INT:
            file.read(reinterpret_cast<char*>(&value.intValue), sizeof(value.intValue));
            break;
        case FLOAT:
            file.read(reinterpret_cast<char*>(&value.floatValue), sizeof(value.floatValue));
            break;
        case STRING:
            size_t stringSize;
            file.read(reinterpret_cast<char*>(&stringSize), sizeof(stringSize));
            value.stringValue.resize(stringSize);
            file.read(&value.stringValue[0], stringSize);
            break;
        }

        values[key] = value;
    }

    file.close();
}
