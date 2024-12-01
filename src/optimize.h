#ifndef OPTIMIZE_H
#define OPTIMIZE_H

#include <string>
#include <vector>

int runAlgorithm();
double layoutValue(const std::vector<char>& layout);
std::vector<char> randomLayout(const std::string& characters);
std::vector<std::vector<char>> initializePopulation(int size, const std::string& characters);
std::vector<std::vector<char>> selectPopulation(const std::vector<std::vector<char>>& population, const std::vector<double>& fitness, int numSelected);
std::vector<char> crossover(const std::vector<char>& parent1, const std::vector<char>& parent2);
void mutate(std::vector<char>& layout, float mutationRate);
std::vector<char> findBestLayout(const std::vector<std::vector<char>>& population);

#endif
