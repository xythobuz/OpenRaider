#!/bin/sh
echo Moving dynamic libraries into place
cp /usr/local/lib/libalut.0.dylib bin/OpenRaider.app/Contents/Frameworks/libalut.0.dylib
cp /opt/local/lib/libSDL_ttf-2.0.0.dylib bin/OpenRaider.app/Contents/Frameworks/libSDL_ttf-2.0.0.dylib
cp /opt/local/lib/libz.1.dylib bin/OpenRaider.app/Contents/Frameworks/libz.1.dylib
cp /opt/local/lib/libSDL-1.2.0.dylib bin/OpenRaider.app/Contents/Frameworks/libSDL-1.2.0.dylib
cp /opt/local/lib/libSDL2-2.0.0.dylib bin/OpenRaider.app/Contents/Frameworks/libSDL2-2.0.0.dylib
cp /opt/local/lib/libbz2.1.0.dylib bin/OpenRaider.app/Contents/Frameworks/libbz2.1.0.dylib
cp /opt/local/lib/libfreetype.6.dylib bin/OpenRaider.app/Contents/Frameworks/libfreetype.6.dylib
cp /opt/local/lib/libpng16.16.dylib bin/OpenRaider.app/Contents/Frameworks/libpng16.16.dylib

echo Fixing Paths for OpenRaider-bin
install_name_tool -change /usr/local/lib/libalut.0.dylib @executable_path/../Frameworks/libalut.0.dylib bin/OpenRaider.app/Contents/MacOS/OpenRaider-bin
install_name_tool -change /opt/local/lib/libSDL_ttf-2.0.0.dylib @executable_path/../Frameworks/libSDL_ttf-2.0.0.dylib bin/OpenRaider.app/Contents/MacOS/OpenRaider-bin
install_name_tool -change /opt/local/lib/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib bin/OpenRaider.app/Contents/MacOS/OpenRaider-bin
install_name_tool -change /opt/local/lib/libSDL2-2.0.0.dylib @executable_path/../Frameworks/libSDL2-2.0.0.dylib bin/OpenRaider.app/Contents/MacOS/OpenRaider-bin
install_name_tool -change /opt/X11/lib/libXmu.6.dylib /usr/X11/lib/libXmu.6.dylib bin/OpenRaider.app/Contents/MacOS/OpenRaider-bin
install_name_tool -change /opt/X11/lib/libXt.6.dylib /usr/X11/lib/libXt.6.dylib bin/OpenRaider.app/Contents/MacOS/OpenRaider-bin
install_name_tool -change /opt/X11/lib/libSM.6.dylib /usr/X11/lib/libSM.6.dylib bin/OpenRaider.app/Contents/MacOS/OpenRaider-bin
install_name_tool -change /opt/X11/lib/libICE.6.dylib /usr/X11/lib/libICE.6.dylib bin/OpenRaider.app/Contents/MacOS/OpenRaider-bin
install_name_tool -change /opt/X11/lib/libXext.6.dylib /usr/X11/lib/libXext.6.dylib bin/OpenRaider.app/Contents/MacOS/OpenRaider-bin
install_name_tool -change /opt/X11/lib/libX11.6.dylib /usr/X11/lib/libX11.6.dylib bin/OpenRaider.app/Contents/MacOS/OpenRaider-bin
install_name_tool -change /opt/X11/lib/libXi.6.dylib /usr/X11/lib/libXi.6.dylib bin/OpenRaider.app/Contents/MacOS/OpenRaider-bin

# SDL_ttf seems to require libSDL-1.2.0. Weird MacPorts thing?
echo Fixing Paths for libSDL-1.2.0.dylib
install_name_tool -change /opt/local/lib/libX11.6.dylib /usr/X11/lib/libX11.6.dylib bin/OpenRaider.app/Contents/Frameworks/libSDL-1.2.0.dylib
install_name_tool -change /opt/local/lib/libXext.6.dylib /usr/X11/lib/libXext.6.dylib bin/OpenRaider.app/Contents/Frameworks/libSDL-1.2.0.dylib
install_name_tool -change /opt/local/lib/libXrandr.2.dylib /usr/X11/lib/libXrandr.2.dylib bin/OpenRaider.app/Contents/Frameworks/libSDL-1.2.0.dylib
install_name_tool -change /opt/local/lib/libXrender.1.dylib /usr/X11/lib/libXrender.1.dylib bin/OpenRaider.app/Contents/Frameworks/libSDL-1.2.0.dylib

echo Fixing Paths for libSDL_ttf-2.0.0.dylib
install_name_tool -change /opt/local/lib/libSDL-1.2.0.dylib @executable_path/../Frameworks/libSDL-1.2.0.dylib bin/OpenRaider.app/Contents/Frameworks/libSDL_ttf-2.0.0.dylib
install_name_tool -change /opt/local/lib/libfreetype.6.dylib @executable_path/../Frameworks/libfreetype.6.dylib bin/OpenRaider.app/Contents/Frameworks/libSDL_ttf-2.0.0.dylib
install_name_tool -change /opt/local/lib/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib bin/OpenRaider.app/Contents/Frameworks/libSDL_ttf-2.0.0.dylib
install_name_tool -change /opt/local/lib/libbz2.1.0.dylib @executable_path/../Frameworks/libbz2.1.0.dylib bin/OpenRaider.app/Contents/Frameworks/libSDL_ttf-2.0.0.dylib

echo Fixing Paths for libfreetype.6.dylib
install_name_tool -change /opt/local/lib/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib bin/OpenRaider.app/Contents/Frameworks/libfreetype.6.dylib
install_name_tool -change /opt/local/lib/libbz2.1.0.dylib @executable_path/../Frameworks/libbz2.1.0.dylib bin/OpenRaider.app/Contents/Frameworks/libfreetype.6.dylib
install_name_tool -change /opt/local/lib/libpng16.16.dylib @executable_path/../Frameworks/libpng16.16.dylib bin/OpenRaider.app/Contents/Frameworks/libfreetype.6.dylib

echo Fixing Paths for libpng16.16.dylib
install_name_tool -change /opt/local/lib/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib bin/OpenRaider.app/Contents/Frameworks/libpng16.16.dylib

echo Dynamic libraries are set up!
