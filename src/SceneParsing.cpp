
#include "SceneParsing.hpp"
#include "Global.hpp"
std::unordered_map<std::string, ComponentAdder> componentAdderMap = {
        {"Position", addPosition},
        {"Velocity", addVelocity},
        {"Drawable", addDrawable},
        {"Collision", addCollision},
        {"Acceleration", addAcceleration},
        {"Animation", addAnimation},
        {"GameConfig", addGameConfig},
        {"Controllable", addControllable},
        {"Laser", addLaser},
        {"Depth", addDepth},
        {"Behavior", addBehavior},
        {"FFT", addFFT},
        {"Spawner", addSpawner},
        {"Follower", addFollower}
};

ComponentAdder* getComponentAdderByName(const std::string& name) {
    if (componentAdderMap.find(name) != componentAdderMap.end()) {
        return &componentAdderMap[name];
    }
    return nullptr;
}

ComponentSpawner makeAdder(const std::string& name, const libconfig::Setting& params) {
    std::cout << "Creating adder for " << name << std::endl;
    if (name == "addPosition") {
        float x = params["x"];
        float y = params["y"];
        return [x, y](entity_t entity, Registry& registry) {
            registry.add_component(entity, Position{x, y});
        };
    }
    // Similar functions for other component types
    if (name == "addVelocity") {
        float x = params["x"];
        float y = params["y"];
        return [x, y](entity_t entity, Registry& registry) {
            registry.add_component(entity, Velocity{x, y});
        };
    }
    if (name == "addAcceleration") {
        float x = params["x"];
        float y = params["y"];
        return [x, y](entity_t entity, Registry& registry) {
            registry.add_component(entity, Acceleration{x, y});
        };
    }
    if (name == "addDrawable") {
        std::string textureName = params["textureName"];
        MyRect textureRect;
        std::cout << "going into textureRect" << std::endl;
        textureRect.left = params["textureRect"][0];
        textureRect.top = params["textureRect"][1];
        textureRect.width = params["textureRect"][2];
        textureRect.height = params["textureRect"][3];
        std::cout << "textureName: " << textureName << std::endl;
        std::cout << "textureRect: " << textureRect.left << ", " << textureRect.top << ", " << textureRect.width << ", " << textureRect.height << std::endl;
        return [textureName, textureRect](entity_t entity, Registry& registry) {
            registry.add_component(entity, Drawable{textureName, textureRect});
        };
    }
    if (name == "addCollision") {
        bool isEnemy = params["isEnemy"];
        return [isEnemy](entity_t entity, Registry& registry) {
            registry.add_component(entity, Collision{isEnemy});
        };
    }
    if (name == "addAnimation") {
        int currentFrame = params["currentFrame"];
        int numFrames = params["numFrames"];
        float frameDuration = params["frameDuration"];
        float elapsedTime = params["elapsedTime"];
        bool loop = params["loop"];
        return [currentFrame, numFrames, frameDuration, elapsedTime, loop](entity_t entity, Registry& registry) {
            registry.add_component(entity, Animation{currentFrame, numFrames, frameDuration/FRAMERATE, elapsedTime, loop});
        };
    }
    if (name == "addGameConfig") {
        float screenWidth = params["screenWidth"];
        float screenHeight = params["screenHeight"];
        float playerSpeed = params["playerSpeed"];
        return [screenWidth, screenHeight, playerSpeed](entity_t entity, Registry& registry) {
            registry.add_component(entity, GameConfig{screenWidth, screenHeight, playerSpeed});
        };
    }
    if (name == "addControllable") {
        bool isLocal = params["isLocal"];
        int client_id = params["client_id"];
        return [isLocal, client_id](entity_t entity, Registry& registry) {
            registry.add_component(entity, Controllable{isLocal, client_id});
        };
    }
    if (name == "addLaser") {
        return [](entity_t entity, Registry& registry) {
            registry.add_component(entity, Laser{});
        };
    }
    if (name == "addDepth") {
        float z = params["z"];
        return [z](entity_t entity, Registry& registry) {
            registry.add_component(entity, Depth{z});
        };
    }
    if (name == "addBehavior") {
        std::string behaviorID = params["type"];
        return [behaviorID](entity_t entity, Registry& registry) {
            registry.add_component(entity, Behavior{behaviorID});
        };
    }
    if (name == "addFFT") {
        return [](entity_t entity, Registry& registry) {
            registry.add_component(entity, FFT{});
        };
    }
    std::cout << "Component adder not found: " << name << std::endl;
    return nullptr;
}
void addPosition(entity_t entity, Registry& registry, const libconfig::Setting& componentData) {
    float x, y;
    componentData.lookupValue("x", x);
    componentData.lookupValue("y", y);
    registry.add_component(entity, Position{x, y});
}

