#include "CUDAShader.cuh"

#include <iostream>
#include <stdio.h>
#include <assert.h>

/**
* Notes:
* Dans les propriétés du projet CUDAShader -> Cuda C/C++ -> Device -> Code Generation
* La valeur par défaut est "compute_52,sm_52"
* Je l'ai changée pour "compute_75,sm_75"
* Car ma RTX 2080Ti est d'architecture Turing, et supporte donc jusqu'au sm_75
* Les constantes ci-dessous sont définies en fonction de cette architecture
*/

// Grid ; max grid dimension = 3
#define THEORATICAL_MAX_X_GRID_DIMENSION 2147483647 // 2^31-1 ; works fine
#define THEORATICAL_MAX_YZ_GRID_DIMENSION 65535     // 2^16 ; works fine
// Block ; max block dimension = 3
#define THEORETICAL_MAX_XY_BLOCK_DIMENSION 1024     // 2^10 ; real limit is lower ?
#define THORETICAL_MAX_Z_BLOCK_DIMENSION 64
// Threads
#define THEORETICAL_MAX_THREADS_PER_BLOCK 1024

#define MAX_X_BLOCK_DIMENSION 896 // Limite trouvée par tests, pour tout le reste à 1

__declspec(dllexport) int** allocate_tab(int n, int m) {
    int** res;
    res = (int**)malloc(n * sizeof(int*));

    if (res == NULL) {
        perror("allocate_tab");
        return NULL;
    }

    for (int i = 0; i < n; i++) {
        res[i] = (int*)malloc(m * sizeof(int));

        if (res[i] == NULL) {
            perror("allocate_tab");
            return NULL;
        }
    }
    return res;
}

__declspec(dllexport) void free_tab(int** tab, int n, int m) {
    if (tab == NULL) return;

    for (int i = 0; i < n; i++) {
        if (tab[i] == NULL) continue;
        free(tab[i]);
    }

}

__declspec(dllexport) void calculate_Julia(double c_re, double c_im, int** h_res, int width, int height, double scale, double offset_re, double offset_im) {
    cudaError_t cuda_res;
    int nbPixel = width * height;
    int* device_tab;
    
    // Allocate device memory (no need to init)
    // Using one dimensionnal array (seems to be better for cuda programming)
    cuda_res = cudaMallocManaged(&device_tab, nbPixel * sizeof(int));
    if (cuda_res != cudaSuccess) { perror("cudaMallocManaged"); exit(EXIT_FAILURE); }

    // Determining dimension to execute
    int block_dim = MAX_X_BLOCK_DIMENSION; // we're using only on dimensionnal blocks (since we cannot go ver 896, for no found reason...)
    int grid_dim = nbPixel / MAX_X_BLOCK_DIMENSION;
    dim3 grid_size(grid_dim); // aka number of blocks (per grid)
    dim3 block_size(block_dim); // aka number of thread (per blocks)

    // Executing kernel
    calculate_Julia_GPU<<<grid_size, block_size>>>(c_re, c_im, device_tab, nbPixel, width, height, scale, offset_re, offset_im);
    cuda_res = cudaDeviceSynchronize();
    if (cuda_res != cudaSuccess) { 
        std::cout << "\n" << cuda_res << std::endl;
        perror("cudaDeviceSynchronize"); exit(EXIT_FAILURE); 
    }

    // Copy results from device to host
    for (int i = 0; i < nbPixel; i++) h_res[i / height][i % height] = device_tab[i];

    // Free allocated memory
    cudaFree(device_tab);
}

__declspec(dllexport) void new_calculate_Julia(double c_re, double c_im, int** h_res, int width, int height, double x_min, double y_min, double x_max, double y_max) {
    cudaError_t cuda_res;
    int nbPixel = width * height;
    int* device_tab;

    // Allocate device memory (no need to init)
    // Using one dimensionnal array (seems to be better for cuda programming)
    cuda_res = cudaMallocManaged(&device_tab, nbPixel * sizeof(int));
    if (cuda_res != cudaSuccess) { perror("cudaMallocManaged"); exit(EXIT_FAILURE); }

    // Determining dimension to execute
    int block_dim = MAX_X_BLOCK_DIMENSION; // we're using only on dimensionnal blocks (since we cannot go ver 896, for no found reason...)
    int grid_dim = nbPixel / MAX_X_BLOCK_DIMENSION;
    dim3 grid_size(grid_dim); // aka number of blocks (per grid)
    dim3 block_size(block_dim); // aka number of thread (per blocks)

    // Executing kernel
    new_calculate_Julia_GPU<<<grid_size, block_size>>>(c_re, c_im, device_tab, nbPixel, width, height, x_min, y_min, x_max, y_max);
    cuda_res = cudaDeviceSynchronize();
    if (cuda_res != cudaSuccess) {
        std::cout << "\n" << cuda_res << std::endl;
        perror("cudaDeviceSynchronize"); exit(EXIT_FAILURE);
    }

    // Copy results from device to host
    for (int i = 0; i < nbPixel; i++) h_res[i / height][i % height] = device_tab[i];

    // Free allocated memory
    cudaFree(device_tab);
}

