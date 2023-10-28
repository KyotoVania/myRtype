/*
** EPITECH PROJECT, 2024
** ECS
** File description:
** SYSTEM
*/
#ifndef SYSTEM_HPP_
#define SYSTEM_HPP_
#include "SpritePool.hpp"
#include "ISystem.hpp"
#include "Component.hpp"
#include "Registry.hpp"
#include "TextureManager.hpp"
#include "SparseArray.hpp"
#include "Behaviour.hpp"
#include "Spawner.hpp"
#include "ConfigServer.hpp"
#include "ClientMain.hpp"
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
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
enum class FFTState {
    IDLE,
    LOADED,
    LOADING,
    NOTLOADED
};

enum class PlayerState {
    InMenu,
    InGame,
    InLobby
};
enum class PlayerSubState {
    isLoading,
    isDownloading,
    isWaiting,
    isReady,
    isPlaying,
    isDead,
    isWin,
    isLose,
    isDisconnected,
    isReconnecting,
    isReconnected,
    isReconnectingFailed
};
using namespace std;
#define BUFFER_SIZE 8192
#define NB_BARS 7
#define BIG_BUFFER




std::unordered_map<std::string, BehaviorFunc> behaviorMap = {
        {"MoveLeft", moveLeft},
        {"Zigzag", zigzagMovement},
        {"Circular", circularMovement},
        {"Diagonal", diagonalMovement},
        {"SineWave", sineWaveMovement},
        {"BackAndForth", backAndForthMovement},
        {"LShape", lShapeMovement},
        {"Random", randomMovement},
        {"SliderLoopFix", sliderLoopFixMovement},
        {"Fixed", fixedMovement},
        {"LoopDiagonalRight", loopDiagonalMovementRight}
        // Add more behaviors here
};
//vetctor of tech music frequency range


struct FFTActive{
    float frequencyAverages;
    float threshold;
    float lastSpawnTime;
    bool isOnCooldown;
    sf::RectangleShape bars;
    sf::RectangleShape thresholdBar;
};
struct SpawnEvent {
    float timestamp;  // Time in seconds
    float posY;       // Y position for spawn
    std::string behavior = "MoveLeft";
    std::string textureName = "e_ship1";
    entity_t entityId;
};

class FFTSystem : public ISystem<FFT> {
private:
    float &deltaTime;
    Registry& registry;  // Add this line
    FFTState state = FFTState::NOTLOADED;
    sf::SoundBuffer buffer;
    sf::Sound song;
    float globalAverage = 0.0f;
    float globalPeak = 0.0f;
    bool averageCalculated = false;
    bool needsUpdate = true;
    sf::Clock fftClock;  // Clock to manage FFT timing

