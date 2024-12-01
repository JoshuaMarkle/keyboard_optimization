#include <thread>
#include <atomic>
#include <mutex>
#include "optimize.h"
#include "settings.h"

std::atomic<bool> simulationRunning(false);
std::mutex layoutMutex;

void optimizationThread() {
    runAlgorithm();
    simulationRunning = false;
}

void startOptimization() {
    simulationRunning = true;
    std::thread optThread(optimizationThread);
    optThread.detach();
}

void stopOptimization() {
    simulationRunning = false;
}

// Safely update bestLayout
void updateBestLayout(const std::vector<char>& layout) {
    std::lock_guard<std::mutex> lock(layoutMutex);
    bestLayout = layout;
}

// Safely get bestLayout
std::string getBestLayout() {
    std::lock_guard<std::mutex> lock(layoutMutex);
    return std::string(bestLayout.begin(), bestLayout.end());
}
