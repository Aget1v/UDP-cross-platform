#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <map>

class Config {
public:
    // Конструктор, который принимает имя файла конфигурации
    Config(const std::string& filename);

    // Метод для получения целочисленного значения по ключу
    int getInt(const std::string& key, int defaultValue) const;

    // Метод для получения значения с плавающей точкой по ключу
    double getDouble(const std::string& key, double defaultValue) const;

    // Метод для получения строкового значения по ключу
    std::string getString(const std::string& key, const std::string& defaultValue) const;

private:
    std::map<std::string, std::string> configData; // Хранит пары ключ-значение конфигурации
};

#endif // CONFIG_HPP
