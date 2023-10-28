/*
** EPITECH PROJECT, 2023
** Yiang
** File description:
** GUI.cpp
*/

#include <future>
#include "GUI/GUI.hpp"

enum GameState {
    MENU,
    GAME,
    WELCOMEMENU,
    EXIT
};


GUI::~GUI() {

}

void GUI::load(const GUIConfig& config)
{
    //event
    event = sf::Event();
    //mouse
    mousePos = sf::Vector2i(0, 0);
    mousePosF = sf::Vector2f(0, 0);
}

void GUI::loadingScreenCreate() {
    //this function will create the loading screen
    //create a pair of int for the resolution
    pair<int, int> res;
    res.first = window.getSize().x;
    res.second = window.getSize().y;
    loadingScreen.load(res);

}

void GUI::reloadLoadingScreen() {
    loadingScreen.~LoadingScreen();
    loadingScreenCreate();
}

void GUI::updateLoadingScreen(int percent) {
    loadingScreen.update(percent);
    window.clear();
    loadingScreen.draw(window);
    window.display();
}

void GUI::resetLoadingScreen() {
    loadingScreen.~LoadingScreen();
}

void GUI::displayLoadingScreen() {
    window.clear();
    loadingScreen.draw(window);
    window.display();
}


void GUI::executeLoadingScreenTasks(const std::vector<std::function<void(int&)>>& tasks) {
    // Activate loading screen
    setActive(true);

    int totalTasks = tasks.size();
    int currentTask = 0;

    auto future = std::async(std::launch::async, [&]() {
        setActive(true);
        for (auto& task : tasks) {
            int progressPercentage = (currentTask * 100) / totalTasks;
            updateLoadingScreen(progressPercentage);
            task(progressPercentage);  // Execute the task
            currentTask++;
        }
        updateLoadingScreen(100);  // Loading complete
    });

    // Wait for the loading to finish
    future.wait();

    // Deactivate loading screen
    setActive(false);
}

/*
void GUI::createMenu(Database& database)
{
    //this function will create the menu
    //create a pair of int for the resolution
    pair<int, int> res;
    res.first = window.getSize().x;
    res.second = window.getSize().y;
    menu.load(res, database);
}
 */



void GUI::updateGame(){

}
/*
void GUI::loop(int state)
{
    //for the moment only call the menu update
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || state == EXIT)
                window.close();
            else
                updateState(state, event);
        }
        window.clear();
        drawState(state);
        window.display();
    }

}
*/