/*
** EPITECH PROJECT, 2024
** ECS
** File description:
** QUADTREE
*/
#ifndef QUADTREE_HPP_
#define QUADTREE_HPP_
#include "ISystem.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <memory>
#include <utility>
#include <functional>
#include <fstream>
#include <chrono>
#include <fftw3.h>

#include <cmath>
#include <thread>
#include <chrono>
#include <complex>
#include <valarray>
#include <unordered_map>
#include <algorithm>  // for std::transform
#include <iterator>  // for std::back_inserter
#include <unordered_set>
#include <future>  // for std::async
#include <array>
#include <map>
#include <set>


class CollisionRectangle {
public:
    CollisionRectangle(float x, float y, float width, float height)
            : x(x), y(y), width(width), height(height) {}

    bool contains(float x, float y, float width, float height) const {
        return !(x < this->x || x + width > this->x + this->width ||
                 y < this->y || y + height > this->y + this->height);
    }
    bool intersects(const CollisionRectangle& other) const {
        return !(x > other.x + other.width ||
                 x + width < other.x ||
                 y > other.y + other.height ||
                 y + height < other.y);
    }

    float getX() const {
        return x;
    }

public:
    float x, y, width, height;
};



class Quadtree {
private:
    // Define Node as a nested class
    class Node {
    public:
        CollisionRectangle boundary;
        int capacity;
        std::vector<entity_t> entities;
        std::unique_ptr<Node> northwest;
        std::unique_ptr<Node> northeast;
        std::unique_ptr<Node> southwest;
        std::unique_ptr<Node> southeast;

        Node(CollisionRectangle boundary, int capacity)
                : boundary(boundary), capacity(capacity) {}

        void insert(entity_t entity, float x, float y, float width, float height);
        void subdivide();
        void queryRange(CollisionRectangle range, std::vector<entity_t>& foundEntities, sparse_array<Position>& positions, sparse_array<Drawable>& drawables);
    };

    std::unique_ptr<Node> root;

public:
    Quadtree(CollisionRectangle boundary, int capacity)
            : root(std::make_unique<Node>(boundary, capacity)) {}

    void insert(entity_t entity, float x, float y, float width, float height) {
        root->insert(entity, x, y, width, height);
    }

    std::vector<entity_t> queryRange(CollisionRectangle range, sparse_array<Position>& positions, sparse_array<Drawable>& drawables) {
        std::vector<entity_t> foundEntities;
        root->queryRange(range, foundEntities, positions, drawables);
        return foundEntities;
    }
    void clear() {
        root = std::make_unique<Node>(root->boundary, root->capacity);
    }

};

class CollisionSystem : public ISystem<Position, Drawable, Collision> {
private:
    Registry& registry;
    Quadtree quadtree;
    bool firstRun = true;

public:
    CollisionSystem(Registry& registry)
            : registry(registry), quadtree(CollisionRectangle(0, 0, 1920, 1080), 4) {}

