#ifndef __CUDATESTS_CUH__
#define __CUDATESTS_CUH__

#include "cuda_runtime.h"
#include "device_launch_parameters.h"

extern "C" {

    /*
    * Effectue des tests de performances CPU vs GPU
    * @returns void
    */
    __declspec(dllexport) void tests();
}

void mainTestCUDA();
void vectorAdd(int* a, int* b, int* c);
__global__ void vectorAddCUDA(int* a, int* b, int* c); // kernel


#endif