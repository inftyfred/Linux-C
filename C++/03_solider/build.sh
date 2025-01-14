#!/bin/bash
#run cmake & make -> exec file
mkdir -p ./build

cd ./build

cmake ..

make -j4
