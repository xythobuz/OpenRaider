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

echo 'USER OR-Travis-Worker OR-Travis-Worker OR-Travis-Worker OR-Travis-Worker' > msg
echo 'NICK OR-Travis-Worker' >> msg
echo 'JOIN #OpenRaider' >> msg
echo "PRIVMSG #OpenRaider Travis $CXX Worker has a screenshot:" >> msg
echo -n "PRIVMSG #OpenRaider " >> msg
cat url >> msg
echo 'QUIT' >> msg

cat msg | nc chat.freenode.net 6667

