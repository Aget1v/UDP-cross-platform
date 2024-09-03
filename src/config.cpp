#include "config.hpp"
#include <fstream>
#include <sstream>

Config::Config(const std::string& filename) {
    std::ifstream file(filename);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream lineStream(line);
            std::string key, value;
            if (std::getline(lineStream, key, '=') && std::getline(lineStream, value)) {
                configData[key] = value;
            }
        }
        file.close();
    }
}

int Config::getInt(const std::string& key, int defaultValue) const {
    auto it = configData.find(key);
    if (it != configData.end()) {
        return std::stoi(it->second);
    }
    return defaultValue;
}

double Config::getDouble(const std::string& key, double defaultValue) const {
    auto it = configData.find(key);
    if (it != configData.end()) {
        return std::stod(it->second);
    }
    return defaultValue;
}

std::string Config::getString(const std::string& key, const std::string& defaultValue) const {
    auto it = configData.find(key);
    if (it != configData.end()) {
        return it->second;
    }
    return defaultValue;
}