    float sample_rate = 0;
    sf::Uint64 sample_count = 0;
    sf::Uint64 buffer_size = 0;
    std::string path = "asset/Song/music.wav"; //TODO: change to path to song
    std::string pathAnalised = "";
    std::vector<Complex> samples;
    fftwf_complex *in =(fftwf_complex*) fftw_malloc(sizeof(fftwf_complex) * BUFFER_SIZE);
    fftwf_complex *out = (fftwf_complex*) fftw_malloc(sizeof(fftwf_complex) * BUFFER_SIZE);
    fftwf_plan p = fftwf_plan_dft_1d(BUFFER_SIZE, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    std::vector<std::complex<float>> bin = std::vector<std::complex<float>>(BUFFER_SIZE / 2);
    std::vector<float> window;
    sf::Time duration = sf::seconds(0);
    sf::Time offsetTime = sf::seconds(0);
    float spawnCooldown = 1.0f; // Cooldown time in seconds
    // debug visual
    std::vector<FFTActive> FFTActiveVector;
    std::vector<SpawnEvent> precalculatedSpawns;
    float thresholdBass = 0.0f; // 20% above average
    float thresholdSubBass = 0.0f; // 20% above average

    std::vector<std::pair<float, float>> frequencyRange = {
            {0, 60},
            {60, 250},
            {250, 500},
            {500, 2000},
            {2000, 4000},
            {4000, 6000},
            {6000, 20000}
    };
public:


    float map_amplitude_to_radius(float amplitude) {
        return amplitude * 1000;
    }
    FFTSystem(float &deltaTime, Registry& registry) : deltaTime(deltaTime), registry(registry) {
        FFTActiveVector.resize(NB_BARS);  // Resize the vector to hold NB_BARS elements


        fftClock.restart();  // Restart the clock
    }

    void operator()(entity_t entity, sparse_array<FFT> &ffts) override {
        auto fftOpt = ffts[entity];
        if (!fftOpt){
            cout << "nop" << endl;
            return;
        }
        if (!needsUpdate) {
            return;
        }
        auto &fft = *ffts[entity];
        if (state == FFTState::NOTLOADED) {
            state = FFTState::LOADING;
            load();  // Load the song
            if (!averageCalculated) {
                analyzeSong();
                precalculateSpawns();
            }
            state = FFTState::LOADED;
            averageCalculated = false;
        } else if (state == FFTState::LOADING) {
            if (song.getStatus() == sf::SoundSource::Status::Playing) {
                state = FFTState::LOADED;
            }
        } else if (state == FFTState::LOADED) {
            if (song.getStatus() != sf::SoundSource::Status::Playing) {
                song.play();
            }
            update();  // Update the FFT calculations
            spawnEnemiesBasedOnFFT();
            // You could also call update_by_sound() here depending on your requirements.
        }
        needsUpdate = false;  // Set this to false after doing the FFT calculation
    }

    void spawnEnemiesBasedOnFFT() {
        float currentTimestamp = fftClock.getElapsedTime().asSeconds();
        auto it = precalculatedSpawns.begin();
        while (it != precalculatedSpawns.end()) {
            auto& event = *it;
            if (event.timestamp <= currentTimestamp) {
                registry.activate_entity(event.entityId);  // This will now also add the components
                std::cout << "Spawned entity " << event.entityId << " at time " << event.timestamp << std::endl;
                it = precalculatedSpawns.erase(it);  // Remove this event
            } else {
                break;  // Future events, no need to check further
            }
        }
    }



    void PostUpdate() override {
        needsUpdate = true;
        path = registry.getPath() + "/Song.wav";

        if (pathAnalised != path) {
            state = FFTState::NOTLOADED;
            fftClock.restart();
        }
    }

    void load() {
        std::cout << "Loading song: " << path << std::endl;
        state = FFTState::LOADING;
        std::ifstream ifs(path);
        if (!ifs.good())
        {
            cout << "Song \"" << path << "\" is not in the database." << endl;
            return;
        }
        buffer.loadFromFile(path);
        song.setBuffer(buffer);

        sample_count = buffer.getSampleCount();
        sample_rate = buffer.getSampleRate() * buffer.getChannelCount();
#ifdef BIG_BUFFER
        if (BUFFER_SIZE < sample_count)
    {
        buffer_size = BUFFER_SIZE;
    }
    else
    {
        buffer_size = sample_count;
    }
#endif

#ifndef BIG_BUFFER
        buffer_size = 1920;//TODO: change THIS
        std::cout << "buffer_size: " << buffer_size << std::endl;
#endif

        song.setLoop(true);
        samples.resize(buffer_size);
        create_hamming_window();
        duration = sf::seconds(static_cast<float>(sample_count) / sample_rate);
        std::cout << "Duration: " << duration.asSeconds() << std::endl;
    }

    bool update() {
        return true;
    }



    void create_hamming_window()
    {
        for (int i = 0; i < buffer_size; i++) {
            window.push_back(static_cast<float>(0.54) - static_cast<float>(0.46) * cos((2 * PI * i) / static_cast<float>(buffer_size)));
        }
    }


    void analyzeSong() {
        // Initialize variables
        if(averageCalculated) return;  // Add this line
        float sum = 0.0f;
        float peak = 0.0f;
        float numBins = 0;



        float sumBass = 0.0f;
        float countBass = 0;
        float sumSubBass = 0.0f;
        float countSubBass = 0;

        // Initialize vectors to hold sum and count for each frequency group
        std::vector<float> sumPerGroup(frequencyRange.size(), 0.0f);
        std::vector<float> numBinsPerGroup(frequencyRange.size(), 0);
        float binSize = static_cast<float>(sample_rate) / buffer_size;

        // Set the step for FFT analysis
        int analyzeStep = 1;
        std::cout << "Analyzing song...\n";
        // Loop through the song samples
        for (sf::Uint64 offset = 0; offset + buffer_size < sample_count; offset += analyzeStep * buffer_size) {
            // Prepare FFT input for this chunk
            for (sf::Uint64 i = 0; i < buffer_size; i++) {
                in[i][0] = static_cast<float>(buffer.getSamples()[offset + i]) * window[i];
                in[i][1] = 0.0;
            }

            // Execute FFTW3 plan
            fftwf_execute(p);

            // Calculate group size
            float subBassIntensity = update_by_sound(0, 60, 10000);
            sumSubBass += subBassIntensity;
            countSubBass++;

            // Calculate Bass intensity (60-250 Hz)
            float bassIntensity = update_by_sound(60, 250, 10000);
            sumBass += bassIntensity;
            countBass++;
            // Analyze this chunk

            for (sf::Uint64 i = 0; i < frequencyRange.size(); ++i) {
                float from = frequencyRange[i].first;
                float to = frequencyRange[i].second;

                sf::Uint64 fromIndex = static_cast<sf::Uint64>(floor(from / binSize));
                sf::Uint64 toIndex = static_cast<sf::Uint64>(floor(to / binSize));

                for (sf::Uint64 j = fromIndex; j < toIndex; ++j) {
                    float magnitude = std::abs(std::complex<float>(out[j][0], out[j][1]));
                    sumPerGroup[i] += magnitude;
                    numBinsPerGroup[i]++;
                }
            }
        }
        std::cout << "Finished analyzing song.\n";
        // Calculate global average and peak
        for (sf::Uint64 i = 0; i < NB_BARS; ++i) {
            sum += sumPerGroup[i];
            numBins += numBinsPerGroup[i];
            if (sumPerGroup[i] / numBinsPerGroup[i] > peak) {
                peak = sumPerGroup[i] / numBinsPerGroup[i];
            }
        }

        globalAverage = sum / numBins;
        globalPeak = peak;
        averageCalculated = true;
        std::cout << "Global average: " << globalAverage << std::endl;
        std::cout << "Global peak: " << globalPeak << std::endl;
        std::cout << "Number of bins: " << numBins << std::endl;
        // Calculate frequency averages
        for (int i = 0; i < NB_BARS; ++i) {
            FFTActive &active = FFTActiveVector[i];
            active.frequencyAverages = sumPerGroup[i] / numBinsPerGroup[i];
            active.threshold = (active.frequencyAverages + (globalPeak - active.frequencyAverages) * 0.2f);
            active.thresholdBar.setPosition(1900 - active.threshold / 35, active.thresholdBar.getPosition().y);
            std::cout << "frequency average for bar " << i << " : " << active.threshold  << std::endl;
        }
        float averageBass = sumBass / countBass;
        float averageSubBass = sumSubBass / countSubBass;

        // Use these averages to set the threshold for triggering spawns
        // For example:
        thresholdBass = averageBass * 1.2f; // 20% above average
        thresholdSubBass = averageSubBass * 1.2f; // 20% above average
        float dynamicThreshold = (globalAverage + (globalPeak - globalAverage) * 0.2f) / 10;
    }

    float update_by_sound(const float from, const float to, const float scale_factor)
    {
        float sum = 0.0f;
        float count = to - from;

        if (count <= 0) {
            std::cout << "Invalid frequency range." << std::endl;
            return 0.0f;
        }

        for (int i = from; i < to; ++i) {
            sum += std::abs(std::complex<float>(out[i][0], out[i][1]));
        }
        return (sum / count) / scale_factor;
    }

    void precalculateSpawns() {
        if (!averageCalculated) {
            std::cout << "Analyzing song first...\n";
            analyzeSong();
        }

        std::cout << "Precalculating spawns...\n";
        std::vector<float> lastSpawnTime(NB_BARS, 0.0f); // Initialize a vector to keep track of the last spawn time for each bar

        float currentTimestamp = 0.0f; // Initialize a variable to keep track of the current time in the song
        int analyzeStep = 1; // Analyze every frame
        float lastSpawnTimeSubBase = 0.0f; // Initialize a variable to keep track of the last spawn time for the base bar
        float lastSpawnTimeBase = 0.0f; // Initialize a variable to keep track of the last spawn time for the base bar
        float binSize = static_cast<float>(sample_rate) / buffer_size;

        // Clear any precalculated spawns
        precalculatedSpawns.clear();

        for (int offset = 0; offset + buffer_size < sample_count; offset += analyzeStep * buffer_size) {
            // Prepare FFT input for this chunk
            for (int i = 0; i < buffer_size; i++) {
                in[i][0] = static_cast<float>(buffer.getSamples()[offset + i]) * window[i];
                in[i][1] = 0.0;
            }

            // Execute FFTW3 plan
            fftwf_execute(p);

            // Calculate group size

            float subBassIntensity = 1.f + update_by_sound(0, 60, 10000);
           // std::cout << "subBassIntensity: " << subBassIntensity << std::endl;
            if (subBassIntensity > thresholdSubBass) {
                if (currentTimestamp - lastSpawnTimeSubBase >= spawnCooldown) {  // Check if enough time has passed for this bar
                    SpawnEvent event;
                    event.timestamp = currentTimestamp;
                    event.posY = 540;
                    event.behavior = "SineWave";
                    event.textureName = "e_ship3";
                    precalculatedSpawns.push_back(event);
                    lastSpawnTimeSubBase = currentTimestamp;  // Update the last spawn time for this bar
                }
            }
            float bassIntensity = 1.f + update_by_sound(60, 250, 10000);
           // std::cout << "bassIntensity: " << bassIntensity << std::endl;
            if (bassIntensity > thresholdBass ) {
                if (currentTimestamp - lastSpawnTimeBase >= spawnCooldown) {  // Check if enough time has passed for this bar
                    SpawnEvent event;
                    event.timestamp = currentTimestamp;
                    event.posY = 600;
                    event.behavior = "SineWave";
                    event.textureName = "e_ship2";
                    precalculatedSpawns.push_back(event);
                    lastSpawnTimeBase = currentTimestamp;  // Update the last spawn time for this bar
                }
            }


            // Analyze this chunk

// Then, in your loop:
            for (int i = 0; i < frequencyRange.size(); ++i) {
                float from = frequencyRange[i].first;
                float to = frequencyRange[i].second;

                sf::Uint64 fromIndex = static_cast<sf::Uint64>(floor(from / binSize));
                sf::Uint64 toIndex = static_cast<sf::Uint64>(floor(to / binSize));

                float sum = 0.0f;
                for (sf::Uint64 j = fromIndex; j < toIndex; ++j) {
                    float magnitude = std::abs(std::complex<float>(out[j][0], out[j][1]));
                    sum += magnitude;
                }

                // Calculate average magnitude for this frequency range
                float avg = sum / (toIndex - fromIndex);
                FFTActive &active = FFTActiveVector[i];

                // Use precalculated threshold to decide if an enemy should spawn
                if (avg >= active.threshold) {
                    if (currentTimestamp - lastSpawnTime[i] >= spawnCooldown) {  // Check if enough time has passed for this bar
                        SpawnEvent event;
                        event.timestamp = currentTimestamp;
                        event.posY = 100 + i * 100;
                        precalculatedSpawns.push_back(event);

                        lastSpawnTime[i] = currentTimestamp;  // Update the last spawn time for this bar
                    }
                }

            }

            currentTimestamp += (float)buffer_size / sample_rate; // Update the current time based on the buffer size and sample rate
            pathAnalised = path;
        }

        // Sort precalculatedSpawns by timestamp
        std::sort(precalculatedSpawns.begin(), precalculatedSpawns.end(),
                  [](const SpawnEvent& a, const SpawnEvent& b) {
                      return a.timestamp < b.timestamp;
                  });

        std::cout << "Finished precalculating spawns.\n";
        size_t totalSpawns = precalculatedSpawns.size();
        int i = 0;
        for (auto& event : precalculatedSpawns) {

           // std::cout << "Spawn event at " << event.timestamp << " seconds, y = " << event.posY << "Spawn :"<< i << " / " << totalSpawns << std::endl;
            i++;
        }
        //now prepare pool of enemies

        for (auto& event : precalculatedSpawns) {
            event.entityId = FFTSpawnEnemy(registry, event.behavior, 1750, event.posY, event.textureName, {0, 0, 128, 128}, {1.0f, 1.0f});
        }
        std::cout << "pathAnalised: " << pathAnalised << std::endl;

    }

    ~FFTSystem() override {

        fftwf_destroy_plan(p);
        fftw_free(in);
        fftw_free(out);
    }
};



class DrawSystem : public ISystem<Drawable, Position, Depth, TextDrawable> {
private:
    std::vector<entity_t> sortedEntities;  // Store sorted entities
    sf::RenderWindow& window;
    AssetManager& assetManager;
    Registry& registry;
    bool firstRun = true;
    SpritePool spritePool;

public:
    DrawSystem(sf::RenderWindow& window, AssetManager& assetManager, Registry& registry)
            : window(window), assetManager(assetManager), registry(registry) {
        invalidateSort();
    }
    void sortEntities(sparse_array<Drawable>& drawables, sparse_array<Depth>& depths, sparse_array<TextDrawable>& textDrawables) {
        sortedEntities.clear();
        //std::cout << "Sorting entities..." << std::endl;
        for (entity_t entity = 0; entity < drawables.size(); ++entity) {
            if ((drawables.is_active(entity) || textDrawables.is_active(entity)) && depths[entity]) {
                sortedEntities.push_back(entity);
            }
        }

        std::sort(sortedEntities.begin(), sortedEntities.end(), [&depths, &drawables](entity_t a, entity_t b) {
            if (depths[a]->z == depths[b]->z) {
                std::string aTexture = drawables[a] ? drawables[a]->textureName : "";
                std::string bTexture = drawables[b] ? drawables[b]->textureName : "";
                return aTexture < bTexture;
            }
            return depths[a]->z < depths[b]->z;
        });

    }

