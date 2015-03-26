#!/bin/bash

# Need to build latest glbinding from source
#GLBVER=`curl https://api.github.com/repos/hpicgs/glbinding/releases/latest | grep -m1 tag_name | cut -d\" -f4`
#curl -L https://github.com/hpicgs/glbinding/archive/${GLBVER}.tar.gz | tar xzf -
#cd glbinding-${GLBVER#v}

# Need to use master until a fixed version is released
git clone https://github.com/hpicgs/glbinding.git
cd glbinding

mkdir build
cd build
cmake ..
make -j4
sudo make install
cd ..
cd ..

# Build OpenRaider and run the unit tests
mkdir build && cd build && cmake .. && make -j 4 && make check

