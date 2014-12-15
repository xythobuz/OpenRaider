#!/bin/bash

sudo apt-get -qq -y install libsdl2-dev libsdl2-ttf-dev libalut-dev libpng12-dev zlib1g-dev cmake glm

if [ "$CC" = "gcc" ]; then sudo apt-get install gcc-4.8; fi
if [ "$CXX" = "g++" ]; then sudo apt-get install g++-4.8; fi

if [ "$CXX" = "clang++" ]; then sudo apt-get install --allow-unauthenticated clang-3.5; fi

