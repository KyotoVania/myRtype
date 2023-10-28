
#include "ConfigServer.hpp"
#include "Global.hpp"
#include <fstream>
#include <iostream>

void loadConfig(const std::string& filePath) {
    std::ifstream file(filePath);
    std::string line;
    std::cout << "Loading config file: " << filePath << std::endl;
    if (!file.is_open()) {
        std::cout << "Failed to open config file: " << filePath << std::endl;
        return;
    }
    while (std::getline(file, line)) {
        if (line[0] == '#') continue;  // Skip comments
        std::istringstream is_line(line);
        std::string key;
        if (std::getline(is_line, key, '=')) {
            std::string value;
            if (std::getline(is_line, value)) {
                configMap[key] = value;
            }
        }
    }
    for (auto& it : configMap) {
        std::cout << it.first << " = " << it.second << std::endl;
    }
}

std::string readFromConfig(const std::string& key) {
    auto it = configMap.find(key);
    if (it != configMap.end()) {
        return it->second;
    }
    return "";  // Key not found
}