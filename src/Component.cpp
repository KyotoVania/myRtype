#include "Behaviour.hpp"
#include "Component.hpp"
#include "Registry.hpp"
#include "TextureManager.hpp"
#include "System.hpp"
#include "Quadtree.hpp"
#include "MenuSystem.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

#include <memory>

void registerServerSystem(Registry& registry, sf::RenderWindow& window, AssetManager& textureManager, float &elapsed_time) {
    std::cout << "deltaTime: " << elapsed_time << std::endl;
    registry.add_system<DrawSystem, Drawable, Position, Depth, TextDrawable>(std::make_unique<DrawSystem>(window, textureManager, registry));
    registry.add_system<FFTSystem, FFT>(std::make_unique<FFTSystem>(elapsed_time, registry));
    registry.add_system<ServerSystem, Network>(std::make_unique<ServerSystem>(registry, elapsed_time));
    registry.add_system<ControlSystem, Position, Velocity, Controllable, GameConfig>(std::make_unique<ControlSystem>(elapsed_time, registry));
    registry.add_system<AnimationSystem, Animation, Drawable>(std::make_unique<AnimationSystem>(elapsed_time));
    registry.add_system<VelocitySystem, Position, Velocity>(std::make_unique<VelocitySystem>(elapsed_time));
    registry.add_system<SpawnSystem, Spawner, Position>(std::make_unique<SpawnSystem>(elapsed_time, textureManager, registry));
    registry.add_system<CollisionSystem, Position, Drawable, Collision>(std::make_unique<CollisionSystem>(registry));
    registry.add_system<BehaviorSystem, Behavior, Position>(std::make_unique<BehaviorSystem>(elapsed_time));
    registry.add_system<FollowerSystem, Follower, Position>(std::make_unique<FollowerSystem>());
    registry.add_system<ClickSystem, isClickable, Position>(std::make_unique<ClickSystem>(window, registry, elapsed_time));
    registry.add_system<MenuSystem, State>(std::make_unique<MenuSystem>(registry, textureManager));
}

void registerClientSystem(Registry& registry, sf::RenderWindow& window, AssetManager& textureManager, float &elapsed_time)
{
    std::cout << "deltaTime: " << elapsed_time << std::endl;
    registry.add_system<ClientSystem, State>(std::make_unique<ClientSystem>(registry, elapsed_time));
    registry.add_system<DrawSystem, Drawable, Position, Depth, TextDrawable>(std::make_unique<DrawSystem>(window, textureManager, registry));
    registry.add_system<AnimationSystem, Animation, Drawable>(std::make_unique<AnimationSystem>(elapsed_time));
    registry.add_system<VelocitySystem, Position, Velocity>(std::make_unique<VelocitySystem>(elapsed_time));
    registry.add_system<SpawnSystem, Spawner, Position>(std::make_unique<SpawnSystem>(elapsed_time, textureManager, registry));
    registry.add_system<BehaviorSystem, Behavior, Position>(std::make_unique<BehaviorSystem>(elapsed_time));
    registry.add_system<FollowerSystem, Follower, Position>(std::make_unique<FollowerSystem>());
    registry.add_system<ClickSystem, isClickable, Position>(std::make_unique<ClickSystem>(window, registry, elapsed_time));
    registry.add_system<MenuSystem, State>(std::make_unique<MenuSystem>(registry, textureManager));
}

void cleanup_system(Registry& registry) {
    // Define screen dimensions
    auto& positionComponents = registry.get_components<Position>();
    auto& laserComponents = registry.get_components<Laser>();
    auto& enemyComponents = registry.get_components<Enemy>();
    auto& gameConfigComponents = registry.get_components<GameConfig>();
    GameConfig config{1920,1080,100};
    if (gameConfigComponents.empty()) {
    } else {
        auto &config = *gameConfigComponents[0];
    }
    int margin = 100;  // Additional pixels to keep entities in memory

    // Note: Assuming that all entities have a Position component.
    for (entity_t entity = 0; entity < positionComponents.size(); ++entity) {
        // Check if the entity has a position
        if (auto posOpt = positionComponents[entity]) {
            // Check if the entity is out of screen bounds with margin
            if (posOpt->x > config.screenWidth + margin || posOpt->y > config.screenHeight + margin ||
                posOpt->x < + margin || posOpt->y < -margin ) {

                // For lasers
                if (laserComponents.contains(entity)) {
                    registry.queue_for_deletion(entity);
                }

                // For enemies or any other entity type
                if (enemyComponents.contains(entity)) {
                //    std::cout << "Enemy destroyed" << std::endl;
                    registry.queue_for_deletion(entity);
                }

                // Add more conditions for other types of entities here
            }
        }
    }
}



void register_all_components(Registry& registry) {
    registry.register_component<Position>();
    registry.register_component<Velocity>();
    registry.register_component<Drawable>();
    registry.register_component<Controllable>();
    registry.register_component<Acceleration>();
    registry.register_component<Spawner>();
    registry.register_component<Laser>();
    registry.register_component<Depth>();
    registry.register_component<Behavior>();
    registry.register_component<Enemy>();
    registry.register_component<GameConfig>();
    registry.register_component<Health>();
    registry.register_component<FFT>();
    registry.register_component<Animation>();
    registry.register_component<Follower>();
    registry.register_component<Collision>();
    registry.register_component<Network>();
    registry.register_component<State>();
    registry.register_component<Client>();
    registry.register_component<UserInput>();
    registry.register_component<isClickable>();
    registry.register_component<TextDrawable>();
}