// Similar functions for other component types

void addVelocity(entity_t entity, Registry& registry, const libconfig::Setting& componentData) {
    float x, y;
    componentData.lookupValue("x", x);
    componentData.lookupValue("y", y);
    registry.add_component(entity, Velocity{x, y});
}

void addCollision(entity_t entity, Registry& registry, const libconfig::Setting& componentData) {
    bool isEnemy = false;
    componentData.lookupValue("isEnemy", isEnemy);
    registry.add_component(entity, Collision{isEnemy});
}


void addDrawable(entity_t entity, Registry& registry, const libconfig::Setting& componentData) {
    std::string textureName;
    MyRect textureRect;
    componentData.lookupValue("textureName", textureName);
    const libconfig::Setting& textureRectSetting = componentData["textureRect"]; // Fixed this line
    textureRect.left = textureRectSetting[0];
    textureRect.top = textureRectSetting[1];
    textureRect.width = textureRectSetting[2];
    textureRect.height = textureRectSetting[3];
    registry.add_component(entity, Drawable{textureName, textureRect});
}

void addAcceleration(entity_t entity, Registry& registry, const libconfig::Setting& componentData) {
    float x, y;
    componentData.lookupValue("x", x);
    componentData.lookupValue("y", y);
    registry.add_component(entity, Acceleration{x, y});
}

void addAnimation(entity_t entity, Registry& registry, const libconfig::Setting& componentData) {
    int currentFrame = 0;
    int numFrames = 1;
    float frameDuration = 0.0f;
    float elapsedTime = 0.0f;
    bool loop = true;

    componentData.lookupValue("currentFrame", currentFrame);
    componentData.lookupValue("numFrames", numFrames);
    componentData.lookupValue("frameDuration", frameDuration);
    componentData.lookupValue("elapsedTime", elapsedTime);
    componentData.lookupValue("loop", loop);
    registry.add_component(entity, Animation{currentFrame, numFrames, frameDuration/FRAMERATE, elapsedTime, loop});
}

void addGameConfig(entity_t entity, Registry& registry, const libconfig::Setting& componentData) {
    float screenWidth, screenHeight, playerSpeed;
    componentData.lookupValue("screenWidth", screenWidth);
    componentData.lookupValue("screenHeight", screenHeight);
    componentData.lookupValue("playerSpeed", playerSpeed);
    std::cout << "screenWidth: " << screenWidth << std::endl;
    std::cout << "screenHeight: " << screenHeight << std::endl;
    std::cout << "playerSpeed: " << playerSpeed << std::endl;
    registry.add_component(entity, GameConfig{screenWidth, screenHeight, playerSpeed});
}

void addControllable(entity_t entity, Registry& registry, const libconfig::Setting& componentData) {
    bool isLocal = false;
    int client_id = 0;
    componentData.lookupValue("isLocal", isLocal);
    componentData.lookupValue("client_id", client_id);
    registry.add_component(entity, Controllable{isLocal, client_id});

}

void addLaser(entity_t entity, Registry& registry, const libconfig::Setting& componentData) {
    registry.add_component(entity, Laser{});
}

void addDepth(entity_t entity, Registry& registry, const libconfig::Setting& componentData) {
    float z = 0.0f;
    componentData.lookupValue("z", z);
    registry.add_component(entity, Depth{z});
}

void addBehavior(entity_t entity, Registry& registry, const libconfig::Setting& componentData) {
    std::string behaviorID;
    Behavior behavior;
  //  float originX = 0.0f;
    //float originY = 0.0f;
    componentData.lookupValue("type", behaviorID);
    std::cout << "behaviorID: " << behaviorID << std::endl;
  //  componentData.lookupValue("originX", originX);
   // componentData.lookupValue("originY", originY);
    registry.add_component(entity, Behavior{behaviorID});
}


