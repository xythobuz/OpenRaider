#!/bin/bash

sudo apt-get -qq -y install libsdl2-dev libsdl2-ttf-dev libalut-dev libpng12-dev cmake

if [ "$CC" = "gcc" ]; then sudo apt-get install gcc-4.8; fi
if [ "$CXX" = "g++" ]; then sudo apt-get install g++-4.8; fi

if [ "$CXX" = "clang++" ]; then sudo apt-get install --allow-unauthenticated clang-3.5; fi

wget 'http://downloads.sourceforge.net/project/ogl-math/glm-0.9.6.3/glm-0.9.6.3.zip'
unzip glm-0.9.6.3.zip
sudo cp -R glm/glm /usr/local/include/glm

