# Fractales

## Description

This repos contains a personnal project blablabla

Pictures (when project is ready)

## Project, compilation and execution

There are currentely three projects integrated in the "Fractale" solution:
- FractalesJulia: A starting C# project that generates and saves an image of a fractal.
- FractalesUI: A real time graphical interface for exploring the generated fractals, and (in the future) beeing able to change the Julia set. It is implemented in C++, and using SFML.
- CUDAShader: A project implementing code parallelization (executing on NVIDIA GPUs) for the former projects. It is written in CUDA and compiles into a .dll

BuildTools contains shell scripts that are executed after successful compilation.

These projects require the following settings to compile and run correctely:
- FractalesJulia: works in any case. But it will be faster if CUDAShader works and compiles properly.
- FractalesUI: needs SFML. You can download it [here](https://www.sfml-dev.org/download.php). It goes in the Libraries folder.
- CUDAShader: Needs an **NVIDIA GPU** and cuda installed. The Cuda files must be located at `C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.8\include` (or you'll have to change the compilation configuration of the project). If you don't have a RTX 2080 Ti or better (supporting sm_75), change this setting to `compute_52,sm_52` in the CUDAShader project properties `CUDAShader -> Cuda C/C++ -> Device -> Code Generation`

I did not try executing this on another OS than Windows. It might work with a few tweaks here and there.
