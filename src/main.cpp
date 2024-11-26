#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>
#include <numeric>
#include <unordered_map>

// --- LANGUAGE DATA --- //

// Character frequencies
std::unordered_map<char, double> frequency = {
	{'a', 7.039965378908611},
	{'b', 1.4181595867450678},
	{'c', 1.8378716271912954},
	{'d', 3.2343930318184317},
	{'e', 11.97656207067099},
	{'f', 2.0034208935538826},
	{'g', 1.9467494641973953},
	{'h', 5.086003187338573},
	{'i', 5.926801121063912},
	{'j', 0.111625542671869},
	{'k', 0.9039951640380283},
	{'l', 4.077595208001319},
	{'m', 2.296738473374732},
	{'n', 6.54915645436061},
	{'o', 8.38256305984503},
	{'p', 1.4703659943946805},
	{'q', 0.04671099631807441},
	{'r', 5.21823652250371},
	{'s', 5.7306836291696435},
	{'t', 8.621613452766939},
	{'u', 3.296216409298236},
	{'v', 1.276309281749739},
	{'w', 2.1985079958234874},
	{'x', 0.10784744738143649},
	{'y', 2.635736659888993},
	{'z', 0.056671429356487335}
};

// --- PHYSICAL KEYBOARD --- //

struct Key {
    int x;
    int y;
    int finger; // 0-9
    int hand;   // 0 = left hand, 1 = right hand
};

class PhysicalKeyboard {
public:
    std::vector<Key> keys;
    
    // Initialize a standard physical keyboard
    PhysicalKeyboard() {
        keys = {
            // Row 1 (top row)
            {0, 0, 0, 0}, {1, 0, 1, 0}, {2, 0, 2, 0}, {3, 0, 3, 0}, {4, 0, 4, 0}, // Left hand
            {5, 0, 5, 1}, {6, 0, 6, 1}, {7, 0, 7, 1}, {8, 0, 8, 1}, {9, 0, 9, 1}, // Right hand
            
            // Row 2 (home row)
            {0, 1, 0, 0}, {1, 1, 1, 0}, {2, 1, 2, 0}, {3, 1, 3, 0}, {4, 1, 4, 0}, // Left hand
            {5, 1, 5, 1}, {6, 1, 6, 1}, {7, 1, 7, 1}, {8, 1, 8, 1}, {9, 1, 9, 1}, // Right hand
            
            // Row 3 (bottom row)
            {0, 2, 0, 0}, {1, 2, 1, 0}, {2, 2, 2, 0}, {3, 2, 3, 0}, {4, 2, 4, 0}, // Left hand
            {5, 2, 5, 1}, {6, 2, 6, 1}, {7, 2, 7, 1}, {8, 2, 8, 1}, {9, 2, 9, 1}  // Right hand
        };
    }

    int keyCount() const { return keys.size(); }
};

// --- Keyboard Valuator --- //

// Evaluate the "fitness" of a layout
double layoutValue(const std::vector<char>& layout, const PhysicalKeyboard& keyboard) {
    double value = 0.0;

    // Example metric: Finger effort
    for (size_t i = 0; i < layout.size(); ++i) {
        char c = layout[i];
        double freq = frequency.count(c) ? frequency[c] : 0.0;

        // Example penalty for weak fingers (pinkies, fingers 0 and 9)
        if (keyboard.keys[i].finger == 0 || keyboard.keys[i].finger == 9) {
            value -= freq * 2; // Higher penalty
        }

        // Example bonus for home row usage
        if (keyboard.keys[i].y == 1) {
            value += freq * 1.5; // Reward home row
        }
    }

    return value;
}

// layoutDistance -> double
// layoutFingerEffort -> 10 doubles
// layoutHandEffort -> 2 doubles
// ...

// --- GENETIC ALGORITHM --- //

// Create a random layout
std::vector<char> randomLayout(const std::string& characters, const PhysicalKeyboard& keyboard) {
    std::vector<char> layout(characters.begin(), characters.end());
    std::shuffle(layout.begin(), layout.end(), std::mt19937{std::random_device{}()});
    return layout;
}

// Create the initial population
std::vector<std::vector<char>> initializePopulation(int size, const std::string& characters, const PhysicalKeyboard& keyboard) {
    std::vector<std::vector<char>> population;
    for (int i = 0; i < size; ++i) {
        population.push_back(randomLayout(characters, keyboard));
    }
    return population;
}

// Select the top layouts based on fitness
std::vector<std::vector<char>> select(const std::vector<std::vector<char>>& population, const std::vector<double>& fitness, int numSelected) {
    std::vector<std::pair<double, std::vector<char>>> scoredPopulation;
    for (size_t i = 0; i < population.size(); ++i) {
        scoredPopulation.emplace_back(fitness[i], population[i]);
    }

    std::sort(scoredPopulation.rbegin(), scoredPopulation.rend()); // Sort by fitness (descending)

    std::vector<std::vector<char>> selected;
    for (int i = 0; i < numSelected; ++i) {
        selected.push_back(scoredPopulation[i].second);
    }
    return selected;
}

// Perform crossover between two parents
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

// Mutate a layout
void mutate(std::vector<char>& layout, double mutationRate) {
    std::mt19937 rng{std::random_device{}()};
    std::uniform_real_distribution<> dist(0.0, 1.0);

    for (size_t i = 0; i < layout.size(); ++i) {
        if (dist(rng) < mutationRate) {
            int swapIndex = rng() % layout.size();
            std::swap(layout[i], layout[swapIndex]);
        }
    }
}

// Function to find the best layout in a population
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

// Genetic algorithm implementation
std::vector<char> geneticAlgorithm(const PhysicalKeyboard& keyboard, const std::string& characters, int populationSize, int generations, double mutationRate) {
    auto population = initializePopulation(populationSize, characters, keyboard);

    for (int gen = 0; gen < generations; ++gen) {

        // Evaluate fitness
        std::vector<double> fitness;
        for (const auto& layout : population) {
            fitness.push_back(layoutValue(layout, keyboard));
        }

        // Select the top layouts
        auto selected = select(population, fitness, populationSize / 2);

        // Create new population
        std::vector<std::vector<char>> newPopulation = selected;

        while (newPopulation.size() < populationSize) {
            const auto& parent1 = selected[std::rand() % selected.size()];
            const auto& parent2 = selected[std::rand() % selected.size()];
            auto child = crossover(parent1, parent2);
            mutate(child, mutationRate);
            newPopulation.push_back(child);
        }

        population = newPopulation;

		// Find and print the best layout of the current generation
		if (gen % 100 != 0) continue;
        auto bestLayout = findBestLayout(population, keyboard);
        std::cout << "Generation " << gen + 1 << ": Best Layout = ";
        for (char c : bestLayout) {
            std::cout << c;
        }
        std::cout << " | Fitness = " << layoutValue(bestLayout, keyboard) << "\n";
    }

    // Return the best layout
    return findBestLayout(population, keyboard);
}

// --- MAIN --- //

int main() {
    PhysicalKeyboard keyboard;
    std::string characters = "abcdefghijklmnopqrstuvwxyz";
    int populationSize = 100;
    int generations = 1000;
    double mutationRate = 0.1;

    auto bestLayout = geneticAlgorithm(keyboard, characters, populationSize, generations, mutationRate);

    std::cout << "Best Layout: ";
    for (char c : bestLayout) {
        std::cout << c;
    }
    std::cout << "\n";

    return 0;
}
