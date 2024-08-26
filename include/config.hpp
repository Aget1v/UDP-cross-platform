#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <unordered_map>

class Config {
public:
    Config(const std::string& filename);
    std::string get(const std::string& key, const std::string& default_value = "") const;
    int getInt(const std::string& key, int default_value = 0) const;
    double getDouble(const std::string& key, double default_value = 0.0) const;

private:
    std::unordered_map<std::string, std::string> data;
    static std::string trim(const std::string& str);
};

#endif // CONFIG_HPP
