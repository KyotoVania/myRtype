#include "taskLoader.hpp"


void executeLoadingTasks(GUI& gui, const std::vector<std::function<void(int&)>>& tasks) {
    // Create a progress variable to hold the loading status
    int progress(0);

    // Create a detached thread to run the loading tasks
    std::thread loadingThread([&]() {
        int totalTasks = tasks.size();
        int currentTask = 0;
        for (auto& task : tasks) {
            progress = (currentTask * 100) / totalTasks;
            task(progress);  // Execute the task
            currentTask++;
        }
        progress = 100;  // Loading complete
    });

    loadingThread.detach();

    // Update the loading screen until all tasks are complete
    while (progress < 100) {
        gui.updateLoadingScreen(progress);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

