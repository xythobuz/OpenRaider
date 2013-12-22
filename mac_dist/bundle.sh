#!/bin/sh

echo Creating bundle directory structure
mkdir -p bin/OpenRaider.app/Contents/MacOS
mkdir -p bin/OpenRaider.app/Contents/Resources/defaults
mkdir -p bin/OpenRaider.app/Contents/Frameworks

echo Moving bundle data in place
cp mac_dist/Info.plist bin/OpenRaider.app/Contents/Info.plist
cp mac_dist/openraider.icns bin/OpenRaider.app/Contents/Resources/openraider.icns
cp mac_dist/OpenRaider bin/OpenRaider.app/Contents/MacOS/OpenRaider
cp bin/release/OpenRaider bin/OpenRaider.app/Contents/MacOS/OpenRaider-bin
cp data/* bin/OpenRaider.app/Contents/Resources/defaults/
