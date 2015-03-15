#!/bin/bash

if [ "$CC" = "gcc" ]; then export CC="gcc-4.8"; fi
if [ "$CXX" = "g++" ]; then export CXX="g++-4.8"; fi

if [ "$CC" = "clang" ]; then export CC="clang-3.5"; fi
if [ "$CXX" = "clang++" ]; then export CXX="clang++-3.5"; fi

# Need to build latest glbinding from source
GLBVER=`curl https://api.github.com/repos/hpicgs/glbinding/releases/latest | grep -m1 tag_name | cut -d\" -f4`
curl -L https://github.com/hpicgs/glbinding/archive/${GLBVER}.tar.gz | tar xzf -
cd glbinding-${GLBVER#v}

# Hacky fix to allow building glbinding with clang on Linux
sed -i 's/  -Werror/  /g' cmake/PlatformLinuxGCC.cmake

mkdir build
cd build
cmake ..
make -j4
sudo make install
cd ..
cd ..

# Build OpenRaider and run the unit tests
mkdir build && cd build && cmake .. && make -j 4 && make check

