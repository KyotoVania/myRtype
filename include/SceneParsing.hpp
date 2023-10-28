/*
** EPITECH PROJECT, 2024
** ECS
** File description:
** SceneParsing
*/
#ifndef SCENEPARSING_HPP_
#define SCENEPARSING_HPP_

#include <libconfig.h++>
#include "Component.hpp"
void addPosition(entity_t entity, Registry& registry, const libconfig::Setting& componentData);
void addVelocity(entity_t entity, Registry& registry, const libconfig::Setting& componentData);
void addCollision(entity_t entity, Registry& registry, const libconfig::Setting& componentData);
void addDrawable(entity_t entity, Registry& registry, const libconfig::Setting& componentData);
void addAcceleration(entity_t entity, Registry& registry, const libconfig::Setting& componentData);
void addAnimation(entity_t entity, Registry& registry, const libconfig::Setting& componentData);
void addGameConfig(entity_t entity, Registry& registry, const libconfig::Setting& componentData);
void addControllable(entity_t entity, Registry& registry, const libconfig::Setting& componentData);
void addLaser(entity_t entity, Registry& registry, const libconfig::Setting& componentData);
void addDepth(entity_t entity, Registry& registry, const libconfig::Setting& componentData);
void addBehavior(entity_t entity, Registry& registry, const libconfig::Setting& componentData);
void addFFT(entity_t entity, Registry& registry, const libconfig::Setting& componentData);
void addSpawner(entity_t entity, Registry& registry, const libconfig::Setting& componentData);
void addFollower(entity_t entity, Registry& registry, const libconfig::Setting& componentData);
void initializeGameEntitiesFromConfig(Registry &registry, const std::string &configFilePath);
#endif /* !SCENEPARSING_HPP_ */