    void operator()(entity_t entity, sparse_array<Drawable>& drawables)  {
    }
    void operator()(entity_t entity, sparse_array<Drawable>& drawables, sparse_array<Position>& positions, sparse_array<Depth>& depths, sparse_array<TextDrawable>& textDrawables) override {
        if (firstRun) {
            sortEntities(drawables, depths, textDrawables);
            for (entity_t sortedEntity : sortedEntities) {
                auto drawableOpt = drawables[sortedEntity];
                auto posOpt = positions[sortedEntity];
                auto depthOpt = depths[sortedEntity];
                auto textDrawableOpt = textDrawables[sortedEntity];

                if (drawableOpt && posOpt && depthOpt) {
                    // Extract the actual components
                    auto& drawable = *drawableOpt;
                    auto& pos = *posOpt;

                    // Get a sprite from the pool
                    sf::Sprite* sprite = spritePool.getSprite();
                    sprite->setTexture(assetManager.GetTexture(drawable.textureName));
                    sprite->setPosition(pos.x, pos.y);
                    sprite->setScale(drawable.scale.x, drawable.scale.y);
                    sprite->setTextureRect(sf::IntRect(drawable.textureRect.left, drawable.textureRect.top, drawable.textureRect.width, drawable.textureRect.height));

                    #ifdef RENDER_HITBOX
                                        // Hitbox rendering code
                        sf::RectangleShape shape;
                        shape.setFillColor(sf::Color::Transparent);
                        shape.setOutlineColor(sf::Color::Red);
                        shape.setOutlineThickness(1);
                        shape.setPosition(sf::Vector2f(pos.x, pos.y));
                        shape.setSize(sf::Vector2f(drawable.textureRect.width, drawable.textureRect.height));
                        window.draw(shape);
                    #endif
                    window.draw(*sprite);
                }
                if (textDrawableOpt && posOpt) {
                    auto& textDrawable = *textDrawableOpt;
                    auto& pos = *posOpt;

                    sf::Text text;
                    text.setFont(assetManager.GetFont(textDrawable.fontName));  // Set the font
                    text.setString(textDrawable.text);  // Set the string
                    text.setCharacterSize(textDrawable.fontSize);  // Set the character size
                    text.setFillColor(textDrawable.color);  // Set the color

                    // Get the bounding box of the text
                    sf::FloatRect textBounds = text.getLocalBounds();

                    // Get the dimensions of the sprite
                    float spriteWidth = 100.0f;
                    float spriteHeight  = 100.0f;
                    if (drawableOpt)
                    {
                        auto& drawable = *drawableOpt;
                        spriteWidth = drawable.textureRect.width * drawable.scale.x;
                        spriteHeight = drawable.textureRect.height * drawable.scale.y;
                    }
                    // Scale the text to fit within the sprite, if necessary
                    if (textBounds.width > spriteWidth || textBounds.height > spriteHeight) {
                        float xScale = spriteWidth / textBounds.width;
                        float yScale = spriteHeight / textBounds.height;
                        float minScale = std::min(xScale, yScale);
                        text.setCharacterSize(static_cast<unsigned int>(textDrawable.fontSize * minScale));
                    }

                    // Re-calculate the bounding box if you've changed the character size
                    textBounds = text.getLocalBounds();

                    // Position the text in the middle of the sprite
                    text.setPosition(pos.x + (spriteWidth - textBounds.width) / 2, pos.y + (spriteHeight - textBounds.height) / 2);

                    window.draw(text);
                }
            }

            firstRun = false;
        }
    }


