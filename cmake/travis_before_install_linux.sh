#!/bin/bash

sudo add-apt-repository -y ppa:zoogie/sdl2-snapshots
sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
sudo add-apt-repository -y ppa:smspillaz/cmake-2.8.12
sudo add-apt-repository -y 'deb http://llvm.org/apt/precise/ llvm-toolchain-precise-3.5 main'
sudo apt-get -qq -y update

