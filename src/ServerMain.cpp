

#include "ServerMain.hpp"
#include "SceneParsing.hpp"



/*
void initializeGameEntitiesServer(Registry& registry)
{
    // Initialize Drawable component with texture and animation settings
    spawnPlayer(registry, {0, 0, 176, 96});
    spawnBackgroundLayer(registry, Behavior{"Fixed"},"background", 0, 0, {0, 0, 1920, 1080}, {1.0f, 1.0f});
    spawnBackgroundLayer(registry, Behavior{"LoopDiagonalRight"},"far_planets", 0.01 * 10.0f * 60/FRAMERATE, 0.014 * 10.0f * 60/FRAMERATE, {0, 0, 272, 160}, {1.0f, 1.0f});
    spawnBackgroundLayer(registry, Behavior{"LoopDiagonalRight"},"stars", 0.02 * 10.0f * 60/FRAMERATE, 0.028 * 10.0f * 60/FRAMERATE, {0, 0, 272, 160}, {1.0f, 1.0f});
    spawnBackgroundLayer(registry, Behavior{"LoopDiagonalRight"},"big_planet", 0.03 * 10.0f * 60/FRAMERATE, 0.042 * 10.0f * 60/FRAMERATE, {0, 0, 88, 87}, {1.0f, 1.0f});
    spawnBackgroundLayer(registry, Behavior{"LoopDiagonalRight"},"ring_planet", 0.04 * 10.0f * 60/FRAMERATE, 0.056 * 10.0f * 60/FRAMERATE, {0, 0, 51, 115}, {1.0f, 1.0f});
    spawnBackgroundLayer(registry, Behavior{"LoopDiagonalRight"}, "stars", 0.05 * 10.0f * 60/FRAMERATE, 0.07 * 10.0f * 60/FRAMERATE, {0, 0, 272, 160}, {1.0f, 1.0f});
    spawnAnimatedEntity(registry, {0, 0, 126, 126}, {1.0f, 1.0f}, "green_planet", 400, 300);

    spawnEnemy(registry, Behavior{"Zigzag"}, 1920, 300, "e_ship1", {0, 0, 128, 128}, {1.0f, 1.0f});
    spawnEnemy(registry, Behavior{"MoveLeft"}, 1920, 200, "e_ship1", {0, 0, 128, 128}, {1.0f, 1.0f});
    spawnEnemy(registry, Behavior{"Circular"}, 1920, 300, "e_ship1", {0, 0, 128, 128}, {1.0f, 1.0f});
    spawnEnemy(registry, Behavior{"Diagonal"}, 1920, 400,"e_ship1", {0, 0, 128, 128}, {1.0f, 1.0f});
    spawnEnemy(registry, Behavior{"SineWave"}, 1920, 500,"e_ship1", {0, 0, 128, 128}, {1.0f, 1.0f});

    spawnEnemy(registry, Behavior{"BackAndForth"}, 500, 100, "e_ship1", {0, 0, 128, 128}, {1.0f, 1.0f});
    spawnEnemy(registry, Behavior{"LShape"}, 700, 100, "e_ship1", {0, 0, 128, 128}, {1.0f, 1.0f});
    spawnEnemy(registry, Behavior{"Random"}, 1200, 300, "e_ship1", {0, 0, 128, 128}, {1.0f, 1.0f});
}
*/
void initializeGameServerSystems(Registry &registry, sf::RenderWindow &window, AssetManager &assetManager,
                                 float &elapsed_time) {
    // Register components
    registerServerSystem(registry, window, assetManager, elapsed_time);
}



void initializeDatabaseAndConfig(Config &config, Database &database, GUI &gui) {


    cout << "loading database" << endl;
    database.load("./config/Database.cfg");
    cout << "loaded database" << endl;
    for (int i = 0; i < database.getNbBeatmaps(); ++i) {
        database.isBeatmapExist(i);
    }
    cout << "checked database" << endl;
    database.print();
    cout << "printed database" << endl;

    cout << "loading config" << endl;
    config.load("config/game.cfg");
    cout << "loaded config" << endl;
    config.print();
    cout << "printed config" << endl;

    std::cout << "Loading GUI configuration" << std::endl;
}

void firstLoadingScreen(GUI &gui, Database &database, Registry &registry, sf::RenderWindow &window) {

    gui.loadingScreenCreate();
    gui.setActive(false);
    std::vector<std::function<void(int &)>> tasks;

    tasks.push_back([&](int &progress) {
        gui.menu.load(make_pair(gui.getWindow().getSize().x, gui.getWindow().getSize().y), database);
    });

    tasks.push_back([&](int &progress) {
        registry.run_fft_system();
    });

    // Call the function to execute tasks with the loading screen
    executeLoadingTasks(gui, tasks);

    std::cout << "Loaded assets" << std::endl;
    // Start the main loop
    // for the moment, the menu is the main loop

    window.clear();


    // Main game loop
}

