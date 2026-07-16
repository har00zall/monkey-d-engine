# Requirement:

1. MSVC
2. CMake (min version 3.24)
3. C++ (min version 20)
4. Vulkan SDK Installed and Path configured

# How to start:

1. clone the project
2. open vs code or any ide
3. build:
   `cmake -S . -B build`
   then
   `cmake --build build`
   or just press "Build" button at the vscode bottom panel
4. copy `assets` folder to `build/sandbox/Debug`
5. compile shader inside `shaders` folder to SPIR-V
   for vertex
   `glslc -fshader-stage=vertex shaders/base.vert -o build/sandbox/Debug/shaders/base.vert.spv`
   for fragment
   `glslc -fshader-stage=fragment shaders/base.frag -o build/sandbox/Debug/shaders/base.frag.spv`
6. run the build at `build/sandbox/Debug/`
