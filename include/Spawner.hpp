/*
** EPITECH PROJECT, 2024
** ECS
** File description:
** Spawner
*/
#ifndef SPAWNER_HPP_
#define SPAWNER_HPP_

#include <iostream>
#include "Registry.hpp"
#include "Component.hpp"
#include "SparseArray.hpp"
#include "Entity.hpp"
#include "TextureManager.hpp"
#include "Behaviour.hpp"

void spawnEnemy(Registry &registry, Behavior behavior, float x, float y, const std::string &textureName,
                const MyRect &textureRect, const MyVector2 &scale);

entity_t spawnBackgroundLayer(Registry &registry, Behavior behavior, const std::string &textureName, float velocityX,
                              float velocityY, const MyRect &textureRect, const MyVector2 &scale);

entity_t spawnPlayer(Registry &registry, const MyRect &rect);

entity_t
spawnAnimatedEntity(Registry &registry, const MyRect &rect, const MyVector2 &scale, const std::string &textureName,
                    float posX, float posY);

entity_t spawnGameConfig(Registry &registry);

entity_t spawnThruster(Registry &registry, entity_t parentEntity);

entity_t spawnNoneLocalPlayer(Registry &registry, const MyRect &rect);

entity_t spawnServerEntity(Registry &registry);

entity_t spawnClientEntity(Registry &registry);

entity_t spawnBeatmapPanel(Registry &registry,
                           Database &database,
                           const int &index);

entity_t
FFTSpawnEnemy(Registry &registry, const std::string &behavior_id, float x, float y, const std::string &textureName,
              const MyRect &textureRect, const MyVector2 &scale);

entity_t spawnButton(Registry &registry, const MyRect &rect, const std::string &textureName, const std::string &text,
                     const std::string &fontName, const MyVector2 &scale, const MyVector2 &textOffset,
                     const MyVector2 &textScale, std::function<void()> onClick, const MyVector2 &position);

entity_t spawnState(Registry &registry);

entity_t spawnFollowerCover(entity_t parent_entity, Registry &registry, const MyRect &rect, const string &textureName,
                            const MyVector2 &scale);

entity_t spawnFollowerText(entity_t parent_entity, Registry &registry, const std::string &Title, const float Xoffset,
                           const float Yoffset);

entity_t spawnDrawable(Registry &registry, const MyRect &rect, const std::string &textureName, const MyVector2 &scale,
                       float posX, float posY, float depth);

#endif /* !SPAWNER_HPP_ */