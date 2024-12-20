#include "optimize.h"
#include "threading.h"
#include "settings.h"
#include <random>
#include <string>
#include <algorithm>
#include <deque>
#include <iostream>

PhysicalKeyboard::PhysicalKeyboard() {
	keys = {
		{0, 0, 0, 0}, {1, 0, 1, 0}, {2, 0, 2, 0}, {3, 0, 3, 0}, {4, 0, 4, 0},
		{5, 0, 5, 1}, {6, 0, 6, 1}, {7, 0, 7, 1}, {8, 0, 8, 1}, {9, 0, 9, 1},
		{0, 1, 0, 0}, {1, 1, 1, 0}, {2, 1, 2, 0}, {3, 1, 3, 0}, {4, 1, 4, 0},
		{5, 1, 5, 1}, {6, 1, 6, 1}, {7, 1, 7, 1}, {8, 1, 8, 1}, {9, 1, 9, 1},
		{0, 2, 0, 0}, {1, 2, 1, 0}, {2, 2, 2, 0}, {3, 2, 3, 0}, {4, 2, 4, 0},
		{5, 2, 5, 1}, {6, 2, 6, 1}, {7, 2, 7, 1}, {8, 2, 8, 1}, {9, 2, 9, 1}
	};
}

// Function prototypes
int runAlgorithm();
double layoutValue(const std::vector<char>& layout);
std::vector<char> randomLayout(const std::string& characters);
std::vector<std::vector<char>> initializePopulation(int size, const std::string& characters);
std::vector<std::vector<char>> selectPopulation(const std::vector<std::vector<char>>& population, const std::vector<double>& fitness, int numSelected);
std::vector<char> crossover(const std::vector<char>& parent1, const std::vector<char>& parent2);
void mutate(std::vector<char>& layout, float mutationRate);
std::vector<char> findBestLayout(const std::vector<std::vector<char>>& population);
// Main function
int runAlgorithm() {
    std::string characters = "abcdefghijklmnopqrstuvwxyz";
    auto population = initializePopulation(populationSize, characters);

    // Run genetic algorithm
    for (int gen = 0; gen < generations; ++gen) {
		currentGeneration = gen;
        std::vector<double> fitness;
        for (const auto& layout : population) {
            fitness.push_back(layoutValue(layout));
        }

        auto selected = selectPopulation(population, fitness, populationSize / 2);
        std::vector<std::vector<char>> newPopulation = selected;

        while (newPopulation.size() < static_cast<size_t>(populationSize)) {
            const auto& parent1 = selected[std::rand() % selected.size()];
            const auto& parent2 = selected[std::rand() % selected.size()];
            auto child = crossover(parent1, parent2);
            mutate(child, mutationRate);
            newPopulation.push_back(child);
        }

        population = newPopulation;

		// Update the best layout
		std::vector<char> logBestLayout = findBestLayout(population);
		updateBestLayout(logBestLayout);
		bestFitness = layoutValue(logBestLayout);
		fitnessHistory.push_back((float) bestFitness);
		// std::cout << "Best Fitness = " << bestFitness << " | " << std::string(logBestLayout.begin(), logBestLayout.end()) << std::endl;
    }

    return 0;
}

// Function definitions
double layoutValue(const std::vector<char>& layout) {
    double value = 0.0;
    for (size_t i = 0; i < layout.size(); ++i) {
        char c = layout[i];
		double freq = ('a' <= c && c <= 'z') ? frequency[c - 'a'] : 0.0;

        if (keyboard.keys[i].finger == 0 || keyboard.keys[i].finger == 9)
            value -= freq * 2;
        if (keyboard.keys[i].y == 1)
            value += freq * 1.5;
    }
    return value;
}

std::vector<char> randomLayout(const std::string& characters) {
    std::vector<char> layout(characters.begin(), characters.end());
    std::shuffle(layout.begin(), layout.end(), std::mt19937{std::random_device{}()});
    return layout;
}

std::vector<std::vector<char>> initializePopulation(int size, const std::string& characters) {
    std::vector<std::vector<char>> population;
    for (int i = 0; i < size; ++i) {
        population.push_back(randomLayout(characters));
    }
    return population;
}

std::vector<std::vector<char>> selectPopulation(const std::vector<std::vector<char>>& population, const std::vector<double>& fitness, int numSelected) {
    std::vector<std::pair<double, std::vector<char>>> scoredPopulation;
    for (size_t i = 0; i < population.size(); ++i) {
        scoredPopulation.emplace_back(fitness[i], population[i]);
    }

    std::sort(scoredPopulation.rbegin(), scoredPopulation.rend());

    std::vector<std::vector<char>> selected;
    for (int i = 0; i < numSelected; ++i) {
        selected.push_back(scoredPopulation[i].second);
    }
    return selected;
}

std::vector<char> crossover(const std::vector<char>& parent1, const std::vector<char>& parent2) {
    int split = parent1.size() / 2;
    std::vector<char> child(parent1.begin(), parent1.begin() + split);
    for (char c : parent2) {
        if (std::find(child.begin(), child.end(), c) == child.end()) {
            child.push_back(c);
        }
    }
    return child;
}

void mutate(std::vector<char>& layout, float mutationRate) {
    std::mt19937 rng{std::random_device{}()};
    std::uniform_real_distribution<> dist(0.0, 1.0);

    for (size_t i = 0; i < layout.size(); ++i) {
        if (dist(rng) < mutationRate) {
            int swapIndex = rng() % layout.size();
            std::swap(layout[i], layout[swapIndex]);
        }
    }
}

std::vector<char> findBestLayout(const std::vector<std::vector<char>>& population) {
    double bestFitnessLocal = -1e9;
	std::vector<char> bestLayoutLocal;

    for (const auto& layout : population) {
        double fitness = layoutValue(layout);
        if (fitness > bestFitnessLocal) {
            bestFitnessLocal = fitness;
			bestLayoutLocal = layout;
        }
    }

    return bestLayoutLocal;
}
