/*
** EPITECH PROJECT, 2023
** Yiang
** File description:
** Config.hpp
*/
#ifndef CONFIG_HPP_
	#define CONFIG_HPP_
#include <string>
#include <map>
#include <libconfig.h++>
#include <iostream>
#include "GUI/GUIConfig.hpp"
#include "Beatmap/BeatmapConfig.hpp"
using namespace std;

class Config {
public:
    Config();
    ~Config();
    void load(const std::string& file);
    void save(const std::string& file);
    void print();
    GUIConfig getGUIConfig() const;
private:
    // Static members to store the configuration values
    string path;
    GUIConfig guiConfig;
};

#endif /*CONFIG_HPP_*/

