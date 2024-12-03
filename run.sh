#!/bin/bash

if [ ! -d "dependencies/Crow/build" ]; then
    git clone https://github.com/CrowCpp/Crow.git dependencies/Crow
    mkdir -p dependencies/Crow/build
    cd dependencies/Crow/build
    cmake .. -DCROW_BUILD_EXAMPLES=OFF -DCROW_BUILD_TESTS=OFF
    make install
    cd ../../../
fi

rm -rf build
mkdir build
cd build
cmake ..
make
./GoQuant

