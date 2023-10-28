#include "SpritePool.hpp"

sf::Sprite* SpritePool::getSprite() {
    if (available.empty()) {
        available.push_back(new sf::Sprite());
    }
    sf::Sprite* sprite = available.front();
    inUse.push_back(sprite);
    available.pop_front();
    return sprite;
}

void SpritePool::returnSprite(sf::Sprite* sprite) {
    inUse.remove(sprite);
    available.push_back(sprite);
}
