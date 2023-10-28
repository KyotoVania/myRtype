
#include "Spawner.hpp"
#include "Global.hpp"


void spawnEnemy(Registry &registry, Behavior behavior, float x, float y, const std::string &textureName,
                const MyRect &textureRect, const MyVector2 &scale) {
    auto enemy = registry.spawn_entity();
    registry.add_component(enemy, Position{x, y});
    registry.add_component(enemy, Collision{true});
    registry.add_component(enemy, Enemy{});
    registry.add_component(enemy, Behavior{std::move(behavior)});
    Drawable drawable;
    drawable.textureName = textureName;
    drawable.textureRect = textureRect;
    drawable.scale = scale;
    registry.add_component(enemy, std::move(drawable));
    registry.add_component(enemy, Depth{2.0f});
    //std::cout << "Enemy spawned at " << x << ", " << y << std::endl;
    //std::cout << "Enemy id: " << enemy << std::endl;
}

entity_t
FFTSpawnEnemy(Registry &registry, const std::string &behavior_id, float x, float y, const std::string &textureName,
              const MyRect &textureRect, const MyVector2 &scale) {
    auto enemy = registry.queue_for_creation([=](Registry &registry, entity_t enemy) {
        registry.add_component(enemy, Position{x, y});
        registry.add_component(enemy, Collision{true});
        registry.add_component(enemy, Enemy{});
        registry.add_component(enemy, Behavior{behavior_id});
        Drawable drawable;
        drawable.textureName = textureName;
        drawable.textureRect = textureRect;
        drawable.scale = scale;

        registry.add_component(enemy, std::move(drawable));
        registry.add_component(enemy, Depth{2.0f});
        //std::cout << "Enemy spawned at " << x << ", " << y << std::endl;
        //  std::cout << "Enemy id: " << enemy << std::endl;
        /*
          ComponentAdder addLaser = [](entity_t entity, Registry& registry) {
              registry.add_component(entity, Laser{});
          };

          ComponentAdder addDepth = [](entity_t entity, Registry& registry) {
              registry.add_component(entity, Depth{3.0f});
          };

          ComponentAdder addDrawable = [](entity_t entity, Registry& registry) {
              registry.add_component(entity, Drawable{"laser", {0, 0, 80, 60}, {1.0f, 1.0f}});
          };

          ComponentAdder addCollision = [](entity_t entity, Registry& registry) {
              registry.add_component(entity, Collision{true});
          };

          ComponentAdder addVelocity = [](entity_t entity, Registry& registry) {
              registry.add_component(entity, Velocity{-10,0});
          };


          Spawner playerSpawner{
                  .spawnRate = 0.2,
                  .timeSinceLastSpawn = 0.0f,
                  .entityToSpawn = "laser",
                  .xOffset = 0,
                  .yOffset = 0.0f,
                  .xVelocity = 1.0f,
                  .yVelocity = 0.0f,
                  .componentAdders = {addLaser, addDepth, addDrawable, addCollision, addVelocity}
                registry.add_component(enemy, Spawner{std::move(playerSpawner)});

          };
  */
    });
    return enemy;
}

entity_t spawnBackgroundLayer(Registry &registry, Behavior behavior, const std::string &textureName, float velocityX,
                              float velocityY, const MyRect &textureRect, const MyVector2 &scale) {

    auto entity = registry.spawn_entity();
    Drawable drawableComponent;
    drawableComponent.textureName = textureName;
    drawableComponent.textureRect = textureRect;
    drawableComponent.scale = scale;
    registry.add_component(entity, Position{0, 0});
    registry.add_component(entity, std::move(drawableComponent));
    registry.add_component(entity, Depth{0.0f});
    registry.add_component(entity, Behavior{std::move(behavior)});

    return entity;
}


entity_t spawnThruster(Registry &registry, entity_t parentEntity, float xOffset, float yOffset) {
    auto parentPos = registry.get_components<Position>();
    if (!parentPos[parentEntity].has_value()) {
        std::cerr << "Parent entity does not have a Position component" << std::endl;
        return -1;
    }
    // Initialize thruster entity and components
    auto thrusterEntity = registry.spawn_entity();
    registry.add_component(thrusterEntity, Position{parentPos[parentEntity]->x, parentPos[parentEntity]->y});
    registry.add_component(thrusterEntity, Drawable{"p_thruster", {0, 0, 48, 48}, {1.0f, 1.0f}});
    registry.add_component(thrusterEntity, Animation{0, 3, 1.0f / FRAMERATE, 0.0f, true});
    registry.add_component(thrusterEntity, Follower{parentEntity, xOffset,
                                                    yOffset}); // Custom component to indicate this entity follows another
    registry.add_component(thrusterEntity, Depth{2.0f});

    return thrusterEntity;
}