void addFFT(entity_t entity, Registry& registry, const libconfig::Setting& componentData) {
    registry.add_component(entity, FFT{});
}
void addSpawner(entity_t entity, Registry& registry, const libconfig::Setting& componentData) {
    float spawnRate = 0.0f;
    float timeSinceLastSpawn = 0.0f;
    std::string entityToSpawn;
    float xOffset = 0.0f;
    float yOffset = 0.0f;
    float xVelocity = 0.0f;
    float yVelocity = 0.0f;
    std::vector<ComponentSpawner > componentAdders;

    componentData.lookupValue("spawnRate", spawnRate);
    componentData.lookupValue("timeSinceLastSpawn", timeSinceLastSpawn);
    componentData.lookupValue("entityToSpawn", entityToSpawn);
    componentData.lookupValue("xOffset", xOffset);
    componentData.lookupValue("yOffset", yOffset);
    componentData.lookupValue("xVelocity", xVelocity);
    componentData.lookupValue("yVelocity", yVelocity);

    std::cout << "starting to look for component adders" << std::endl;
    // Check if componentAdders is present
// Inside your function that populates the Spawner component
    if (componentData.exists("componentAdders")) {
        const libconfig::Setting& componentAddersData = componentData["componentAdders"];
        for (int i = 0; i < componentAddersData.getLength(); ++i) {
            const libconfig::Setting& componentAdderData = componentAddersData[i];
            std::string name;
            componentAdderData.lookupValue("name", name);
            const libconfig::Setting& params = componentAdderData["params"];

            ComponentSpawner adder = makeAdder(name, params);
            if (adder) {
                componentAdders.push_back(adder);
            }
            std::cout << "componentAdder: " << name << std::endl;
        }
    }

    registry.add_component(entity, Spawner{spawnRate, timeSinceLastSpawn, entityToSpawn, xOffset, yOffset, xVelocity, yVelocity, componentAdders});
}

void addFollower(entity_t parentEntity, Registry& registry, const libconfig::Setting& componentData) {
    // Read the basic settings
    float xOffset = 0.0f;
    float yOffset = 0.0f;
    std::string type;

    componentData.lookupValue("xOffset", xOffset);
    componentData.lookupValue("yOffset", yOffset);
    componentData.lookupValue("type", type); // Assuming 'type' is the entity type of the follower

    // Create the follower entity
    entity_t followerEntity = registry.spawn_entity();

    // Add the Follower component to it
    registry.add_component(followerEntity, Follower{parentEntity, xOffset, yOffset});

    // Add other components based on the componentAdders
    if (componentData.exists("componentAdders")) {
        const libconfig::Setting& componentAddersData = componentData["componentAdders"];
        for (int i = 0; i < componentAddersData.getLength(); ++i) {
            const libconfig::Setting& componentAdderData = componentAddersData[i];
            std::string name;
            componentAdderData.lookupValue("name", name);

            const libconfig::Setting& params = componentAdderData["params"];
            ComponentSpawner adder = makeAdder(name, params); // Assuming makeAdder returns a ComponentSpawner

            if (adder) {
                std::cout << "Adding component " << name << " to follower" << std::endl;
                adder(followerEntity, registry);
            }
        }
        //add the position in a kinda hacky way
    }
}



void initializeGameEntitiesFromConfig(Registry &registry, const std::string &configFilePath) {
    libconfig::Config cfg;
    std::cout << "Loading config file: " << configFilePath << std::endl;
    try {
        cfg.readFile(configFilePath.c_str());
    } catch (const libconfig::FileIOException &fioex) {
        std::cerr << "I/O error while reading config file." << std::endl;
        return;
    } catch (const libconfig::ParseException &pex) {
        std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
                  << " - " << pex.getError() << std::endl;
        return;
    }
    std::cout << "Config file loaded successfully." << std::endl;

    const libconfig::Setting &root = cfg.getRoot();
    const libconfig::Setting &entities = root["entities"];

    for (int i = 0; i < entities.getLength(); ++i) {
        const libconfig::Setting &entityData = entities[i];
        auto entity = registry.spawn_entity();

        // Iterate over all fields in the entity's configuration
        for (int j = 0; j < entityData.getLength(); ++j) {
            const libconfig::Setting &componentData = entityData[j];
            const std::string &componentName = componentData.getName();

            // Find the component adder for this component
            auto it = componentAdderMap.find(componentName);
            if (it != componentAdderMap.end()) {
                // Use the component adder to add the component to the entity
                it->second(entity, registry, componentData);
            }
            //debug print of the entity
            std::cout << "Entity " << i << " : " << componentName << std::endl;
        }
    }
    //for debug can you add a entity that have a textDrawable component thats all
    auto entity = registry.spawn_entity();
    registry.add_component(entity, Position{0, 0});
    registry.add_component(entity, TextDrawable{"Hello World", "font", 50});
    registry.add_component(entity, Depth{0.0f});
    registry.add_component(entity, Drawable{"background", MyRect{0, 0, 1920, 1080}});
}