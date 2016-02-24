#!/bin/sh
rm -rf build
mkdir build
cd build
ln -s ../data data
cmake ..
make