    // Call this whenever you modify the Depth component of any entity
    void invalidateSort() {
        firstRun = true;
    }
    void PostUpdate() override {
        invalidateSort();
    }
};



class VelocitySystem : public ISystem<Position, Velocity> {
private:
    float& deltaTime;

public:
    VelocitySystem(float& dt) : deltaTime(dt) {}

    void operator()(entity_t entity, sparse_array<Position>& positions, sparse_array<Velocity>& velocities) override {

        auto posOpt = positions[entity];
        auto velOpt = velocities[entity];

        if (posOpt.has_value() && velOpt.has_value()) {
            auto& pos = *positions[entity];
            auto& vel = *velocities[entity];
            pos.x += vel.x * deltaTime * 100.0f;
            pos.y += vel.y * deltaTime * 100.0f;
            if (pos.x < 0) {
                pos.x = 0;
            }
            if (pos.y < 0) {
                pos.y = 0;
            }
        }
    }
};


class ControlSystem : public ISystem<Position, Velocity, Controllable, GameConfig> {
    private:
        float& deltaTime;
     Registry& registry;  // Add this line
    std::unordered_map<entity_t, std::queue<Event>> eventQueue;  // Queue to hold events for each entity

public:
        ControlSystem(float& deltaTime, Registry& registry) : deltaTime(deltaTime), registry(registry) {
            registry.eventBus.addListener(PlayerMoved, [this, &registry](const Event& event) {
                entity_t entity = registry.clientToEntityMap[event.clientID];
                if (entity != 0) {
                    eventQueue[entity].push(event);
                }
            });
        }  // Modify this line
        void operator()(entity_t entity, sparse_array<Position>& positions, sparse_array<Velocity>& velocities, sparse_array<Controllable>& controllables, sparse_array<GameConfig>& gameConfigs) override {
            auto controllableOpt = controllables[entity];
            auto velOpt = velocities[entity];
            auto gameConfigOpt = gameConfigs[entity];
            if (gameConfigs.empty()) {
                std::cerr << "GameConfig not set. Exiting addControlSystem." << std::endl;
                return;
            }
            if (controllableOpt.has_value())
            {
                auto& controllable = *controllables[entity];
                if (!controllable.isLocal) {
                    auto& vel = *velocities[entity];
                    auto& config = *gameConfigs[entity];
                    float tempX = 0.0f;
                    float tempY = 0.0f;
                    while (!eventQueue[entity].empty()) {
                        const Event& event = eventQueue[entity].front();
                        if (registry.clientToEntityMap[event.clientID] != entity) {
                            eventQueue[entity].pop();//not sure if this is needed
                            continue;
                        }
                        if (event.type == PlayerMoved) {
                            auto& config = *gameConfigs[entity];
                            if (event.content == "i")
                            {
                                vel.y = -1.0 * config.playerSpeed * deltaTime;
                            }
                            else if (event.content == "k")
                            {
                                vel.y = 1.0 * config.playerSpeed * deltaTime;
                            }

                            if (event.content == "j")
                            {
                                vel.x = -1.0f * config.playerSpeed  * deltaTime;
                            }
                            else if (event.content == "l")
                            {
                                vel.x = 1.0f * config.playerSpeed  * deltaTime;
                            }
                        }

                        eventQueue[entity].pop();
                    }
                    return;
                }

            }
            if (velOpt.has_value() && gameConfigOpt.has_value()) {
                auto& vel = *velocities[entity];
                auto& config = *gameConfigs[entity];
                // Use the global config
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
                    vel.y = -1.0 * config.playerSpeed * deltaTime;
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                    vel.y = 1.0 * config.playerSpeed * deltaTime;
                } else {
                    vel.y = 0.0f;
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
                    vel.x = -1.0f * config.playerSpeed  * deltaTime;
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                    vel.x = 1.0f * config.playerSpeed  * deltaTime;
                } else {
                    vel.x = 0.0f * config.playerSpeed  * deltaTime;
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
                    float y = rand() % 720 + 100;
                    FFTSpawnEnemy(registry, "MoveLeft", 900, y, "e_ship1", {0, 0, 128, 128}, {1.0f, 1.0f});
                }
            }
        }
};

