#!/bin/bash

set -e

# Clone and build Crow if not already done
if [ ! -d "dependencies/Crow/build" ]; then
    git clone https://github.com/CrowCpp/Crow.git dependencies/Crow
    mkdir -p dependencies/Crow/build
    pushd dependencies/Crow/build
    cmake .. -DCROW_BUILD_EXAMPLES=OFF -DCROW_BUILD_TESTS=OFF
    make install
    popd
fi

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    mkdir build
fi

# Build and run the project
pushd build
cmake ..
make
./GoQuant
popd