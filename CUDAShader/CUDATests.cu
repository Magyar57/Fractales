#include "CUDATests.cuh"

#include <iostream>
#include <stdio.h>
#include <assert.h>

#include <chrono> // A SUPPRIMER

#define USING_GPU true
#define N 10

__declspec(dllexport) void tests() {
    using std::cout;
    using std::endl;

    int nb_exec = 50;

    for (int i = 0; i < nb_exec; i++) {
        std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();
        mainTestCUDA();
        std::chrono::steady_clock::time_point stop = std::chrono::high_resolution_clock::now();
        std::chrono::microseconds duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        char* device_name = USING_GPU ? "GPU" : "CPU";
        cout << "Temps de calcul (" << device_name << "): " << duration.count() / 1000. << "ms" << endl;
    }
}

void mainTestCUDA() {
    using std::cout;
    using std::endl;

    // Allocations et initialisations
    int* a, * b, * c;
    cudaMallocManaged(&a, N * sizeof(int));
    cudaMallocManaged(&b, N * sizeof(int));
    cudaMallocManaged(&c, N * sizeof(int));
    assert(a && b && c);

    for (int i = 0; i < N; i++) {
        a[i] = i;
        b[i] = N * i; // N fois plus
        c[i] = 0;
    }

    // Calcul (soit GPU, soit CPU)
    if (USING_GPU) vectorAddCUDA<<<1, N >>>(a, b, c);
    else vectorAdd(a, b, c);
    cudaDeviceSynchronize();

    // Libération de mémoire
    cudaFree(a);
    cudaFree(b);
    cudaFree(c);
}

void vectorAdd(int* a, int* b, int* c) {
    int i;
    for (i = 0; i < N; i++) {
        c[i] = a[i] + b[i];
    }
}

__global__ void vectorAddCUDA(int* a, int* b, int* c) {
    int i = threadIdx.x;
    if (i < N) c[i] = a[i] + b[i];
}