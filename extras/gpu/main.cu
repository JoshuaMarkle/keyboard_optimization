#include <cuda_runtime.h>
#include <curand_kernel.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <limits>

// --- DATA --- // 

// Character frequencies (using a fixed array for GPU compatibility)
__constant__ double frequencyArray[26] = {
    7.03, 1.41, 1.83, 3.23, 11.97, 2.00, 1.94, 5.08, 5.92, 0.11, 
    0.90, 4.07, 2.29, 6.54, 8.38, 1.47, 0.04, 5.21, 5.73, 8.62, 
    3.29, 1.27, 2.19, 0.10, 2.63, 0.05
};

// GPU-friendly key structure
struct Key {
    int x;
    int y;
    int finger;
    int hand;
};

// CUDA Error Checking Macro
#define CUDA_CHECK(call) \
    if ((call) != cudaSuccess) { \
        std::cerr << "CUDA Error: " << cudaGetErrorString(cudaGetLastError()) << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
        exit(1); \
    }

// --- GPU Kernels --- //

// Kernel to initialize curand states for randomness
__global__ void initCurandStates(curandState *state, int seed, int populationSize) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < populationSize) {
        curand_init(seed + idx, 0, 0, &state[idx]);
    }
}

// Kernel for fitness evaluation
__global__ void evaluateFitness(
    const char *layouts, 
    const int *keys, 
    double *fitness, 
    int populationSize, 
    int layoutSize
) {
    __shared__ Key sharedKeys[40];
    if (threadIdx.x < 40) {
        sharedKeys[threadIdx.x] = reinterpret_cast<const Key*>(keys)[threadIdx.x];
    }
    __syncthreads();

    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < populationSize) {
        double value = 0.0;
        for (int i = 0; i < layoutSize; ++i) {
            char c = layouts[idx * layoutSize + i];
            double freq = frequencyArray[c - 'a'];

            const Key &key = sharedKeys[i];
            if (key.finger == 0 || key.finger == 9) value -= freq * 2;
            if (key.y == 1) value += freq * 1.5;
        }
        fitness[idx] = value;
    }
}

// Kernel for mutation
__global__ void mutateKernel(char *population, curandState *state, int layoutSize, float mutationRate, int populationSize) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < populationSize) {
        curandState localState = state[idx];
        for (int i = 0; i < layoutSize; ++i) {
            if (curand_uniform(&localState) < mutationRate) {
                int swapIdx = curand(&localState) % layoutSize;
                char temp = population[idx * layoutSize + i];
                population[idx * layoutSize + i] = population[idx * layoutSize + swapIdx];
                population[idx * layoutSize + swapIdx] = temp;
            }
        }
        state[idx] = localState;
    }
}

// Kernel for crossover
__global__ void crossoverKernel(
    const char *parents, 
    char *offspring, 
    curandState *state, 
    int layoutSize, 
    int populationSize
) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < populationSize / 2) {
        curandState localState = state[idx];
        int parent1Idx = curand(&localState) % populationSize;
        int parent2Idx = curand(&localState) % populationSize;

        for (int i = 0; i < layoutSize / 2; ++i) {
            offspring[idx * layoutSize + i] = parents[parent1Idx * layoutSize + i];
        }
        for (int i = layoutSize / 2; i < layoutSize; ++i) {
            offspring[idx * layoutSize + i] = parents[parent2Idx * layoutSize + i];
        }
        state[idx] = localState;
    }
}

// --- GPU Genetic Algorithm --- //

void runGeneticAlgorithmGPU(int populationSize, int generations, float mutationRate, const std::string &characters) {
    int layoutSize = characters.size();
    int numKeys = 40; // Assuming 10 keys per row, 4 rows

    // Initialize keyboard
    std::vector<Key> keys(numKeys);
    for (int i = 0; i < numKeys; ++i) {
        keys[i] = {i % 10, i / 10, i % 10, i / 5};
    }

    // Initialize population
    std::vector<char> flatPopulation(populationSize * layoutSize);
    std::generate(flatPopulation.begin(), flatPopulation.end(), [&characters]() {
        return characters[std::rand() % characters.size()];
    });

    // Allocate GPU memory
    char *d_population;
    char *d_offspring;
    double *d_fitness;
    int *d_keys;
    curandState *d_states;

    CUDA_CHECK(cudaMalloc(&d_population, populationSize * layoutSize * sizeof(char)));
    CUDA_CHECK(cudaMalloc(&d_offspring, (populationSize / 2) * layoutSize * sizeof(char)));
    CUDA_CHECK(cudaMalloc(&d_fitness, populationSize * sizeof(double)));
    CUDA_CHECK(cudaMalloc(&d_keys, numKeys * sizeof(Key)));
    CUDA_CHECK(cudaMalloc(&d_states, populationSize * sizeof(curandState)));

    // Copy data to GPU
    CUDA_CHECK(cudaMemcpy(d_population, flatPopulation.data(), populationSize * layoutSize * sizeof(char), cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemcpy(d_keys, keys.data(), numKeys * sizeof(Key), cudaMemcpyHostToDevice));

    // Initialize random states
    initCurandStates<<<(populationSize + 255) / 256, 256>>>(d_states, time(nullptr), populationSize);

    // Configure kernel dimensions
    int threadsPerBlock = 256;
    int blocksPerGrid = (populationSize + threadsPerBlock - 1) / threadsPerBlock;

    // Run genetic algorithm
    for (int gen = 0; gen < generations; ++gen) {
        evaluateFitness<<<blocksPerGrid, threadsPerBlock>>>(d_population, d_keys, d_fitness, populationSize, layoutSize);

        // Perform selection, crossover, and mutation
        crossoverKernel<<<blocksPerGrid, threadsPerBlock>>>(d_population, d_offspring, d_states, layoutSize, populationSize);
        mutateKernel<<<blocksPerGrid, threadsPerBlock>>>(d_offspring, d_states, layoutSize, mutationRate, populationSize / 2);

        // Replace population with offspring
        CUDA_CHECK(cudaMemcpy(d_population, d_offspring, (populationSize / 2) * layoutSize * sizeof(char), cudaMemcpyDeviceToDevice));
    }

    // Retrieve results
    std::vector<char> finalPopulation(populationSize * layoutSize);
    CUDA_CHECK(cudaMemcpy(finalPopulation.data(), d_population, populationSize * layoutSize * sizeof(char), cudaMemcpyDeviceToHost));

    cudaFree(d_population);
    cudaFree(d_offspring);
    cudaFree(d_fitness);
    cudaFree(d_keys);
    cudaFree(d_states);
}

// --- Main Function --- //

int main() {
    const std::string characters = "abcdefghijklmnopqrstuvwxyz";
    const int populationSize = 1000;
    const int generations = 1000;
    const float mutationRate = 0.05;

    runGeneticAlgorithmGPU(populationSize, generations, mutationRate, characters);

    return 0;
}
