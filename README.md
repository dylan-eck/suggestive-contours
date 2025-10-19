# Suggestive Contours

## About

This is an implementation of the paper "Suggestive Contours for Conveying Shape" published in "ACM Transactions on Graphics (TOG), Volume 22, Issue 3"

## Building and Running

NOTE: Currently, I have only tested this on Fedora via a Distrobox container.

1. Clone this repository

2. Create Distrobox container:

    `distrobox create -i fedora:40 -n fedora-dev && distrobox enter fedora-dev`

3. Install dependencies inside the container:

    `sudo dnf install -y glm-devel glfw-devel mesa-libGL-devel libX11-devel libXrandr-devel libXinerama-devel libXcursor-devel libXi-devel gcc-c++ cmake git`


4. Navigate to the repository root directory and configure CMake.

    `cmake -S . -B build`

5. Navigate to the build directory and build the project by running `make`.

6. You can then run the executable, which will be located in `/build/src`.