#!/bin/bash
# Build the game in ./build and launch it (Ubuntu/Debian).
set -e

sudo apt-get -y install libsfml-dev cmake g++

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j"$(nproc)"

./build/ParchisGame
