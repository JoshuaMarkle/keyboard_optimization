<h1 align="center">🦎 GECKO 🦎</h1>
<h3 align="center">Genitic Engine for Custom Keyboard Optimization</h3>

**GECKO** is a application designed to visualize and optimize custom keyboards. The goal is to be able to take any possible physical key configuration and though some statistical model, generate an optimal keyboard layout for for that keyboard.

![Screenshot 1](https://github.com/JoshuaMarkle/keyboard_optimization/blob/main/docs/screenshot1.png?raw=true)

### # Quick Setup 🛠️

```
git clone --depth 1 --recurse-submodules --shallow-submodules https://github.com/JoshuaMarkle/keyboard_optimization.git
```
Run the Makefile with `make`

### # Project Goals 🚀

- Build the statistical model
  - Implement a combination of a genetic algorithm/simulated annealing/tabu search to optimize given keyboards
  - Include a variety of optimization parameters for efficient optimization
  - [Maybe] Select between different types of optimization techniques
- Keyboard parameters
  - Each key's parameters
    - Location: (x,y) [Maybe] z-cord & rotational-cord for 3D
    - Finger Assignment: What finger is pressing the key?
    - Hand Assignment: What hand is supposed to hit the key?
  - Each finger's parameters
    - Difficulty: How difficult is it to use a finger? (0 = not possible; every other number becomes relative)
    - Horizontal Difficulty: How difficult is it to move side to side? (0 = not possible)
    - Resting Key: What key ID to rest at?
  - Compiled down pre-optimization
- Language Profile
  - A separate script to take in a text corpus and make a language profile to then be used in optimization
- Custom keyboard creator: Actually build custom keyboards within graphical software and assign parameters
- Import/Export/Templates
  - Physical Keyboards
  - Language Profiles
- [Maybe] Self Contained Libraries: I want to be able to clone the project and have it just work without installing OpenGL on the side
