
#include "TextureManager.hpp"

void loadAllTexture(AssetManager& assetManager//TODO rename to loadAllAssets for game
)
{
    //clear before loading
    assetManager.UnloadAllTextures();
    assetManager.LoadTexture("green_planet", "asset/GreenPlanet.png");
    assetManager.LoadTexture("background", "asset/layers/parallax-space-backgound.png");
    assetManager.LoadTexture("big_planet", "asset/layers/parallax-space-big-planet.png");
    assetManager.LoadTexture("far_planets", "asset/layers/parallax-space-far-planets.png");
    assetManager.LoadTexture("ring_planet", "asset/layers/parallax-space-ring-planet.png");
    assetManager.LoadTexture("stars", "asset/layers/parallax-space-stars.png");
    assetManager.LoadTexture("laser", "asset/Spaceship/laser.png");
    assetManager.LoadTexture("laserAnimated", "asset/Spaceship/shootingSheet.png");
    assetManager.LoadTexture("p_spaceship", "asset/Spaceship/vehicle-1.png");
    assetManager.LoadTexture("p_thruster", "asset/Spaceship/thrust/thrust.png");
    assetManager.LoadTexture("e_ship1", "asset/ennemy_ship/Ship2/Ship2.png");
    assetManager.LoadTexture("e_ship2", "asset/ennemy_ship/Ship3/Ship3.png");
    assetManager.LoadTexture("e_ship3", "asset/ennemy_ship/Ship4/Ship4.png");
    assetManager.LoadTexture("button", "asset/Button11.png");
    //fonts
    assetManager.LoadFont("font", "asset/Fonts/sansation.ttf");
}


void loadMenuAsset(AssetManager& assetManager, Database& database)
{
    //clear before loading
    assetManager.UnloadAllTextures();
    for (int i = 1; i <= 6; ++i) {
        std::string texturePath = "asset/UI/Btn0" + std::to_string(i) + ".png";
        assetManager.LoadTexture("Btn0" + std::to_string(i), texturePath);
    }

    assetManager.LoadTexture( "background","asset/UI/Background.jpg");
    assetManager.LoadTexture("MainPanel01", "asset/UI/MainPanel01.png" );
    assetManager.LoadTexture("ArrowsRight","asset/UI/ArrowsRight.png");
    assetManager.LoadTexture("ArrowsLeft","asset/UI/ArrowsLeft.png" );
    for (int i = 0; i < database.getNbBeatmaps(); ++i) {
        BeatmapConfig beatmap = database.getBeatmap(i);
        if (beatmap.getFolderPath() == "") {
            continue;
        }
        std::string coverPath = "asset/Beatmaps/" + beatmap.getFolderPath() + "/Cover.jpg";
        assetManager.LoadTexture(beatmap.getFolderPath(),coverPath);
    }
    //fonts
    assetManager.LoadFont("font", "asset/Fonts/sansation.ttf");


    //load the lobby texture
    assetManager.LoadTexture("LobbyBackground", "asset/UI/lobby_background.jpg");
    assetManager.LoadTexture("LobbyButtonLeft", "asset/UI/lobby_button_left.png");
    assetManager.LoadTexture("LobbyButtonRight", "asset/UI/lobby_button_right.png");
    assetManager.LoadTexture("LobbyHolderBig", "asset/UI/lobby_holder_big.png");
    assetManager.LoadTexture("LobbyHolderSmall", "asset/UI/lobby_holder_small.png");
    assetManager.LoadTexture("LobbyGridHolder", "asset/UI/lobby_grid_holder.png");
    assetManager.LoadTexture("lobbyPlayerIcon", "asset/UI/lobby_player_icon.png");

}