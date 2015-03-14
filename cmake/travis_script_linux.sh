#!/bin/bash

if [ "$CC" = "gcc" ]; then export CC="gcc-4.8"; fi
if [ "$CXX" = "g++" ]; then export CXX="g++-4.8"; fi

if [ "$CC" = "clang" ]; then export CC="clang-3.5"; fi
if [ "$CXX" = "clang++" ]; then export CXX="clang++-3.5"; fi

git clone https://github.com/hpicgs/glbinding.git
cd glbinding
cmake .
make -j4
sudo make install
cd ..

mkdir build && cd build && cmake .. && make -j 4 && make check

