#include "optimize.h"
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <algorithm>
#include <deque>

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
double layoutValue(const std::vector<char>& layout, const PhysicalKeyboard& keyboard);
std::vector<char> randomLayout(const std::string& characters);
std::vector<std::vector<char>> initializePopulation(int size, const std::string& characters);
std::vector<std::vector<char>> selectPopulation(const std::vector<std::vector<char>>& population, const std::vector<double>& fitness, int numSelected);
std::vector<char> crossover(const std::vector<char>& parent1, const std::vector<char>& parent2);
void mutate(std::vector<char>& layout, float mutationRate);
std::vector<char> findBestLayout(const std::vector<std::vector<char>>& population, const PhysicalKeyboard& keyboard);

// Main function
int runAlgorithm() {
    // Parameters for genetic algorithm
    std::string characters = "abcdefghijklmnopqrstuvwxyz";
    int populationSize = 100;
    int generations = 1000;
    float mutationRate = 0.05;

    // Initialize keyboard and population
    PhysicalKeyboard keyboard;
    auto population = initializePopulation(populationSize, characters);

    // Run genetic algorithm
    for (int gen = 0; gen < generations; ++gen) {
        // Calculate fitness
        std::vector<double> fitness;
        for (const auto& layout : population) {
            fitness.push_back(layoutValue(layout, keyboard));
        }

        // Select and reproduce
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

        // Log best layout
        // auto bestLayout = findBestLayout(population, keyboard);
        // double bestFitness = layoutValue(bestLayout, keyboard);
        // std::cout << "Generation " << gen + 1 << ": Best Fitness = " << bestFitness << " | " << std::string(bestLayout.begin(), bestLayout.end()) << std::endl;
    }

	// Log last best layout
	auto bestLayout = findBestLayout(population, keyboard);
	double bestFitness = layoutValue(bestLayout, keyboard);
	std::cout << "Best Fitness = " << bestFitness << " | " << std::string(bestLayout.begin(), bestLayout.end()) << std::endl;

    return 0;
}

// Function definitions
double layoutValue(const std::vector<char>& layout, const PhysicalKeyboard& keyboard) {
    double value = 0.0;
    for (size_t i = 0; i < layout.size(); ++i) {
        char c = layout[i];
        double freq = (0 <= c && c <= 26) ? frequency[c - 'a'] : 0.0;

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

std::vector<char> findBestLayout(const std::vector<std::vector<char>>& population, const PhysicalKeyboard& keyboard) {
    double bestFitness = -1e9; // A very low initial value
    std::vector<char> bestLayout;

    for (const auto& layout : population) {
        double fitness = layoutValue(layout, keyboard);
        if (fitness > bestFitness) {
            bestFitness = fitness;
            bestLayout = layout;
        }
    }

    return bestLayout;
}
