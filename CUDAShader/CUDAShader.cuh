#ifndef __CUDASHADER_CUH__
#define __CUDASHADER_CUH__

#include "cuda_runtime.h"
#include "device_launch_parameters.h"

extern "C" {
    
    /*
    * Alloue un tableau à deux dimensions [n * m]
    * @returns int** : tableau alloué
    */
    __declspec(dllexport) int** allocate_tab(int n, int m);

    /*
    * Libère un tableau à deux dimensions [n * m]
    * @returns void
    */
    __declspec(dllexport) void free_tab(int** tab, int n, int m);
    
    /*
    * Suite: z²+c de premier terme complexe c ; Re(c) = c_re ; Im(c) = c_im
    *
    * On calcule :
    * - Pour chaque pixel de res (i,j) € (width x height)
    * - On associe un complexe z0 = f(scale, i), f(scale, j)
    * - res[i][j] = est-ce que la suite z²+c de premier terme z0 diverge ? 1 : 0
    *
    * => A voir remplacer le booléen par une couleur
    */
    __declspec(dllexport) void calculate_Julia(double c_re, double c_im, int** res, int width, int height, double scale, double offset_re, double offset_im);
    __declspec(dllexport) void new_calculate_Julia(double c_re, double c_im, int** res, int width, int height, double x_min, double y_min, double x_max, double y_max);
}

__global__ void calculate_Julia_GPU(double c_re, double c_im, int* h_res, const int nbPixel, const int width, const int height, double scale, double offset_re, double offset_im);

__global__ void new_calculate_Julia_GPU(double c_re, double c_im, int* h_res, const int nbPixel, const int width, const int height, double x_min, double y_min, double x_max, double y_max);

void calculate_Julia_CPU(double c_re, double c_im, int** res, int width, int height, double scale, double offset_re, double offset_im);
bool isSuiteDivergente(double c_re, double c_im, double z0_re, double z0_im);
double module(double z_re, double z_im);


#endif