entity_t spawnPlayer(Registry &registry, const MyRect &rect) {
    Drawable drawableComponent;
    drawableComponent.textureName = "p_spaceship";
    drawableComponent.textureRect = rect;
    auto entity = registry.spawn_entity();
    registry.add_component(entity, Position{400, 500});
    registry.add_component(entity, Velocity{0, 0});
    registry.add_component(entity, Acceleration{0, 0});
    registry.add_component(entity, Controllable{true});
    registry.add_component(entity, GameConfig{1920, 1080, 100});
    //ADD THE COMPONENT IN THE SPAWNER

/*
    ComponentAdder addLaser = [](entity_t entity, Registry& registry) {
        registry.add_component(entity, Laser{});
    };

    ComponentAdder addDepth = [](entity_t entity, Registry& registry) {
        registry.add_component(entity, Depth{3.0f});
    };

    ComponentAdder addDrawable = [](entity_t entity, Registry& registry) {
        registry.add_component(entity, Drawable{"laserAnimated", {0, 0, 64, 64}, {1.0f, 1.0f}});
    };
    ComponentAdder addAnimated = [](entity_t entity, Registry& registry) {
        registry.add_component(entity, Animation{0, 5, 3.0f/FRAMERATE, 0.0f, false});
    };

    ComponentAdder addCollision = [](entity_t entity, Registry& registry) {
        registry.add_component(entity, Collision{});
    };

    ComponentAdder addVelocity = [](entity_t entity, Registry& registry) {
        registry.add_component(entity, Velocity{10,0});
    };


    Spawner playerSpawner{
            .spawnRate = 5,
            .timeSinceLastSpawn = 0.0f,
            .entityToSpawn = "laserAnimated",
            .xOffset = 200,
            .yOffset = 0.0f,
            .xVelocity = 1.0f,
            .yVelocity = 0.0f,
            .componentAdders = {addLaser, addDepth, addDrawable, addCollision, addVelocity, addAnimated}
    };

    registry.add_component(entity, Spawner{std::move(playerSpawner)});
    */
    registry.add_component(entity, std::move(drawableComponent));
    registry.add_component(entity, Depth{3.0f});
    registry.add_component(entity, Health{100.0f});
    registry.add_component(entity, FFT{});
    spawnThruster(registry, entity, 0, 10);
    std::cout << "PLAYER SPAWER ADDED" << std::endl;
    return entity;
}

entity_t
spawnAnimatedEntity(Registry &registry, const MyRect &rect, const MyVector2 &scale, const std::string &textureName,
                    float posX, float posY) {
    Drawable drawableComponent;
    drawableComponent.textureName = textureName;
    drawableComponent.textureRect = rect;
    drawableComponent.scale = scale;

//    drawableComponent.textureRect = {0, 0, 126, 126};
    auto entity = registry.spawn_entity();
    registry.add_component(entity, Position{posX, posY});
    registry.add_component(entity, Velocity{0, 0});
    registry.add_component(entity, Acceleration{0, 0});
    registry.add_component(entity, std::move(drawableComponent));
    registry.add_component(entity, Depth{1.0f});
    registry.add_component(entity, Animation{0, 100, 1.0f / FRAMERATE, 0.0f, true});
    return entity;
}

entity_t spawnState(Registry &registry) {
    auto entity = registry.spawn_entity();
    registry.add_component(entity, State{});
    return entity;
}


entity_t spawnServerEntity(Registry &registry) {
    std::cout << "Spawning ServerEntity..." << std::endl;
    auto entity = registry.spawn_entity();
    registry.add_component(entity, Network{});
    return entity;
}

entity_t spawnClientEntity(Registry &registry) {
    std::cout << "Spawning ClientEntity..." << std::endl;
    auto entity = registry.spawn_entity();
    registry.add_component(entity, State{});
    return entity;
}

