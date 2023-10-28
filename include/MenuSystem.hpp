/*
** EPITECH PROJECT, 2024
** ECS
** File description:
** QUADTREE
*/
#ifndef MENUSYSTEM_HPP
#define MENUSYSTEM_HPP

#include "ISystem.hpp"
#include "Spawner.hpp"
#include "SceneParsing.hpp"
#include <SFML/Graphics.hpp>




//maybe create a vector of lambda for the button

class MenuSystem : public ISystem<State> {
private:
    Registry &registry;
    std::vector<entity_t> beatmapEntities;
    int selectedPanelIndex = 0;
    AssetManager &assetManager;
    Database database;
    bool gameStartedFlag = false;
    bool lobbyCreatedFlag = false;
    bool lobbySelectionCreatedFlag = false;
    bool lobbyJoinedFlag = false;
    bool tryConnectFlag = false;

public:
    MenuSystem(Registry &registry, AssetManager &assetManager) : registry(registry), assetManager(assetManager) {


    }

    void InitializeMainMenu(Registry &registry) {
        cout << "loading database" << endl;
        database.load("./config/Database.cfg");
        cout << "loaded database" << endl;
        for (int i = 0; i < database.getNbBeatmaps(); ++i) {
            database.isBeatmapExist(i);
        }
        cout << "checked database" << endl;
        database.print();
        cout << "printed database" << endl;
        loadMenuAsset(assetManager, database);
        for (int i = 0; i < database.getNbBeatmaps(); ++i) {
            beatmapEntities.push_back(spawnBeatmapPanel(registry, database, i));
        }


        std::vector<std::function<void()>> buttonCallbacks = {
                [this, &registry]() {
                    Event newEvent;
                    newEvent.type = EventType::GameExited;
                    newEvent.content = "-1";
                    newEvent.clientID = 0;
                    registry.eventBus.fireEvent(newEvent);
                },
                [this, &registry]() {
                    Event newEvent;
                    newEvent.type = EventType::IndexMoved;
                    newEvent.content = "-1";
                    newEvent.clientID = 0;
                    registry.eventBus.fireEvent(newEvent);
                },
                [this, &registry]() {

                    Event newEvent;
                    newEvent.type = EventType::IndexMoved;
                    newEvent.content = "1";
                    newEvent.clientID = 0;
                    registry.eventBus.fireEvent(newEvent);
                },
                [this, &registry]() {
                    Event newEvent;
                    newEvent.type = EventType::AskDataCreationLobby;
                    //regroupe name of the song and artiste name
                    newEvent.content = database.getBeatmap(selectedPanelIndex).getFolderPath() + " " +
                                       database.getBeatmap(selectedPanelIndex).getName() + " " +
                                       database.getBeatmap(selectedPanelIndex).getArtist();
                    newEvent.clientID = 0;
                    registry.eventBus.fireEvent(newEvent);
                },
                [this, &registry]() {
                    Event newEvent;
                    newEvent.type = EventType::LobbySelectionCreated;
                    newEvent.content = "-1";
                    newEvent.clientID = 0;
                    registry.eventBus.fireEvent(newEvent);
                },
                [this, &registry]() {
                    Event newEvent;
                    newEvent.type = EventType::ClientTryConnect;
                    newEvent.content = "-1";
                    newEvent.clientID = 0;
                    registry.eventBus.fireEvent(newEvent);
                }
        };

        registry.eventBus.addListener(IndexMoved, [this, &registry](const Event &event) {
            // Your code to handle the event
            int oldIndex = selectedPanelIndex;
            selectedPanelIndex += atoi(event.content.c_str());
            if (selectedPanelIndex < 0)
                selectedPanelIndex = 0;
            if (selectedPanelIndex > beatmapEntities.size() - 1)
                selectedPanelIndex = beatmapEntities.size() - 1;

            // Only proceed if the index actually changed
            if (oldIndex == selectedPanelIndex) {
                return;
            }

            // Calculate the distance to move based on the initial panel spacing
            int distance = 1000;  // Initial distance between each panel

            // Calculate the offset based on the old and new selected indices
            int offset = (selectedPanelIndex - oldIndex) * distance;

            // Update all beatmap panels to move them to the left or right
            for (entity_t entity: beatmapEntities) {
                auto &position = registry.get_components<Position>();
                auto &positionOpt = position[entity];
                if (!positionOpt) {
                    std::cerr << "Beatmap panel entity " << entity << " has no position component" << std::endl;
                    continue;
                }
                auto &pos = *positionOpt;

                // Move each panel by the calculated offset
                pos.x -= offset;

                // Optional: Add boundary checks here
            }
        });

        registry.eventBus.addListener(LobbyCreated, [this, &registry](const Event &event) {
            // Your code to handle the event
            std::cout << "LobbyCreated" << std::endl;
            std::cout << "selectedPanelIndex: " << selectedPanelIndex << std::endl;
            lobbyCreatedFlag = true;
            auto &stateOpt = registry.get_components<State>()[registry.stateEntity];
            if (!stateOpt.has_value()) {
                std::cerr << "Entity " << registry.stateEntity << " has no state component" << std::endl;
                return;
            }
            auto &state = *stateOpt;
            state.state = LOBBY;
            //need to kill all the beatmap panel, button and background
        });

// Initialize buttons
        for (int i = 0; i < 6; ++i) {
            std::string textureName = "Btn0" + std::to_string(i + 1);
            MyRect rect{0, 0, 155, 42};  // Modify as needed
            std::string text = "Button" + std::to_string(i + 1);  // Modify as needed
            MyVector2 scale{1, 1};  // Modify as needed
            MyVector2 textOffset{0, 0};  // Modify as needed
            MyVector2 textScale{1, 1};  // Modify as needed
            float x = 200 + i * 200;
            float y = 900;
            MyVector2 position{x, y};
            spawnButton(registry, rect, textureName, text, "font", scale, textOffset, textScale, buttonCallbacks[i],
                        position);
        }
        spawnBackgroundLayer(registry, Behavior{"Fixed"}, "background", 0, 0, MyRect{0, 0, 1920, 1080},
                             MyVector2{1, 1});
        std::cout << "END OF INIT" << std::endl;

    }

