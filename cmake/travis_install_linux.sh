#!/bin/bash

sudo apt-get -qq -y install libsdl2-dev libsdl2-ttf-dev libalut-dev libpng12-dev cmake

if [ "$CC" = "gcc" ]; then sudo apt-get install gcc-4.8; fi
if [ "$CXX" = "g++" ]; then sudo apt-get install g++-4.8; fi

if [ "$CXX" = "clang++" ]; then sudo apt-get install --allow-unauthenticated clang-3.5; fi

wget 'http://downloads.sourceforge.net/project/ogl-math/glm-0.9.6.3/glm-0.9.6.3.zip'
unzip glm-0.9.6.3.zip
sudo cp -R glm/glm /usr/local/include/glm

if [ "$CXX" = "clang++" ]; then export CXXFLAGS="-stdlib=libc++"; fi
if [ "$CXX" = "clang++" ]; then svn co --quiet http://llvm.org/svn/llvm-project/libcxx/trunk libcxx; fi
if [ "$CXX" = "clang++" ]; then cd libcxx/lib && bash buildit; fi
if [ "$CXX" = "clang++" ]; then sudo cp ./libc++.so.1.0 /usr/lib/; fi
if [ "$CXX" = "clang++" ]; then sudo mkdir /usr/include/c++/v1; fi
if [ "$CXX" = "clang++" ]; then cd .. && sudo cp -r include/* /usr/include/c++/v1/; fi
if [ "$CXX" = "clang++" ]; then cd /usr/lib && sudo ln -sf libc++.so.1.0 libc++.so; fi
if [ "$CXX" = "clang++" ]; then sudo ln -sf libc++.so.1.0 libc++.so.1 && cd $cwd; fi

