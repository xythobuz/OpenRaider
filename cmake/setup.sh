#!/bin/sh

echo "Setting up OpenRaider for user $USER..."
mkdir -p ~/.OpenRaider
cp data/OpenRaider.init ~/.OpenRaider/
mkdir -p ~/.OpenRaider/paks
mkdir -p ~/.OpenRaider/music
mkdir -p ~/.OpenRaider/data
mkdir -p ~/.OpenRaider/sshots
cp data/*.tga ~/.OpenRaider/data
cp data/*.ttf ~/.OpenRaider/data
echo "DONE"