    void InitializeLobby(Registry &registry) {
        std::cout << "InitializeLobby" << std::endl;
        std::vector<std::function<void()>> buttonCallbacks = {
                [this, &registry]() {
                    Event newEvent;
                    newEvent.type = EventType::LobbyDestroyed;
                    newEvent.content = "-1";
                    newEvent.clientID = 0;
                    registry.eventBus.fireEvent(newEvent);
                },
                [this, &registry]() {
                    Event newEvent;
                    newEvent.type = EventType::LobbyMessageSent;
                    newEvent.content = "-1";
                    newEvent.clientID = 0;
                    registry.eventBus.fireEvent(newEvent);
                },
                [this, &registry]() {
                    Event newEvent;
                    newEvent.type = EventType::GameStarted;
                    newEvent.content = "-1";
                    newEvent.clientID = 0;
                    registry.eventBus.fireEvent(newEvent);
                },
                [this, &registry]() {
                    Event newEvent;
                    newEvent.type = EventType::PlayerReady;
                    newEvent.content = "-1";
                    newEvent.clientID = 0;
                    registry.eventBus.fireEvent(newEvent);
                }
        };
        //will now create the entities of the lobby :
        // 1 background
        // 1 button to leave the lobby
        // 1 button to send a message(later)
        // 1 button to start the game
        // 1 button to ready up (later as depend on the server)
        // 1 text box to display the message (later)
        // 1 text box to display the player in the lobby (later)
        // 1 text box to display the player ready (later)
        // 2 holder for the artiste name and name of the song
        // 1 cover of the song
        //they are all gonna be follower of the background
        std::cout << "size of constructor and inactive entities : "
                  << registry._entity_constructors.size() << " " << registry._inactive_entities.size()
                  << " " << registry._active_entities.size() << std::endl;
        //background
        spawnBackgroundLayer(registry, Behavior{"Fixed"}, "background", 0, 0, MyRect{0, 0, 1920, 1080},
                             MyVector2{1, 1});
        entity_t background = spawnDrawable(registry, MyRect{0, 0, 922, 599}, "LobbyBackground",MyVector2{1.5, 1.5}, 300, 125, 1);
        //button to leave the lobby
        entity_t BackButton = spawnButton(registry, MyRect{0, 0, 225, 50}, "LobbyButtonLeft", "Leave", "font",
                                          MyVector2{0.5, 0.5},
                                          MyVector2{0, 0}, MyVector2{1, 1}, buttonCallbacks[0], MyVector2{200, 900});

        registry.add_component<Follower>(BackButton, Follower{background, 50, 800});
        //button to start the game
        entity_t StartButton = spawnButton(registry, MyRect{0, 0, 225, 50}, "LobbyButtonRight", "Start", "font",
                                           MyVector2{0.5, 0.5},
                                           MyVector2{0, 0}, MyVector2{1, 1}, buttonCallbacks[2], MyVector2{200, 900});
        registry.add_component<Follower>(StartButton, Follower{background, 1000, 800});

        //the big cover of the song
        spawnFollowerCover(background, registry, MyRect{0, 0, 500, 500},
                           database.getBeatmap(selectedPanelIndex).getFolderPath(), MyVector2{1000, 150});
        //the 2 holder for the artiste name and name of the song

        entity_t TitleHolder = spawnDrawable(registry, MyRect{0, 0, 208, 29}, "LobbyHolderBig",MyVector2{1, 1}, 300, 125, 2);
        registry.add_component<Follower>(TitleHolder, Follower{background, 300, 100});
        spawnFollowerText(TitleHolder, registry, database.getBeatmap(selectedPanelIndex).getArtist(), 0, 0);
        entity_t ArtistHolder = spawnDrawable(registry, MyRect{0, 0, 183, 29}, "LobbyHolderSmall",MyVector2{1, 1}, 300, 125, 2);
        spawnFollowerText(ArtistHolder, registry, database.getBeatmap(selectedPanelIndex).getName(), 0, 0);
        registry.add_component<Follower>(ArtistHolder, Follower{background, 300 + 208 + 10, 100});


        std::cout << "END OF INIT LOBBY" << std::endl;

    }

