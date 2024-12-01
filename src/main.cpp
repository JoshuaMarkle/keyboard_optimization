#include "gui.h"
#include "settings.h"
#include "threading.h"

// Global variables
int currentGeneration;
std::vector<char> bestLayout;
double bestFitness;
std::vector<float> fitnessHistory;
PhysicalKeyboard keyboard;

// Optimization Variables
int populationSize = 1000;
int generations = 1000;
float mutationRate = 0.1f;
float weightDistance = 1.0f, weightEffort = 1.0f;

int main() {
	setupGui();
	while (!guiLoop()) {}
	cleanupGui();
	return 0;
}