entity_t spawnNoneLocalPlayer(Registry &registry, const MyRect &rect) {
    Drawable drawableComponent;
    drawableComponent.textureName = "p_spaceship";
    drawableComponent.textureRect = rect;
    auto entity = registry.spawn_entity();
    registry.add_component(entity, Position{400, 500});
    registry.add_component(entity, Velocity{0, 0});
    registry.add_component(entity, Acceleration{0, 0});
    registry.add_component(entity, Controllable{false});
    registry.add_component(entity, GameConfig{1920, 1080, 100});
    registry.add_component(entity, std::move(drawableComponent));
    registry.add_component(entity, Depth{3.0f});
    registry.add_component(entity, Health{100.0f});
    registry.add_component(entity, FFT{});
    spawnThruster(registry, entity, 0, 10);
    ComponentSpawner addLaser = [](entity_t entity, Registry &registry) {
        registry.add_component(entity, Laser{});
    };

    ComponentSpawner addDepth = [](entity_t entity, Registry &registry) {
        registry.add_component(entity, Depth{3.0f});
    };

    ComponentSpawner addDrawable = [](entity_t entity, Registry &registry) {
        registry.add_component(entity, Drawable{"laserAnimated", {0, 0, 64, 64}, {1.0f, 1.0f}});
    };
    ComponentSpawner addAnimated = [](entity_t entity, Registry &registry) {
        registry.add_component(entity, Animation{0, 5, 3.0f / FRAMERATE, 0.0f, false});
    };

    ComponentSpawner addCollision = [](entity_t entity, Registry &registry) {
        registry.add_component(entity, Collision{});
    };

    ComponentSpawner addVelocity = [](entity_t entity, Registry &registry) {
        registry.add_component(entity, Velocity{10, 0});
    };


    Spawner playerSpawner{
            .spawnRate = 5,
            .timeSinceLastSpawn = 0.0f,
            .entityToSpawn = "laserAnimated",
            .xOffset = 200,
            .yOffset = 0.0f,
            .xVelocity = 1.0f,
            .yVelocity = 0.0f,
            .componentSpawners = {addLaser, addDepth, addDrawable, addCollision, addVelocity, addAnimated}
    };

    registry.add_component(entity, Spawner{std::move(playerSpawner)});

    return entity;

}


entity_t spawnButton(Registry &registry, const MyRect &rect, const std::string &textureName, const std::string &text,
                     const std::string &fontName, const MyVector2 &scale, const MyVector2 &textOffset,
                     const MyVector2 &textScale, std::function<void()> onClick, const MyVector2 &position) {
    Drawable drawableComponent;
    drawableComponent.textureName = textureName;
    drawableComponent.textureRect = rect;
    drawableComponent.scale = scale;
    auto entity = registry.spawn_entity();
    registry.add_component(entity, Position{position.x, position.y});
    registry.add_component(entity, std::move(drawableComponent));
    registry.add_component(entity, Depth{3.0f});
// Add the isClickable component with a lambda function
    registry.add_component(entity, isClickable{false, onClick});
    registry.add_component(entity, TextDrawable{text, fontName});
    return entity;
}


entity_t spawnFollowerCover(entity_t parent_entity, Registry &registry, const MyRect &rect, const string &textureName,
                            const MyVector2 &offset) {
    //this will added to the beatmap panel
    auto entity = registry.spawn_entity();
    registry.add_component(entity, Position{0, 0});
    registry.add_component(entity, Drawable{textureName, rect, {0.5f, 0.5f}});
    registry.add_component(entity, Depth{2.0f});
    registry.add_component(entity, Follower{parent_entity, offset.x, offset.y});
    return entity;
}

entity_t
spawnFollowerText(entity_t parent_entity, Registry &registry, const std::string &Title, const float Xoffset,
                  const float Yoffset) {
    //this will added to the beatmap panel
    auto entity = registry.spawn_entity();
    registry.add_component(entity, Position{0, 0});
    registry.add_component(entity, TextDrawable{Title, "font"});
    registry.add_component(entity, Depth{2.0f});
    registry.add_component(entity, Follower{parent_entity, Xoffset, Yoffset});

    return entity;
}

entity_t spawnBeatmapPanel(Registry &registry,
                           Database &database,
                           const int &index) {
    Drawable drawableComponent;
    drawableComponent.textureName = "MainPanel01";
    drawableComponent.textureRect = {0, 0, 852, 527};
    drawableComponent.scale = {1.0f, 1.0f};
    BeatmapConfig beatmap = database.getBeatmap(index);

    auto entity = registry.spawn_entity();
    // place the panel at the center of the screen and in a column
    float y = 250;
    float x = 400 + index * 1000;
    registry.add_component(entity, Position{x, y});
    registry.add_component(entity, std::move(drawableComponent));
    registry.add_component(entity, Depth{1.0f});
    spawnFollowerCover(entity, registry, {0, 0, 500, 500}, beatmap.getFolderPath(), {100, 100});
    spawnFollowerText(entity, registry, beatmap.getName(), 500, 0);
    spawnFollowerText(entity, registry, beatmap.getArtist(), 500, 50);
    spawnFollowerText(entity, registry, std::to_string(beatmap.getDifficulty()), 500, 100);

    // registry.add_component(entity, TextDrawable{database.getBeatmap(index).getSongName(), "sansation"});
    return entity;


}


entity_t
spawnDrawable(Registry &registry, const MyRect &rect, const std::string &textureName, const MyVector2 &scale,
              float posX, float posY, float depth) {
    Drawable drawableComponent;
    drawableComponent.textureName = textureName;
    drawableComponent.textureRect = rect;
    drawableComponent.scale = scale;
    auto entity = registry.spawn_entity();
    registry.add_component(entity, Position{posX, posY});
    registry.add_component(entity, std::move(drawableComponent));
    registry.add_component(entity, Depth{depth});
    return entity;
}