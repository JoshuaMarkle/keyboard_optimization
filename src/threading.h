#ifndef THREADING_H
#define THREADING_H

#include <atomic>
#include <mutex>

extern std::atomic<bool> simulationRunning;
extern std::mutex layoutMutex;

void optimizationThread();
void startOptimization();
void stopOptimization();
void updateBestLayout(const std::vector<char>& layout);
std::string getBestLayout();

#endif
