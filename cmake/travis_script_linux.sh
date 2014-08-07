#!/bin/bash

if [ "$CC" = "gcc" ]; then export CC="gcc-4.8"; fi
if [ "$CXX" = "g++" ]; then export CXX="g++-4.8"; fi

mkdir build && cd build && cmake .. && make -j 4 && make check

