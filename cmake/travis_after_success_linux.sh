#!/bin/bash

cd build
cd src

echo "set basedir \".\"" > OpenRaider.ini
echo "set pakdir \".\"" >> OpenRaider.ini
echo "set audiodir \".\"" >> OpenRaider.ini
echo "set datadir \"../../data\"" >> OpenRaider.ini
echo "set font \"../../data/test.ttf\"" >> OpenRaider.ini

xvfb-run -n 99 -s "-screen 0 640x480x24" ./OpenRaider &
sleep 10 # Wait for OpenRaider to start
import -window root -display :99.0 screenshot.png

wget http://imgur.com/tools/imgurbash.sh
chmod a+x ./imgurbash.sh
./imgurbash.sh screenshot.png > url

export SHIC_SCRIPT=":j #OpenRaider; New Screenshot: $(url); :q"

wget https://raw.githubusercontent.com/halhen/shic/master/shic
chmod a+x ./shic
./shic -h chat.freenode.net -p 6667 -n OR-Travis-Worker

