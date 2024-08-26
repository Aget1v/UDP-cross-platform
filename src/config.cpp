#include "config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

Config::Config(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open configuration file: " << filename << std::endl;
        throw std::runtime_error("Could not open config file");
    }
    
    std::string line;
    std::string section;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue; // Игнорируем комментарии и пустые строки
        }
        if (line[0] == '[' && line.back() == ']') {
            section = line.substr(1, line.size() - 2);
        } else {
            auto delimiterPos = line.find('=');
            if (delimiterPos != std::string::npos) {
                std::string key = trim(line.substr(0, delimiterPos));
                std::string value = trim(line.substr(delimiterPos + 1));
                data[section + "." + key] = value;
            }
        }
    }
    file.close();
}

std::string Config::get(const std::string& key, const std::string& default_value) const {
    auto it = data.find(key);
    if (it != data.end()) {
        return it->second;
    }
    return default_value;
}

int Config::getInt(const std::string& key, int default_value) const {
    return std::stoi(get(key, std::to_string(default_value)));
}

double Config::getDouble(const std::string& key, double default_value) const {
    return std::stod(get(key, std::to_string(default_value)));
}

std::string Config::trim(const std::string& str) {
    const char* whitespace = " \t\n\r";
    size_t start = str.find_first_not_of(whitespace);
    size_t end = str.find_last_not_of(whitespace);
    return (start == std::string::npos || end == std::string::npos) ? "" : str.substr(start, end - start + 1);
}
