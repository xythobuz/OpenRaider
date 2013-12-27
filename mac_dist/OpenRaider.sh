#!/bin/sh
cd `dirname $0`
if [ ! -d "~/.OpenRaider" ]; then
    echo "Setting up OpenRaider for user $USER..."
    mkdir -p ~/.OpenRaider
    cp ../Resources/defaults/OpenRaider.init ~/.OpenRaider/
    cp ../Resources/defaults/custom.cfg ~/.OpenRaider/
    mkdir -p ~/.OpenRaider/paks/custom
    mkdir -p ~/.OpenRaider/music
    mkdir -p ~/.OpenRaider/data
    mkdir -p ~/.OpenRaider/sshots
    cp ../Resources/defaults/*.tga ~/.OpenRaider/data
    cp ../Resources/defaults/*.wav ~/.OpenRaider/data
    cp ../Resources/defaults/*.ttf ~/.OpenRaider/data
    cp ../Resources/defaults/*.tr2 ~/.OpenRaider/paks/custom
    echo "DONE"
    osascript -e 'tell app "System Events" to display alert "Initial Configuration stored in ~/.OpenRaider\n\nView and edit OpenRaider.init to your needs..."'
fi
./OpenRaider-bin
