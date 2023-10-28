#include <SFML/Graphics.hpp>
#include <iostream>
#include "Registry.hpp"  // Assume this header contains your Registry class definition
#include "Component.hpp" // Assume this header contains your component definitions
#include "SparseArray.hpp"
#include "Entity.hpp"
#include "Spawner.hpp"
#include "TextureManager.hpp"
#include "Global.hpp"
#include  "Database/Database.hpp"
#include "GUI/GUI.hpp"
#include "Config/Config.hpp"

#ifdef SERVER_MODE
#include "taskLoader.hpp"
#include "ServerMain.hpp"
#endif
#ifdef CLIENT_MODE
#include "ClientMain.hpp"
#include "ConfigServer.hpp"
#endif
void gameLoop(Registry &registry, sf::RenderWindow &window, sf::Clock &clock, float &elapsed_time) {
    window.clear();
    elapsed_time = clock.getElapsedTime().asSeconds();
    registry.run_systems();
    clock.restart();
    window.display();
    cleanup_system(registry);
    registry.delete_queued_entities();
    registry.create_queued_entities();
    registry.AddComponentsToNewEntities();
}


void MainLoop(Registry &registry, sf::RenderWindow &window, sf::Clock &clock, float &elapsed_time, GUI &gui,
                Database &database) {
    std::vector<std::function<void(int &)>> tasks;
#ifdef SERVER_MODE
    spawnServerEntity(registry);

    while (window.isOpen()) {
        // Handle SFML events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        gameLoop(registry, window, clock, elapsed_time);
    }
#endif
#ifdef CLIENT_MODE
    registry.stateEntity = spawnState(registry);
    auto &state = registry.get_components<State>()[registry.stateEntity];
    if (!state.has_value()) {
        std::cout << "state not found" << std::endl;
        return;
    }


    while (window.isOpen()) {
        // Handle SFML events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        window.clear();

        switch (state->state) {
            case GameState::MENU:
                /*
                if (gui.menu.update(event, gui.getWindow()) == 1) {
                    std::get<2>(registry.gameStates) = GameState::GAME;
                    //std::get<0>(registry.gameStates) = false;
                    std::cout << "Starting game" << std::endl;
                    registry.setPath(gui.menu.getActualSongName());
                    //clear entities that where suppsoed to be created in the old song
                    tasks.clear();

                    tasks.emplace_back([&](int &progress) {
                        initializeGameEntitiesFromConfig(registry, "Scene/GameScene.cfg");
                    });
                    tasks.emplace_back([&](int &progress) {
                        registry.run_fft_system();
                    });

                    tasks.emplace_back([&](int &progress) {
                        spawnServerEntity(registry);
                    });

                    // Call the function to execute tasks with the loading screen
                    executeLoadingTasks(gui, tasks);
                    gui.menu.setState(0);

                }
                gui.menu.draw(window);
*/                gameLoop(registry, window, clock, elapsed_time);
                break;
            case GameState::GAME:
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
                    std::get<2>(registry.gameStates) = GameState::MENU;
                    std::cout << "Back to menu size of constructor and inactive entities : "
                              << registry._entity_constructors.size() << " " << registry._inactive_entities.size()
                              << " " << registry._active_entities.size() << std::endl;
                    registry.clearGameState();
                    std::cout << "Back to menu size of constructor and inactive entities : "
                              << registry._entity_constructors.size() << " " << registry._inactive_entities.size()
                              << " " << registry._active_entities.size() << std::endl;

                    break;
                }
                gameLoop(registry, window, clock, elapsed_time);
                break;
            case GameState::MENUPRELOAD:
                std::cout << "MENUPRELOAD" << std::endl;
                gameLoop(registry, window, clock, elapsed_time);
                break;
            case GameState::LOBBY:
                std::cout << "LOBBY" << std::endl;
                gameLoop(registry, window, clock, elapsed_time);
                break;
            case GameState::EXIT:
                window.close();
                break;
            default:
                break;
        }
    }
#endif
}

int main() {
    Registry registry;
    Config config;

    Database database;
    sf::RenderWindow window;
#ifdef SERVER_MODE
    window.create(sf::VideoMode(1920, 1080), "R-Type Server");
#endif

#ifdef CLIENT_MODE
    window.create(sf::VideoMode(1920, 1080), "R-Type Client");
#endif

    window.setFramerateLimit(FRAMERATE);
    GUI gui(window);
    AssetManager& assetManager = AssetManager::Instance();
    loadAllTexture(assetManager);

    // Timing variables for frame animation
    sf::Clock clock = sf::Clock();
    float elapsed_time = 0.0f;

    register_all_components(registry);

#ifdef SERVER_MODE
    initializeGameServerSystems(registry, window, assetManager, elapsed_time);

    initializeDatabaseAndConfig(config, database, gui);
   // firstLoadingScreen(gui, database, registry, window);
#endif

#ifdef CLIENT_MODE
    std::cout << "SO COOL TO BE A CLIENT" << std::endl;
    // Add client initialization and loop here
    loadConfig("config/ClientConnectionConfig.cfg");
    registerClientSystem(registry, window, assetManager, elapsed_time);
#endif
    MainLoop(registry, window, clock, elapsed_time, gui, database);
    return 0;
}

