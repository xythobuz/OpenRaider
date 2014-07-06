#!/bin/bash

cd build
cd src

export DISPLAY=:99.0
sh -e /etc/init.d/xvfb start

echo "set basedir \".\"" > OpenRaider.ini
echo "set pakdir \".\"" >> OpenRaider.ini
echo "set audiodir \".\"" >> OpenRaider.ini
echo "set datadir \".\"" >> OpenRaider.ini
echo "set font \"../../data/test.ttf\"" >> OpenRaider.ini

./OpenRaider &
sleep 5 # Wait for OpenRaider to start

import -window root screenshot.png

# Find a better way for this
uuencode screenshot.png | mail -s "Screenshot" xythobuz@xythobuz.de

