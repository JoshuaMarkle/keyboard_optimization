#ifndef GENETIC_H
#define GENETIC_H

#include <unordered_map>
#include <random>
#include <algorithm>
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <memory>
#include <atomic>
#include <mutex>
#include <deque>

struct Key {
    int x;
    int y;
    int finger;
    int hand;
};

class PhysicalKeyboard {
public:
    std::vector<Key> keys;
    PhysicalKeyboard();
    int keyCount() const { return keys.size(); }
};

extern std::atomic<bool> running;
extern std::mutex dataMutex;
extern std::vector<float> generationValues;
extern std::deque<std::string> logs;
extern std::vector<char> bestLayout;

void geneticAlgorithm(std::shared_ptr<PhysicalKeyboard> keyboard, const std::string& characters, int populationSize, int generations, float mutationRate);

#endif
