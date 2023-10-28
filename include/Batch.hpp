/*
** EPITECH PROJECT, 2024
** ECS
** File description:
** Batch
*/
#ifndef BATCH_HPP_
#define BATCH_HPP_
/*
#include <iostream>
#include "Registry.hpp"
#include "SparseArray.hpp"
#include "Entity.hpp"
#include "TextureManager.hpp"
#include "Behaviour.hpp"

#include <SFML/Graphics.hpp>
#include <vector>

class Batch {
public:
    Batch(const sf::Texture& texture) : texture(texture) {
        vertexArray.setPrimitiveType(sf::Quads);
    }
    void AddEntity(entity_t entity, const sf::Sprite& sprite) {
        // Store the entity ID for reference
        entities.push_back(entity);

        // Get the position and texture coordinates of the sprite
        sf::Vector2f position = sprite.getPosition();
        sf::IntRect textureRect = sprite.getTextureRect();

        // Create 4 vertices for the sprite (quad)
        sf::Vertex topLeft, topRight, bottomLeft, bottomRight;

        // Set positions
        topLeft.position = position;
        topRight.position = {position.x + textureRect.width, position.y};
        bottomLeft.position = {position.x, position.y + textureRect.height};
        bottomRight.position = {position.x + textureRect.width, position.y + textureRect.height};

        // Set texture coordinates
        topLeft.texCoords = {static_cast<float>(textureRect.left), static_cast<float>(textureRect.top)};
        topRight.texCoords = {static_cast<float>(textureRect.left + textureRect.width), static_cast<float>(textureRect.top)};
        bottomLeft.texCoords = {static_cast<float>(textureRect.left), static_cast<float>(textureRect.top + textureRect.height)};
        bottomRight.texCoords = {static_cast<float>(textureRect.left + textureRect.width), static_cast<float>(textureRect.top + textureRect.height)};

        // Add the vertices to the vertex array
        vertexArray.append(topLeft);
        vertexArray.append(topRight);
        vertexArray.append(bottomLeft);
        vertexArray.append(bottomRight);
        std::cout << "Added entity " << entity << " to batch." << std::endl;

    }


    void RemoveEntity(entity_t entity) {
        // Find the entity in the entities list
        auto it = std::find(entities.begin(), entities.end(), entity);

        if (it != entities.end()) {
            // Calculate the index in the vertex array
            std::size_t index = std::distance(entities.begin(), it) * 4;  // Each entity contributes 4 vertices

            // Create a new vertex array and copy over all vertices except the ones to remove
            sf::VertexArray newVertexArray;
            for (std::size_t i = 0; i < vertexArray.getVertexCount(); ++i) {
                if (i < index || i >= index + 4) {
                    newVertexArray.append(vertexArray[i]);
                }
            }

            // Replace the old vertex array with the new one
            vertexArray = newVertexArray;

            // Remove the entity from the list
            entities.erase(it);
        } else {
            // Entity not found in the batch
            std::cerr << "Warning: Entity not found in batch. Cannot remove." << std::endl;
        }
    }



    void UpdateEntity(entity_t entity, const sf::Sprite& newSprite) {
        // Find the entity in the entities list
        auto it = std::find(entities.begin(), entities.end(), entity);

        if (it != entities.end()) {
            // Calculate the index in the vertex array
            std::size_t index = std::distance(entities.begin(), it) * 4;  // Each entity contributes 4 vertices

            // Get the position and texture coordinates of the new sprite
            sf::Vector2f position = newSprite.getPosition();
            sf::IntRect textureRect = newSprite.getTextureRect();

            // Update positions
            vertexArray[index].position = position;
            vertexArray[index + 1].position = {position.x + textureRect.width, position.y};
            vertexArray[index + 2].position = {position.x, position.y + textureRect.height};
            vertexArray[index + 3].position = {position.x + textureRect.width, position.y + textureRect.height};

            // Update texture coordinates
            vertexArray[index].texCoords = {static_cast<float>(textureRect.left), static_cast<float>(textureRect.top)};
            vertexArray[index + 1].texCoords = {static_cast<float>(textureRect.left + textureRect.width), static_cast<float>(textureRect.top)};
            vertexArray[index + 2].texCoords = {static_cast<float>(textureRect.left), static_cast<float>(textureRect.top + textureRect.height)};
            vertexArray[index + 3].texCoords = {static_cast<float>(textureRect.left + textureRect.width), static_cast<float>(textureRect.top + textureRect.height)};
        } else {
            // Entity not found in the batch
            std::cerr << "Warning: Entity not found in batch. Cannot update." << std::endl;
        }
    }


    void Draw(sf::RenderWindow& window) {
        window.draw(vertexArray);
    }
    void Clear() {
        entities.clear();
        vertexArray.clear();
    }


private:
    const sf::Texture& texture;
    sf::VertexArray vertexArray;
    std::vector<entity_t> entities;  // Store entity IDs for reference
};


class BatchManager {
public:
    BatchManager() = default;

    void AddBatch(const std::string& textureName, const sf::Texture& texture) {
        batches[textureName] = std::make_unique<Batch>(texture);
    }

    Batch* GetBatch(const std::string& textureName) {
        return batches[textureName].get();
    }

    void DrawAll(sf::RenderWindow& window) {
        for (auto& [textureName, batch] : batches) {
            batch->Draw(window);
        }
    }

    void ClearAll() {
        for (auto& [textureName, batch] : batches) {
            batch->Clear();
        }
    }

private:
    std::unordered_map<std::string, std::unique_ptr<Batch>> batches;
};
*/
#endif /* !BATCH_HPP_ */