    void operator()(entity_t entity, sparse_array<Position>& positions, sparse_array<Drawable>& drawables, sparse_array<Collision>& collisions) override {
        // Initialize the quadtree if it's the first run
        if (firstRun) {
            for (entity_t e = 0; e < positions.size(); ++e) {
                if (positions[e].has_value() && collisions[e].has_value()) {
                    auto& pos = *positions[e];
                    float width = 0, height = 0;
                    if (drawables[e]) {
                        auto& drawable = *drawables[e];
                        width = drawable.textureRect.width;
                        height = drawable.textureRect.height;
                    }

                    quadtree.insert(e, pos.x, pos.y, width, height);
                }
            }
            firstRun = false;
        }

        // Skip entities that are not collidable
        if (!(positions[entity] && collisions[entity])) {
            return;
        }

        auto& pos = *positions[entity];
        auto& col = *collisions[entity];
        float width = 0, height = 0;

        if (drawables[entity]) {
            auto& drawable = *drawables[entity];
            width = drawable.textureRect.width;  // Apply scaling factor
            height = drawable.textureRect.height;  // Apply scaling factor
        }


        // Query the quadtree for entities that might collide with this one
        auto nearbyEntities = quadtree.queryRange(CollisionRectangle(pos.x, pos.y, width, height), positions, drawables);

        // Loop through the nearby entities and check for collisions
        for (const auto& otherEntity : nearbyEntities) {
            if (otherEntity == entity) continue;  // Skip self-collision

            auto& otherPos = *positions[otherEntity];
            float otherWidth = 0, otherHeight = 0;
            auto& otherCollisionInfo = *collisions[otherEntity];
            if (otherCollisionInfo.isEnemy == col.isEnemy) continue;  // Same team, for example
            if (drawables[otherEntity]) {
                auto& otherDrawable = *drawables[otherEntity];
                otherWidth = otherDrawable.textureRect.width;  // Apply scaling factor
                otherHeight = otherDrawable.textureRect.height;  // Apply scaling factor
            }

            // Here, perform your precise collision detection between 'entity' and 'otherEntity'
            // If collision detected, handle it accordingly we will just make a print for the example
            //if (laserPos.x >= enemyPos.x && laserPos.x <= enemyPos.x + 243 && laserPos.y >= enemyPos.y && laserPos.y <= enemyPos.y + 140) {
            if (pos.x < otherPos.x + otherWidth &&
                pos.x + width > otherPos.x &&
                pos.y < otherPos.y + otherHeight &&
                pos.y + height > otherPos.y) {
                registry.queue_for_deletion(entity);
                registry.queue_for_deletion(otherEntity);
             //   std::cout << "Collision detected between " << entity << " and " << otherEntity << std::endl;
            }
        }
    }


    void PostUpdate() override {
        // Reset the quadtree and invalidate the firstRun flag
        quadtree.clear();
        firstRun = true;
    }
};


// Node::insert method
void Quadtree::Node::insert(entity_t entity, float x, float y, float width, float height) {
    if (!boundary.contains(x, y, width, height)) {
        return;
    }

    if (entities.size() < capacity) {
        entities.push_back(entity);
        return;
    }

    if (!northwest) {
        subdivide();
    }

    northwest->insert(entity, x, y, width, height);
    northeast->insert(entity, x, y, width, height);
    southwest->insert(entity, x, y, width, height);
    southeast->insert(entity, x, y, width, height);
}

// Node::subdivide method
void Quadtree::Node::subdivide() {
    int subWidth = boundary.width / 2;
    int subHeight = boundary.height / 2;
    int x = boundary.x;
    int y = boundary.y;

    northwest = std::make_unique<Node>(CollisionRectangle(x, y, subWidth, subHeight), capacity);
    northeast = std::make_unique<Node>(CollisionRectangle(x + subWidth, y, subWidth, subHeight), capacity);
    southwest = std::make_unique<Node>(CollisionRectangle(x, y + subHeight, subWidth, subHeight), capacity);
    southeast = std::make_unique<Node>(CollisionRectangle(x + subWidth, y + subHeight, subWidth, subHeight), capacity);
}

// Node::queryRange method
// Node::queryRange method
void Quadtree::Node::queryRange(CollisionRectangle range, std::vector<entity_t>& foundEntities, sparse_array<Position>& positions, sparse_array<Drawable>& drawables) {
    if (!boundary.intersects(range)) {
        return;
    }

    for (const auto& entity : entities) {
        // Check if the entity has a Position and Drawable component
        if (positions[entity] && drawables[entity]) {
            auto& pos = *positions[entity];
            auto& drawable = *drawables[entity];
            float x = pos.x, y = pos.y;
            float width = drawable.textureRect.width, height = drawable.textureRect.height;

            // Now, call the contains method with correct arguments
            if (range.contains(x, y, width, height)) {
                foundEntities.push_back(entity);
            }
        }
    }

    if (!northwest) {
        return;
    }

    northwest->queryRange(range, foundEntities, positions, drawables);
    northeast->queryRange(range, foundEntities, positions, drawables);
    southwest->queryRange(range, foundEntities, positions, drawables);
    southeast->queryRange(range, foundEntities, positions, drawables);
}
#endif /* !QUADTREE_HPP_ */
