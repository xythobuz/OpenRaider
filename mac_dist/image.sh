#!/bin/sh

echo Creating temporary image
hdiutil create -size 32m -fs HFS+ -volname "OpenRaider" bin/tmp.dmg

echo Mounting image
hdiutil attach bin/tmp.dmg

echo Moving files
cp -r bin/OpenRaider.app /Volumes/OpenRaider/OpenRaider.app
osascript -e 'tell application "Finder" to make alias file to POSIX file "/Applications" at POSIX file "/Volumes/OpenRaider/"'
cp COPYING /Volumes/OpenRaider/
cp README.md /Volumes/OpenRaider/
cp README.old /Volumes/OpenRaider/

echo Unmounting image
hdiutil detach /Volumes/OpenRaider

echo Converting image
hdiutil convert bin/tmp.dmg -format UDZO -o bin/OpenRaider.dmg
rm -rf bin/tmp.dmg

echo Image ready!
