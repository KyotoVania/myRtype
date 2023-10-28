/*
** EPITECH PROJECT, 2024
** ECS
** File description:
** sprite pool
*/
#ifndef SPRITEPOOL_HPP_
#define SPRITEPOOL_HPP_
#include <list>
#include <SFML/Graphics.hpp>

class SpritePool {
public:
    sf::Sprite* getSprite();
    void returnSprite(sf::Sprite* sprite);

private:
    std::list<sf::Sprite*> available;
    std::list<sf::Sprite*> inUse;
};
#endif /* !SPRITEPOOL_HPP_ */