__global__ void calculate_Julia_GPU(double c_re, double c_im, int* d_res, const int nbPixel, const int width, const int height, double scale, double offset_re, double offset_im) {
    int n = blockDim.x * blockIdx.x + threadIdx.x; // index of thread: block_idx * 896 + thread_idx     // For fullHD: blockIdx € [0,2315] and threadIdx € [0,896] so n < 2315*896
    int i = n / height;
    int j = n % height;

    if (!(n < nbPixel)) return;

    double z0_re = (i - offset_re) * scale;
    double z0_im = (j - offset_im) * scale;

    // isSuiteDivergente

    double zi_re = z0_re; // init zi = z0
    double zi_im = z0_im;
    double zi_mod = z0_re * z0_re + z0_im * z0_im;

    double zi_re_temp;
    const unsigned int MAX_ITERATION = 200;
    const double MODULE_MAX = 4.0;

    int nb_iteration = 0;
    while (zi_mod < MODULE_MAX && nb_iteration < MAX_ITERATION) {
        zi_re_temp = zi_re;                             // next zi
        zi_re = zi_re * zi_re - zi_im * zi_im + c_re;   // next zi
        zi_im = 2 * zi_re_temp * zi_im + c_im;          // next zi
        zi_mod = zi_re * zi_re + zi_im * zi_im;   // update zi (squared) module
        nb_iteration++;
    }

    if (zi_mod >= MODULE_MAX) { // On a un module très grand
        d_res[i * height + j] = true;
    }
    else {
        d_res[i * height + j] = false;
    }

    // fin isSuiteDivergente

}

__global__ void new_calculate_Julia_GPU(double c_re, double c_im, int* d_res, const int nbPixel, const int width, const int height, double x_min, double y_min, double x_max, double y_max) {
    int n = blockDim.x * blockIdx.x + threadIdx.x; // index of thread: block_idx * 896 + thread_idx     // For fullHD: blockIdx € [0,2315] and threadIdx € [0,896] so n € [0, 2315*896[
    int i = n / height;
    int j = n % height;

    if (!(n < nbPixel)) return;

    double pas_x = (x_max - x_min) / width;
    double pas_y = (y_max - y_min) / height;
    double z0_re = i * pas_x + x_min;
    double z0_im = j * pas_y + y_min;
    z0_re *= -1; // Mirror on 0x

    // isSuiteDivergente

    double zi_re = z0_re; // init zi = z0
    double zi_im = z0_im;
    double zi_mod = z0_re * z0_re + z0_im * z0_im;

    double zi_re_temp;
    const unsigned int MAX_ITERATION = 200;
    const double MODULE_MAX = 4.0;

    int nb_iteration = 0;
    while (zi_mod < MODULE_MAX && nb_iteration < MAX_ITERATION) {
        zi_re_temp = zi_re;                             // next zi
        zi_re = zi_re * zi_re - zi_im * zi_im + c_re;   // next zi
        zi_im = 2 * zi_re_temp * zi_im + c_im;          // next zi
        zi_mod = zi_re * zi_re + zi_im * zi_im;   // update zi (squared) module
        nb_iteration++;
    }

    if (zi_mod >= MODULE_MAX) { // On a un module très grand
        d_res[i * height + j] = true;
    }
    else {
        d_res[i * height + j] = false;
    }

    // fin isSuiteDivergente

    if (z0_re == 0) d_res[i *height + j] = -1; // Ox
    if (z0_im == 0) d_res[i * height + j] = -1; // Ox

}

// CPU Version (never executed)
#pragma region CPU Version

void calculate_Julia_CPU(double c_re, double c_im, int** res, int width, int height, double scale, double offset_re, double offset_im) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            res[i][j] = isSuiteDivergente(c_re, c_im, (i - offset_re) * scale, (j - offset_im) * scale) ? 1 : 0; // z0 = new Complexe((i - offset.re) * scale, (j - offset.im) * scale)
        }
    }

    return;
}

bool isSuiteDivergente(double c_re, double c_im, double z0_re, double z0_im) {
    double zi_re = z0_re; // init zi = z0
    double zi_im = z0_im;
    double zi_mod = module(z0_re, z0_im);
    double zi_re_temp;
    const unsigned int MAX_ITERATION = 200;
    const double MODULE_MAX = 4.0;

    int i = 0;
    while (zi_mod < MODULE_MAX && i < MAX_ITERATION) {
        zi_re_temp = zi_re;                             // next zi
        zi_re = zi_re * zi_re - zi_im * zi_im + c_re;   // next zi
        zi_im = 2 * zi_re_temp * zi_im + c_im;          // next zi
        zi_mod = module(zi_re, zi_im);                  // update zi module
        i++;
    }

    if (zi_mod >= MODULE_MAX) { // On a un module très grand
        return true;
    }

    return false;
}

double module(double z_re, double z_im) {
    return sqrt(z_re * z_re + z_im * z_im);
}

#pragma endregion