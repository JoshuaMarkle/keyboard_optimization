#ifndef OPTIMIZE_H
#define OPTIMIZE_H

#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <algorithm>
#include <deque>

const double frequency[26] = {
    7.039965378908611,  1.4181595867450678,  1.8378716271912954,
    3.2343930318184317, 11.97656207067099,   2.0034208935538826,
    1.9467494641973953, 5.086003187338573,   5.926801121063912,
    0.111625542671869,  0.9039951640380283,  4.077595208001319,
    2.296738473374732,  6.54915645436061,    8.38256305984503,
    1.4703659943946805, 0.04671099631807441, 5.21823652250371,
    5.7306836291696435, 8.621613452766939,   3.296216409298236,
    1.276309281749739,  2.1985079958234874,  0.10784744738143649,
    2.635736659888993,  0.05667142935648733
};

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

extern PhysicalKeyboard keyboard;

int runAlgorithm();
double layoutValue(const std::vector<char>& layout, const PhysicalKeyboard& keyboard);
std::vector<char> randomLayout(const std::string& characters);
std::vector<std::vector<char>> initializePopulation(int size, const std::string& characters);
std::vector<std::vector<char>> selectPopulation(const std::vector<std::vector<char>>& population, const std::vector<double>& fitness, int numSelected);
std::vector<char> crossover(const std::vector<char>& parent1, const std::vector<char>& parent2);
void mutate(std::vector<char>& layout, float mutationRate);
std::vector<char> findBestLayout(const std::vector<std::vector<char>>& population, const PhysicalKeyboard& keyboard);

#endif
