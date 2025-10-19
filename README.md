# Suggestive Contours

![GIF of the program running, displaying a 3D model of a bunny rendered as line art using the technique described in the paper mentioned below.](/assets/demo.gif)

## About

This is an implementation of the paper "Suggestive Contours for Conveying Shape" published in "ACM Transactions on Graphics (TOG), Volume 22, Issue 3"

## Building and Running

1. Clone this repository and `cd` into the root directory.

2. Configure CMake by running `cmake -S . -B build`.

3. `cd` into the build directory and run `make` to build the project.

4. The executable will be in `build/src`. You will need to run it from that location or it wont be able to find the shader files.