class AnimationSystem : public ISystem<Animation, Drawable> {
private:
    float& deltaTime;

public:
    AnimationSystem(float& deltaTime) : deltaTime(deltaTime) {}

    void operator()(entity_t e, sparse_array<Animation>& animations, sparse_array<Drawable>& drawables) override {

        auto animOpt = animations[e];
        auto drawableOpt = drawables[e];
        if (animOpt.has_value()) {
            auto& anim = *animations[e];
            anim.elapsedTime += deltaTime;

            if (anim.elapsedTime >= anim.frameDuration) {
                anim.elapsedTime = 0.0f;

                if (anim.loop) {
                    // Normal looping behavior
                    anim.currentFrame = (anim.currentFrame + 1) % anim.numFrames;
                } else {
                    // If not looping, stop at the last frame
                    if (anim.currentFrame < anim.numFrames - 1) {
                        anim.currentFrame++;
                    }
                }

                // Update the texture rect if drawable component exists
                if (drawableOpt.has_value()) {
                    auto& drawable = *drawables[e];
                    drawable.textureRect.left = drawable.textureRect.width * anim.currentFrame;
                }
            }
        }
    }
};


class SpawnSystem : public ISystem<Spawner, Position> {
private:
    float& deltaTime;
    AssetManager& assetManager;
    Registry& registry;

public:
    SpawnSystem(float& deltaTime, AssetManager& assetManager, Registry& registry) : deltaTime(deltaTime), assetManager(assetManager), registry(registry) {}
    void operator()(entity_t entity, sparse_array<Spawner>& spawners, sparse_array<Position>& positions) override {

        auto spawnOpt = spawners[entity];
        auto posOpt = positions[entity];

        if (spawnOpt) {
            auto& spawn = *spawners[entity];
            float posX = 0;
            float posY = 0;
            if (posOpt) {
                auto& pos = *positions[entity];
                posX = pos.x;
                posY = pos.y;
            }

            spawn.timeSinceLastSpawn += deltaTime;
            float timeRequiredForNextSpawn = 1.0f / spawn.spawnRate;

            if (spawn.timeSinceLastSpawn >= timeRequiredForNextSpawn) {
                spawn.timeSinceLastSpawn = 0.0f;  // Reset timer
                auto projectile = registry.spawn_entity();

                registry.add_component(projectile, Position{posX + spawn.xOffset, posY + spawn.yOffset});
                for (const auto& addComponent : spawn.componentSpawners) {
                    addComponent(projectile, registry);  // Apply each ComponentSpawner to the new entity
                }

            }
        }
    }
};


class BehaviorSystem : public ISystem<Behavior, Position> {
private:
    float& deltaTime;

public:
    BehaviorSystem(float& deltaTime) : deltaTime(deltaTime) {}

    void operator()(entity_t e, sparse_array<Behavior>& behaviors, sparse_array<Position>& positions) override {

        auto behaviorOpt = behaviors[e];
        auto posOpt = positions[e];

        if (behaviorOpt && posOpt) {
            auto& behavior = *behaviors[e];
            auto& pos = *positions[e];

            // Look up the behavior function from the map
            auto it = behaviorMap.find(behavior.behaviorID);
            if (it != behaviorMap.end()) {
                // Execute the behavior function
                it->second(pos, deltaTime);
            }
        }
    }
};
class FollowerSystem : public ISystem<Follower, Position> {
public:
    void operator()(entity_t entity, sparse_array<Follower>& followers, sparse_array<Position>& positions) override {
        auto followerOpt = followers[entity];
        auto posOpt = positions[entity];
        if (followerOpt && posOpt) {
            auto posTargetOpt = positions[followerOpt->target];
            auto& follower = *followers[entity];
            auto& pos = *positions[entity];
            if (posTargetOpt) {
                auto& posTarget = *positions[follower.target];
                pos.x = posTarget.x + follower.xOffset;
                pos.y = posTarget.y + follower.yOffset;
            }
        }
    }
};

struct lobby {
    std::string name;
    int id;
    int playerCount;
    std::string artistName;
    std::string songName;
    //id in the lobby/game and the entity id
    std::vector<std::pair<int, entity_t>> players;
};



