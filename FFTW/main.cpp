    #include <SFML/Graphics.hpp>
    #include <SFML/Audio.hpp>
    #include <fftw3.h>
    #include <iostream>
    #include <cmath>
    #include <vector>
    #include <thread>
    #include <chrono>
    #include <complex>

    using Complex = std::complex<double>;
    using ComplAr = std::valarray<Complex>;
    const float PI = static_cast<float>(3.14159265358979323846);
    using namespace std;
    using namespace sf;
    const int BUFFER_SIZE = 8192;
    const int NUM_BINS = 80;
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;
    const int MAX_RADIUS = 200;
    const int BAR_WIDTH = 10;
    std::vector<sf::VertexArray> halo;
    std::vector<sf::Color> halo_colors = {sf::Color::Black, sf::Color::White, sf::Color::Black, sf::Color::White};
    std::vector<float> halo_heights = {1.1f, 1.2f, 1.3f, 1.4f};  // Increased values
    sf::Vector2f center(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    float base_radius = 150.0f;  // Reduced from 250.0f

    std::vector<std::pair<double, double>> frequencyRanges = {
            {20, 60},     // Sub-Bass
            {60, 250},    // Bass Line
            {250, 500},   // Low Mids
            {500, 2000},  // Mids
            {2000, 4000}, // Upper Mids
            {4000, 6000}, // Presence
            {6000, 20000} // Brilliance/High Frequencies

    };



    std::vector<std::pair<int, int>> convertFrequencyRangesToBinRanges(const std::vector<std::pair<double, double>>& freqRanges, double sampleRate, int bufferSize) {
        std::vector<std::pair<int, int>> binRanges;
        for (const auto& range : freqRanges) {
            int binFrom = static_cast<int>(std::round(range.first * bufferSize / sampleRate));
            int binTo = static_cast<int>(std::round(range.second * bufferSize / sampleRate));
            binRanges.emplace_back(binFrom, binTo);
        }
        return binRanges;
    }

    void applyHammingWindow(double* windowedSamples, const sf::Int16* samples, std::size_t size) {
        for (std::size_t i = 0; i < size; ++i) {
            windowedSamples[i] = samples[i] * (0.54 - 0.46 * std::cos(2 * M_PI * i / (size - 1)));
        }
    }

    int calculateDifficulty(const sf::Int16* samples, std::size_t sampleCount) {
        double totalMagnitude = 0;
        for (std::size_t i = 0; i < sampleCount; ++i) {
            totalMagnitude += std::abs(samples[i]);
        }
        int difficulty = static_cast<int>(totalMagnitude / sampleCount) % 11;
        return difficulty;
    }
    float update_by_sound(const int from, const int to, const int scale_factor, const std::vector<std::complex<double>>& fftResults, float minAmplitude, float maxAmplitude)
    {
        float sum = 0;
        for (int i = from; i < to; ++i)
        {
            sum += std::abs(fftResults[i]);  // Compute the magnitude of the complex number
        }

        float avgAmplitude = (sum / (to - from)) / scale_factor;
        if (maxAmplitude != minAmplitude) {  // To avoid division by zero
            avgAmplitude = (avgAmplitude - minAmplitude) / (maxAmplitude - minAmplitude);
        }
        // Apply logarithmic scaling and threshold
        float threshold = 0.1;  // Adjust as needed
        if (avgAmplitude > threshold) {
            if (avgAmplitude > 0) {
                avgAmplitude = std::log10(avgAmplitude) * 10;
            }
            return avgAmplitude;
        } else {
            return 0.0;
        }
    }


    void frequency_spectrum_round(std::vector<sf::VertexArray>& VAs,
                                  std::vector<sf::Color>& colors,
                                  const std::vector<float>& heights,
                                  const sf::Vector2f& center,
                                  const float& radius,
                                  const int& from,
                                  const int& to,
                                  const std::vector<std::complex<double>>& fftResults
                                  ) // Added this parameter
    {
        if (VAs.size() != colors.size() || VAs.size() != heights.size())
        {
            std::cout << "VAs.size() = " << VAs.size() << std::endl;
            std::cout << "colors.size() = " << colors.size() << std::endl;
            std::cout << "heights.size() = " << heights.size() << std::endl;
            throw invalid_argument("number of VertexArrays is not the same as number of colors");
        }
        if(from == 0)
        {
            throw invalid_argument("spectrum cant start at 0, set different 'from' value");
        }

        const float scale = float(to-from) / 180.f;		//divided by angle (in degrees) that the scale should cover (360 - circle; 180 - half a circle)

        for (int i = 0; i < VAs.size(); i++)
        {
            VAs[i].append(Vertex(center, colors[i]));
        }
        for(int j = 0; j < VAs.size(); j++) {
            float new_radius = radius;
            float angle;
            Vector2f cartez;
            Vector2f base_position;

            for (float i = static_cast<float>(from); i < static_cast<float>(to); i *= static_cast<float>(1.01)) {
                // Here, replace 'bin' with 'fftResults'
                int index = static_cast<int>(i);
                if(index >= 0 && index < fftResults.size()) {
                    new_radius = radius * static_cast<float>(1 + (abs(fftResults[index]) / 10000000 * heights[j]));
                }
                angle = ((i - from) / scale + 90) * PI / 180;
                cartez = Vector2f(new_radius * cos(angle), new_radius * sin(angle)); //polar coordinates to cartez
                base_position = center + cartez;
                VAs[j].append(Vertex(base_position, colors[j]));
                //std::cout << "Accessing fftResults at index: " << static_cast<int>(i) << std::endl;

            }

            angle = (270 * PI / 180);
            cartez = Vector2f(new_radius * cos(angle), new_radius * sin(angle));
            VAs[j].append(Vertex(center + cartez));

            //generating right side of the halo
            for (float i = static_cast<float>(from); i< static_cast<float>(to); i *= static_cast<float>(1.01))
            {
                int index = static_cast<int>(i);
                if(index >= 0 && index < fftResults.size()) {
                    new_radius = radius * static_cast<float>(1 + (abs(fftResults[index]) / 10000000 * heights[j]));
                }
                angle = (-(i - from) / scale + 90) * PI / 180;
                cartez = Vector2f(new_radius * cos(angle), new_radius * sin(angle));
                base_position = center + cartez;
                VAs[j].append(Vertex(base_position, colors[j]));
                // std::cout << "Accessing fftResults at index: " << static_cast<int>(i) << std::endl;
            }
            //last vertex for completing full semicircle
            angle = -(90 * PI / 180);
            cartez = Vector2f(new_radius * cos(angle), new_radius * sin(angle));
            VAs[j].append(Vertex(center + cartez));
        }

    }
    void populateFFTResults(fftw_complex* out, std::vector<std::complex<double>>& fftResults) {
        for (std::size_t i = 0; i < fftResults.size(); ++i) {
            std::complex<double> c(out[i][0], out[i][1]);
            fftResults[i] = c;
        }
    }


    void performFFT(const double* windowedSamples, fftw_complex* in, fftw_complex* out, fftw_plan& p, std::vector<std::complex<double>>& fftResults) {
        // Populate the 'in' array and execute FFT
        for (std::size_t i = 0; i < BUFFER_SIZE; ++i) {
            in[i][0] = windowedSamples[i];
            in[i][1] = 0;
        }
        fftw_execute(p);
        populateFFTResults(out, fftResults);
    }
    std::pair<float, float> getMinMaxAmplitude(const sf::Int16* samples, std::size_t sampleCount) {
        float minAmplitude = std::numeric_limits<float>::max();
        float maxAmplitude = std::numeric_limits<float>::min();

        // Prepare FFT variables
        fftw_complex *in, *out;
        fftw_plan p;
        in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * BUFFER_SIZE);
        out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * BUFFER_SIZE);
        p = fftw_plan_dft_1d(BUFFER_SIZE, in, out, FFTW_FORWARD, FFTW_MEASURE);

        // Container to store FFT results
        std::vector<std::complex<double>> fftResults(BUFFER_SIZE / 2);

        // Windowed samples
        double windowedSamples[BUFFER_SIZE];

        // Loop through audio samples
        for (std::size_t start = 0; start < sampleCount; start += BUFFER_SIZE) {
            // Apply Hamming window
            applyHammingWindow(windowedSamples, samples + start, std::min(BUFFER_SIZE, static_cast<int>(sampleCount - start)));

            // Perform FFT
            performFFT(windowedSamples, in, out, p, fftResults);

            // Update min and max amplitudes
            for (const auto& fftResult : fftResults) {
                float amplitude = std::abs(fftResult);
                minAmplitude = std::min(minAmplitude, amplitude);
                maxAmplitude = std::max(maxAmplitude, amplitude);
            }
        }

        fftw_destroy_plan(p);
        fftw_free(in);
        fftw_free(out);
        std::cout << "minAmplitude: " << minAmplitude << std::endl;
        std::cout << "maxAmplitude: " << maxAmplitude << std::endl;
        return {minAmplitude, maxAmplitude};
    }

    void analyzeAudio(sf::RenderWindow& window, const sf::Int16* samples, std::size_t sampleCount, sf::Sound& sound, int difficulty, float minAmplitude, float maxAmplitude)
    {
        const double sampleRate = 44100.0;

        // Prepare FFT variables
        fftw_complex *in, *out;
        fftw_plan p;
        in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * BUFFER_SIZE);
        out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * BUFFER_SIZE);
        p = fftw_plan_dft_1d(BUFFER_SIZE, in, out, FFTW_FORWARD, FFTW_MEASURE);
        for (int i = 0; i <4; i++)
        {
            halo.push_back(VertexArray(TriangleFan));
        }
        // Prepare channels
        std::vector<sf::Int16> leftChannel, rightChannel;
        for (std::size_t i = 0; i < sampleCount; i += 2) {
            leftChannel.push_back(samples[i]);
            rightChannel.push_back(samples[i + 1]);
        }

        // Container to store FFT results
        std::vector<std::complex<double>> fftResults(BUFFER_SIZE / 2);

        double windowedSamples[BUFFER_SIZE];
        std::vector<sf::CircleShape> frequencyCircles(frequencyRanges.size());

