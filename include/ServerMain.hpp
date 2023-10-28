/*
** EPITECH PROJECT, 2024
** ECS
** File description:
** serverMain
*/

#ifndef SERVERMAIN_HPP_
#define SERVERMAIN_HPP_
#include "Component.hpp"
#include "SparseArray.hpp"
#include "GUI/GUI.hpp"
#include "Spawner.hpp"
#include "TextureManager.hpp"
#include "Global.hpp"
#include "Database/Database.hpp"
#include "Config/Config.hpp"
#include "taskLoader.hpp"

void initializeGameEntitiesServer(Registry& registry);
void initializeGameServerSystems(Registry& registry, sf::RenderWindow& window,  AssetManager& assetManager,  float& elapsed_time);
void initializeDatabaseAndConfig(Config& config, Database& database, GUI& gui);
void firstLoadingScreen(GUI& gui, Database& database, Registry& registry,sf::RenderWindow& window);
void serverLoop(Registry& registry, sf::RenderWindow& window, sf::Clock& clock, float& elapsed_time, GUI& gui, Database& database);

extern std::unordered_map<std::string, ComponentAdder> componentAdderMap;
#endif /* !SERVERMAIN_HPP_ */