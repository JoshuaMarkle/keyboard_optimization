#ifndef GUI_H
#define GUI_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "genetic.h"
#include <unordered_map>
#include <random>
#include <algorithm>
#include <thread>
#include <chrono>

class GUI {
public:
    void initialize();
    void render();
    void cleanup();

private:
    int populationSize;
    int generations;
    float mutationRate;

    std::shared_ptr<PhysicalKeyboard> keyboard;
    std::string characters;
};

#endif
