#!/bin/bash

# Create build directory if not exists
mkdir -p build
# Go into build, generate Ninja files, then build
cd build
cmake -G Ninja ..
ninja

# Go back and run
cd ..
./build/hello