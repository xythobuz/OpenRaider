#!/bin/bash

cd build
cd src

echo "set basedir \".\"" > OpenRaider.ini
echo "set pakdir \".\"" >> OpenRaider.ini
echo "set audiodir \".\"" >> OpenRaider.ini
echo "set datadir \".\"" >> OpenRaider.ini
echo "set font \"../../data/test.ttf\"" >> OpenRaider.ini

xvfb-run -a -s "-screen 0 640x480x24" ./OpenRaider &
sleep 5 # Wait for OpenRaider to start

import -window root screenshot.png

# Find a better way for this!
sudo apt-get install uuencode mail
uuencode screenshot.png | mail -s "Screenshot" xythobuz@xythobuz.de

