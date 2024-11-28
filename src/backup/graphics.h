#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "genetic.h"
#include "imgui.h"
#include <vector>
#include <string>

// Function prototypes
void renderKey(float x, float y, float size, char character);
void renderKeyboard(const PhysicalKeyboard& keyboard, const std::vector<char>& layout);

#endif