    void InitializeLobbySelectionScene(Registry &registry)
    {
        std::cout << "InitializeLobby" << std::endl;
        std::vector<std::function<void()>> buttonCallbacks = {
                [this, &registry]() {
                    Event newEvent;
                    newEvent.type = EventType::LobbyDestroyed;
                    newEvent.content = "-1";
                    newEvent.clientID = 0;
                    registry.eventBus.fireEvent(newEvent);
                },
                [this, &registry]() {
                    Event newEvent;
                    newEvent.type = EventType::LobbyMessageSent;
                    newEvent.content = "-1";
                    newEvent.clientID = 0;
                    registry.eventBus.fireEvent(newEvent);
                },
                [this, &registry]() {
                    Event newEvent;
                    newEvent.type = EventType::GameStarted;
                    newEvent.content = "-1";
                    newEvent.clientID = 0;
                    registry.eventBus.fireEvent(newEvent);
                },
                [this, &registry]() {
                    Event newEvent;
                    newEvent.type = EventType::PlayerReady;
                    newEvent.content = "-1";
                    newEvent.clientID = 0;
                    registry.eventBus.fireEvent(newEvent);
                }
        };
    }
    void operator()(entity_t entity, sparse_array<State> &states) override {
        // Your code to update the entity
        auto &stateOpt = states[entity];
        if (!stateOpt.has_value()) {
            std::cerr << "Entity " << entity << " has no state component" << std::endl;
            return;
        }
        auto &state = *states[entity];
        switch (state.state) {
            case MENU:
                break;
            case LOBBY:
                std::cout << "LOBBY" << std::endl;
                break;
            case MENUPRELOAD:
                InitializeMainMenu(registry);
                state.state = MENU;
                std::cout << "MENUPRELOAD ended" << std::endl;
                break;
            case EXIT:
                break;
            default:
                break;
        }
    }

    void PostUpdate() override {
        if (lobbyCreatedFlag) {
            registry.clearGameState();
            std::cout << "size of constructor and inactive entities : "
                      << registry._entity_constructors.size() << " " << registry._inactive_entities.size()
                      << " " << registry._active_entities.size() << std::endl;
            std::cout << "LobbyCreated" << std::endl;
            std::string songName = database.getBeatmap(selectedPanelIndex).getFolderPath();
            std::cout << "songName: " << songName << std::endl;
            registry.setPath("./asset/Beatmaps/" + songName);
            InitializeLobby(registry);
            lobbyCreatedFlag = false;
            registry.stateChanged = true;

        }

        if (gameStartedFlag) {
            std::cout << "Game started" << std::endl;
            std::cout << "GameStarted" << std::endl;
            std::cout << "selectedPanelIndex: " << selectedPanelIndex << std::endl;
            std::cout << "beatmapEntities.size(): " << beatmapEntities.size() << std::endl;
            //need to kill all the beatmap panel, button and background
            registry.clearGameState();
            loadAllTexture(assetManager);
            //get the name of the actual song
            std::string songName = database.getBeatmap(selectedPanelIndex).getFolderPath();
            std::cout << "songName: " << songName << std::endl;
            registry.setPath("./asset/Beatmaps/" + songName);
            //check the number of entities
            std::cout << "size of constructor and inactive entities : "
                      << registry._entity_constructors.size() << " " << registry._inactive_entities.size()
                      << " " << registry._active_entities.size() << std::endl;
            spawnServerEntity(registry);
            initializeGameEntitiesFromConfig(registry, "Scene/GameScene.cfg");
            std::cout << "size of constructor and inactive entities : "
                      << registry._entity_constructors.size() << " " << registry._inactive_entities.size()
                      << " " << registry._active_entities.size() << std::endl;
            registry.stateChanged = true;
            gameStartedFlag = false;
        }
    }
};

#endif /* !MENUSYSTEM_HPP */