class ServerSystem : public ISystem<Network> {
private:
    float& deltaTime;
    std::vector<std::pair<struct sockaddr_in, int>> clients;
    int clientIdCounter = 0;
    int tcp_sockfd;
    int udp_sockfd;
    Registry& registry;
    std::unordered_map<EventType, std::function<void(const Event&)>> eventHandlerMap;
    std::vector<std::vector<int>> clientToEntityMap;
    std::vector<lobby> activeLobbies; // To hold active lobbies
    int nextLobbyId = 1; // To generate unique lobby IDs
    bool newPlayerToAdd = false;
    bool newLobbyToAdd = false;
    //content, id of client, id of lobby
    std::tuple <std::string, int, int> contentIDLobbyCreation;
public:
    ServerSystem(Registry& registry, float& deltaTime) : registry(registry), deltaTime(deltaTime) {
        tcp_sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (tcp_sockfd == -1) {
            perror("socket");
            // handle error
            return;
        }

        struct sockaddr_in serverAddr;
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(12345);  // Replace with your port number
        serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  // Replace with your IP address

        if (bind(tcp_sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
            perror("bind");
            // handle error
            return;
        }

        if (listen(tcp_sockfd, 5) == -1) {
            perror("listen");
            // handle error
            return;
        }

        int flags = fcntl(tcp_sockfd, F_GETFL, 0);
        fcntl(tcp_sockfd, F_SETFL, flags | O_NONBLOCK);

        udp_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (udp_sockfd == -1) {
            perror("UDP socket");
            // handle error
            return;
        }

        struct sockaddr_in udp_serverAddr;
        memset(&udp_serverAddr, 0, sizeof(udp_serverAddr));
        udp_serverAddr.sin_family = AF_INET;
        udp_serverAddr.sin_port = htons(12346);  // Replace with your UDP port number
        udp_serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  // Replace with your IP address

        if (bind(udp_sockfd, (struct sockaddr *)&udp_serverAddr, sizeof(udp_serverAddr)) == -1) {
            perror("UDP bind");
            // handle error
            return;
        }

        int udp_flags = fcntl(udp_sockfd, F_GETFL, 0);
        fcntl(udp_sockfd, F_SETFL, udp_flags | O_NONBLOCK);
        registry.eventBus.addListener(PlayerJoined, [this, &registry](const Event& event) {
            std::cout << "PlayerJoined event received" << std::endl;
            //we will add a new entity to the game using spawnedNonLocalPlayer
            newPlayerToAdd = true;
        });
        registry.eventBus.addListener(PlayerLeft, [this, &registry](const Event& event) {
            std::cout << "PlayerLeft event received" << std::endl;
            //we will remove the entity from the game
            //we will also remove the client from the clientToEntityMap
            //we will also remove the client from the lobby
        });
        registry.eventBus.addListener(PlayerMoved, [this, &registry](const Event& event) {
            std::cout << "PlayerMoved event received serverSystem side" << std::endl;

        });
        registry.eventBus.addListener(AskDataCreationLobby, [this, &registry](const Event& event) {
            // Handle lobby creation
            //check if the client id is already in the lobby and if its present in the clientToEntityMap
            //if its not in the map we will refuse
            //if its in the map we will add it to the lobby
            std::cout << "AskDataCreationLobby event received serverSystem side" << std::endl;
            newLobbyToAdd = true;
            contentIDLobbyCreation = std::make_tuple(event.content, event.clientID, 1);
            std::cout << "contentIDLobbyCreation: " << std::get<0>(contentIDLobbyCreation) << std::endl;
            std::cout << "contentIDLobbyCreation: " << std::get<1>(contentIDLobbyCreation) << std::endl;
            std::cout << "contentIDLobbyCreation: " << std::get<2>(contentIDLobbyCreation) << std::endl;
        });
        registry.eventBus.addListener(LobbyJoined, [this, &registry](const Event& event) {
            // Handle joining a lobby
            int lobbyId = std::stoi(event.content);
            // Add the player to the corresponding lobby
            // ... (lookup lobby and add player)
            for (auto& lobby : activeLobbies) {
                if (lobby.id == lobbyId) {
                    lobby.players.push_back({lobby.playerCount, registry.clientToEntityMap[event.clientID]});
                    lobby.playerCount++;
                    // Send a LobbyJoined event
                    std::cout << "LobbyJoined event sent" << std::endl;
                    return;
                }
            }

        });

        eventHandlerMap[LobbyLeft] = [this](const Event& event) {
            // Handle leaving a lobby
            int lobbyId = std::stoi(event.content);
            // Remove the player from the lobby
            // ... (lookup lobby and remove player)
        };
        eventHandlerMap[LobbyDestroyed] = [this] (const Event& event) {
            // Handle destroying a lobby
            int lobbyId = std::stoi(event.content);
            // Remove the lobby from the list of active lobbies
            // ... (lookup lobby and remove it)
        };
    }


    void add_client(struct sockaddr_in new_client) {
        for (const auto &pair : clients) {
            if (pair.first.sin_addr.s_addr == new_client.sin_addr.s_addr && pair.first.sin_port == new_client.sin_port) {
                return; // Client already exists
            }
        }
        clientIdCounter++;
        clients.push_back({new_client, clientIdCounter});
    }

    void broadcast(const char *message, ssize_t len, struct sockaddr_in *exclude_addr = nullptr) {
        for (const auto &pair : clients) {
            const struct sockaddr_in &client = pair.first;
            if (exclude_addr && client.sin_addr.s_addr == exclude_addr->sin_addr.s_addr && client.sin_port == exclude_addr->sin_port) {
                continue; // Skip sending back to the source client
            }
            sendto(udp_sockfd, message, len, 0, (struct sockaddr *)&client, sizeof(client));
        }
    }

    void operator()(entity_t entity, sparse_array<Network>& networks) override {
        //priuint the content of the lobby actually active
        for (auto& lobby : activeLobbies) {
          //  std::cout << "Lobby " << lobby.id << " has " << lobby.playerCount << " players" << std::endl;
        }
        // Listen for new TCP connections
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int client_sock = accept(tcp_sockfd, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (client_sock != -1) {
            // Handle TCP Client here or in a separate function
            // For example, send initial game state, etc.
            std::thread client_thread([this, client_sock](){
                char buffer[1024];
                ssize_t bytes_received = recv(client_sock, buffer, 1023, 0);
                if (bytes_received <= 0) {
                    perror("recv");
                    close(client_sock);
                    return;
                }
                buffer[bytes_received] = '\0';
                std::cout << "Received (TCP): " << buffer << std::endl;
                close(client_sock);
            });
            client_thread.detach();
        }  else {
            if (errno != EWOULDBLOCK && errno != EAGAIN) {
                // Handle error
                std::cerr << "Error accepting TCP connection: " << strerror(errno) << std::endl;
            }
        }

        char udp_buffer[1024];
        struct sockaddr_in udp_clientAddr;
        socklen_t udp_clientAddrLen = sizeof(udp_clientAddr);
        ssize_t recvLen = recvfrom(udp_sockfd, udp_buffer, 1024, 0, (struct sockaddr *)&udp_clientAddr, &udp_clientAddrLen);
        if (recvLen != -1) {
            udp_buffer[recvLen] = '\0';
            add_client(udp_clientAddr);

            int client_id = -1;
            for (const auto &pair : clients) {
                if (pair.first.sin_addr.s_addr == udp_clientAddr.sin_addr.s_addr && pair.first.sin_port == udp_clientAddr.sin_port) {
                    client_id = pair.second;
                    break;
                }
            }

            if (client_id != -1) {
                std::string receivedMessage(udp_buffer);
                std::string eventTypeStr = receivedMessage.substr(0, receivedMessage.find(":"));
                std::string eventContent = receivedMessage.substr(receivedMessage.find(":") + 1);
                // Lookup the EventType based on the identifier
                auto eventTypeIt = eventIdentifierMap.find(eventTypeStr);
                if (eventTypeIt != eventIdentifierMap.end()) {
                    EventType eventType = eventTypeIt->second;
                    Event newEvent;
                    newEvent.type = eventType;
                    //check if the event exist in the map
                    std::cout << "Received from client_" << client_id << " (UDP): " << udp_buffer << std::endl;
                    newEvent.content = std::string(eventContent);
                    newEvent.clientID = client_id;
                    std::cout << "firing event " << newEvent.type << " with content " << newEvent.content << " and clientID " << newEvent.clientID << std::endl;
                    registry.eventBus.fireEvent(newEvent);
                } else {
                    std::cout << "Unknown event type received from client_" << client_id << " (UDP): " << udp_buffer << std::endl;
                }
            } else {
                std::cout << "Received from unknown client (UDP): " << udp_buffer << std::endl;
            }

            broadcast(udp_buffer, recvLen, &udp_clientAddr);
        }

        // Your game logic here
        // For example, update game state and broadcast it
    }
    void PostUpdate() override {
        if (newPlayerToAdd) {
            entity_t new_entity = registry.spawn_entity();
            Client clientComponent;
            clientComponent.id = clientIdCounter;
            clientComponent.entity_id = new_entity;
            std::cout << "new player id new_entity: " << new_entity << std::endl;
            registry.add_component(new_entity,std::move(clientComponent));
            registry.clientToEntityMap[clientIdCounter] = new_entity;
            newPlayerToAdd = false;
        }
        if (newLobbyToAdd){
            if (registry.clientToEntityMap.find(std::get<1>(contentIDLobbyCreation)) == registry.clientToEntityMap.end()) {
                // Client not found in the map
                // Send a LobbyJoinFailed event
                std::cout << "Client " << std::get<1>(contentIDLobbyCreation) << " not found in the map" << std::endl;
                return;
            }
            // now check if the client is already in a lobby
            for (auto& lobby : activeLobbies) {
                for (auto& player : lobby.players) {
                    if (player.first == std::get<1>(contentIDLobbyCreation)) {
                        // Client already in a lobby
                        // Send a LobbyJoinFailed event
                        std::cout << "Client " << std::get<1>(contentIDLobbyCreation) << " already in a lobby" << std::endl;
                        return;
                    }
                }
            }
            std::cout << "Client " << std::get<1>(contentIDLobbyCreation) << " is not in a lobby" << std::endl;
            lobby newLobby;
            newLobby.id = nextLobbyId++;
            newLobby.name = "Lobby " + std::to_string(newLobby.id);
            newLobby.playerCount = 1;
            newLobby.artistName = std::get<0>(contentIDLobbyCreation);
            newLobby.songName = "songName";
            newLobby.players.push_back({1, registry.clientToEntityMap[std::get<1>(contentIDLobbyCreation)]});
            std::cout << "event content : " << std::get<0>(contentIDLobbyCreation) << std::endl;
            // Add more initializations
            activeLobbies.push_back(newLobby);
            //lets now send the messge back to the client

            std::string message = "LC:" + std::to_string(newLobby.id) + " " + newLobby.name + " " + newLobby.artistName + " " + newLobby.songName;
            std::cout << "message: " << message << std::endl;
            //add a send to client using the event.clientID
            std::cout << "client id: " << std::get<1>(contentIDLobbyCreation) << std::endl;
            sendto(udp_sockfd, message.c_str(), message.length(), 0, (struct sockaddr *)&clients[std::get<1>(contentIDLobbyCreation) - 1].first, sizeof(clients[std::get<1>(contentIDLobbyCreation) - 1].first));
            newLobbyToAdd = false;

        }
    }
};


class ClientSystem : public ISystem<State> {
private:
    int tcp_sockfd;
    int udp_sockfd;
    std::thread listener_thread;
    Registry& registry;
    float& deltaTime;
    struct sockaddr_in serverAddr;
    bool isConnected = false;
    bool isTryingToConnect = false;
    bool askForLobby = false;
    std::string requestContent = "";
public:
    ClientSystem(Registry& registry, float& deltaTime) : registry(registry), deltaTime(deltaTime) {
        registry.eventBus.addListener(PlayerMoved, [this](const Event& event) {
            // Your code to handle PlayerMoved event
            std::cout << "PlayerMoved event received" << std::endl;
        });
        registry.eventBus.addListener(ClientTryConnect, [this](const Event& event) {
            // will try to connect to the server
            std::cout << "ClientTryConnect event received" << std::endl;
            isTryingToConnect = true;
        });
        registry.eventBus.addListener(AskDataCreationLobby, [this](const Event& event) {
            // Your code to handle LobbyCreated event
            std::cout << "LobbyCreated event received" << std::endl;
            askForLobby = true;
            requestContent = event.content;
        });
    }

    int tryToJoin()
    {
        std::cout << "ClientSystem constructor" << std::endl;
        // Initialize TCP
        std::string serverIP = readFromConfig("server_ip");
        int tcpPort = std::stoi(readFromConfig("tcp_port"));

        tcp_sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (tcp_sockfd == -1) {
            perror("TCP socket");
            return 1;
        }
        // Error handling
        std::cout << "serverIP: " << serverIP << std::endl;
        std::cout << "tcpPort: " << tcpPort << std::endl;

        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(tcpPort);
        serverAddr.sin_addr.s_addr = inet_addr(serverIP.c_str());

        if (connect(tcp_sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
            perror("TCP connect");
            return 1;
        }
        // Error handling

        // Initialize UDP
        int udpPort = std::stoi(readFromConfig("udp_port"));
        udp_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (udp_sockfd == -1) {
            perror("UDP socket");
            return 1;
        }        // Error handling
        std::cout << "udpPort: " << udpPort << std::endl;

        if (listener_thread.joinable()) {
            listener_thread.join();  // Or listener_thread.detach();
        }
        listener_thread = std::thread(&ClientSystem::listen_to_server, this);


        // Fire an initial event to inform the server that a new player has joined.
        Event newEvent;
        newEvent.type = PlayerJoined;  // Assuming you have an EventType for this
        registry.eventBus.fireEvent(newEvent);

        // Send "PJ:" message to server
        send_message("PJ:");
        isConnected = true;
        return 0;
    }

    ~ClientSystem() {
        listener_thread.join();
        close(udp_sockfd);
        close(tcp_sockfd);
    }

    void listen_to_server() {
        char buffer[1024];
        while (true) {
            struct sockaddr_in serverAddr;
            socklen_t serverAddrLen = sizeof(serverAddr);
            ssize_t recvLen = recvfrom(udp_sockfd, buffer, 1024, 0, (struct sockaddr*)&serverAddr, &serverAddrLen);
            // Error handling

            buffer[recvLen] = '\0';
            std::string receivedMessage(buffer);
            std::cout << "Received from server: " << buffer << std::endl;
            std::string eventTypeStr = receivedMessage.substr(0, receivedMessage.find(":"));
            std::string eventContent = receivedMessage.substr(receivedMessage.find(":") + 1);
            Event newEvent;
            //might need to add a spliter to the buffer to get the event type
            newEvent.type = parseEventType(eventTypeStr);
            registry.eventBus.fireEvent(newEvent);
        }
    }

    void send_message(const std::string& message) {
        struct sockaddr_in serverAddr;
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(std::stoi(readFromConfig("udp_port")));
        serverAddr.sin_addr.s_addr = inet_addr(readFromConfig("server_ip").c_str());
        std::cout << "trying to send message: " << message << std::endl;
        if (sendto(udp_sockfd, message.c_str(), message.length(), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
            perror("UDP sendto");
        }
    }

    void operator()(entity_t entity, sparse_array<State>& states) override {
        // Game logic and event triggering
        if (!isConnected) {
            return;
        }
/*
        char input = getch();
        //check if content is either i,j,k,l
        if (input != 'i' && input != 'j' && input != 'k' && input != 'l') {
            return;
        }
        Event newEvent;
        newEvent.type = PlayerMoved; // Function to map input to EventType
        registry.eventBus.fireEvent(newEvent);

        std::string message = "PM:";  // For example, PM for PlayerMoved
        message += input;
        send_message(message);
        */
    }

    void PostUpdate() override {
        if (isTryingToConnect) {
            isTryingToConnect = false;
            if (tryToJoin() == 0) {
                std::cout << "Connected to server" << std::endl;
            }
        }
        if (askForLobby) {
            askForLobby = false;
            std::cout << "Asking for lobby" << std::endl;
            send_message("ADCL: " + requestContent);
            requestContent = "";
        }
    }

    // The rest of your code, including utility functions like getch, mapInputToEventType, parseEventType
};



class ClickSystem : public ISystem<isClickable, Position> {
private:
    sf::RenderWindow& window;
    Registry& registry;
    float& deltaTime;
public:
    ClickSystem(sf::RenderWindow& window, Registry& registry, float& deltaTime) : window(window), registry(registry), deltaTime(deltaTime) {}

    void operator()(entity_t entity, sparse_array<isClickable>& clickables, sparse_array<Position>& positions) override {
        auto clickableOpt = clickables[entity];
        auto posOpt = positions[entity];
        auto drawableOpt = registry.get_components<Drawable>()[entity];  // Assuming yourRegistry is available

        if (clickableOpt && posOpt) {
            auto& clickable = *clickables[entity];
            auto& pos = *positions[entity];
            float width = drawableOpt ? drawableOpt->textureRect.width : 50.0f;  // Replace 50.0f with some default
            float height = drawableOpt ? drawableOpt->textureRect.height : 50.0f;  // Replace 50.0f with some default

            sf::Vector2i mousePos = sf::Mouse::getPosition(window);  // Get mouse position
            bool isMousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);  // Check for left click
#ifdef RENDER_HITBOX
            std::cout << "mousePos.x: " << mousePos.x << std::endl;
            std::cout << "mousePos.y: " << mousePos.y << std::endl;
#endif

            clickable.elapsedTimeSinceLastClick += deltaTime;

            if (isMousePressed && mousePos.x >= pos.x && mousePos.x <= pos.x + width && mousePos.y >= pos.y && mousePos.y <= pos.y + height) {
                // Don't immediately set isClicked to true; check elapsed time
                if (clickable.elapsedTimeSinceLastClick >= 0.2f) {  // assuming 0.02s cooldown
                    clickable.isClicked = true;
                    // Reset the elapsed time only here
                     clickable.elapsedTimeSinceLastClick = 0.0f;
                }
            }

// Update the elapsed time even if not clicking, allowing it to go beyond one frame
            if (clickable.isClicked) {
                clickable.onClick();
                clickable.isClicked = false;  // Reset after handling
            }
        }
    }
};






#endif /* !SYSTEM_HPP_ */