// Initialize the circles
        for (size_t i = 0; i < frequencyCircles.size(); ++i) {
            frequencyCircles[i].setRadius(5);
            frequencyCircles[i].setFillColor(sf::Color::Green);
            frequencyCircles[i].setPosition(100 + i * 120, 100);
        }
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
            }

            if (sound.getStatus() != sf::Sound::Playing) {
                continue;
            }

            for (std::size_t channel = 0; channel < 2; ++channel) {
                const sf::Int16* channelSamples = (channel == 0) ? leftChannel.data() : rightChannel.data();

                for (std::size_t start = 0; start < leftChannel.size(); start += BUFFER_SIZE) {
                    sf::Time offset = sound.getPlayingOffset();
                    std::size_t startSample = static_cast<std::size_t>(offset.asSeconds() * sampleRate);

                    applyHammingWindow(windowedSamples, samples + startSample,
                                       std::min(BUFFER_SIZE, static_cast<int>(sampleCount - startSample)));
                    performFFT(windowedSamples, in, out, p, fftResults);

                    //clear the halo
                    for (auto &&layer: halo) {
                        layer.clear();
                    }
                    window.clear();
                    float avgAmplitude = update_by_sound(60, 250, 1, fftResults, minAmplitude, maxAmplitude);

    // Define your limits
                    float min_radius = 50.0f;  // Set your own value
                    float max_radius = 100.0f; // Set your own value
                    float MIN_AMPLITUDE = 0.001f; // Minimum amplitude that you expect (set your own value)
                    float MAX_AMPLITUDE = 1000000.0f; // Maximum amplitude that you expect (set your own value)

    // Calculate adaptive scale factor
                    float scaled_radius = min_radius + (max_radius - min_radius) *
                                                       ((avgAmplitude - minAmplitude) / (maxAmplitude - minAmplitude));

    // Clamp the radius within the min and max limits
                    scaled_radius = std::max(min_radius, std::min(scaled_radius, max_radius));

                    std::vector<std::pair<int, int>> binRanges = convertFrequencyRangesToBinRanges(frequencyRanges, 44100.0,
                                                                                                   BUFFER_SIZE);
                    for (int i = 0; i < binRanges.size(); ++i) {
                        int from = binRanges[i].first;
                        int to = binRanges[i].second;
                        frequency_spectrum_round(halo, halo_colors, halo_heights, center, 50, from, to,
                                                 fftResults);
                    }
                    float bassAmplitude = update_by_sound(20, 100, 1, fftResults, minAmplitude, maxAmplitude);  // 20-100Hz for heavy bass

    // Scale the bass amplitude to a reasonable radius size
                    for (size_t i = 0; i < binRanges.size(); ++i) {
                        const auto& range = binRanges[i];
                        float amplitude = update_by_sound(range.first, range.second, 1, fftResults, minAmplitude, maxAmplitude);
                        // Update the circle's radius based on the amplitude
                        frequencyCircles[i].setRadius(5 +  amplitude);
                        window.draw(frequencyCircles[i]);
                    }
                    for (int i = halo.size() - 1; i > 0; i--) {
                        window.draw(halo[i]);
                    }
                    window.display();
                }
            }
        }

        fftw_destroy_plan(p);
        fftw_free(in);
        fftw_free(out);
    }

    int main() {
        sf::SoundBuffer buffer;
        if (!buffer.loadFromFile("../asset/Song/music.wav")) {
            return -1;
        }

        const sf::Int16* samples = buffer.getSamples();
        std::size_t sampleCount = buffer.getSampleCount();

        sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Audio Visualizer");
        window.setFramerateLimit(60);
        sf::Sound sound;
        //int difficulty = calculateDifficulty(samples, sampleCount);
        auto [minAmplitude, maxAmplitude] = getMinMaxAmplitude(samples, sampleCount);
        sound.setBuffer(buffer);
        sound.play();

        analyzeAudio(window, samples, sampleCount, sound, 1, minAmplitude, maxAmplitude);

        return 0;
    }