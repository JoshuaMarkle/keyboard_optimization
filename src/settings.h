#ifndef SETTINGS_H
#define SETTINGS_H

#include <vector>
#include <atomic>

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
extern std::vector<char> bestLayout;

// Optimization Parameters
extern int populationSize;
extern int generations;
extern float mutationRate;
extern float weightDistance, weightEffort;

#endif
