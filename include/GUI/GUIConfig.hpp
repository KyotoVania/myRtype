/*
** EPITECH PROJECT, 2023
** Yiang
** File description:
** GUIConfig.hpp
*/
#ifndef GUICONFIG_HPP_
	#define GUICONFIG_HPP_

#include <string>
#include <libconfig.h++>
#include <iostream>
using namespace std;

struct themeData {
    int id;
    string name;
};

class GUIConfig {
	public:
		GUIConfig();
		~GUIConfig();
        void setResolution(pair<int, int> res);
        void setTitle(string title);
        void setFPS(int fps);
        void setTheme(themeData theme);
        pair<int, int> getResolution() const;
        string getTitle() const;
        int getFPS() const;
        themeData getTheme() const;
        void load(const libconfig::Setting& guiSetting);
        void print();

        libconfig::Setting& getConfig(const std::string& file) {
            libconfig::Config cfg;

            // Read the file. If there is an error, report it and exit.
            try {
                cfg.readFile(file.c_str());
            }
            catch(const libconfig::FileIOException &fioex) {
                std::cerr << "I/O error while reading file." << std::endl;
            }
            catch(const libconfig::ParseException &pex) {
                std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
                          << " - " << pex.getError() << std::endl;
            }

            // Get the GUI configuration
            libconfig::Setting& gui = cfg.lookup("GUI");
            std::cout << "GUI: " << gui.getLength() << std::endl;
            return gui;
        }

	protected:
    pair<int, int> res;
    string title;
    int fps;
    themeData theme;
	private:
};

#endif /*GUICONFIG_HPP_*/