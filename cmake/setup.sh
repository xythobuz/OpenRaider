#!/bin/sh

echo "Setting up OpenRaider for user $USER..."

mkdir -p ~/.OpenRaider
mkdir -p ~/.OpenRaider/paks
mkdir -p ~/.OpenRaider/music
mkdir -p ~/.OpenRaider/data
mkdir -p ~/.OpenRaider/sshots

cp data/OpenRaider.ini ~/.OpenRaider/
cp data/*.tga ~/.OpenRaider/data

echo "DONE"
