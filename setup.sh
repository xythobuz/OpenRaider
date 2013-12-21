#!/bin/sh

## If you modify this file, do the same changes to mac_dist/OpenRaider!!!

echo "Setting up OpenRaider for user $USER..."
mkdir -p ~/.OpenRaider
cp data/OpenRaider.init ~/.OpenRaider/
cp data/custom.cfg ~/.OpenRaider/
mkdir -p ~/.OpenRaider/paks/custom
mkdir -p ~/.OpenRaider/music
mkdir -p ~/.OpenRaider/data
mkdir -p ~/.OpenRaider/sshots
cp data/*.tga ~/.OpenRaider/data
cp data/*.wav ~/.OpenRaider/data
cp data/*.ttf ~/.OpenRaider/data
cp data/*.tr2 ~/.OpenRaider/paks/custom
echo "DONE"
