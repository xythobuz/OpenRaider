#!/bin/sh

echo "Setting up OpenRaider for user $USER..."
mkdir -p ~/.OpenRaider
cp data/OpenRaider.ini ~/.OpenRaider/
mkdir -p ~/.OpenRaider/paks
mkdir -p ~/.OpenRaider/music
mkdir -p ~/.OpenRaider/data
mkdir -p ~/.OpenRaider/sshots
cp data/*.tga ~/.OpenRaider/data
cp data/*.ttf ~/.OpenRaider/data
cp data/*.pc ~/.OpenRaider/data
echo "DONE"
