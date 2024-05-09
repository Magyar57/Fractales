# FractalesUI-OpenGL

An implementation of a fractal visualisation, running on GPU using GLSL.

# Controls

All the controlls are available on the settings control pannel UI. To make the control pannel appear, press "S" (for "Settings") when the GL window is active.

# Compilation

- Install the needed dependencies: cmake, OpenGL, GLFW, GLEW, GLM
- Download the external libraries files by running the `SRC/ExternalLibs/download.sh` script
- Create a build folder `mkdir build && cd build`
- Create the makefile using cmake `cmake ..`
- Then compile the application `make`

It will compile and link a `fractales` executable.
