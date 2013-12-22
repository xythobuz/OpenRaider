#!/bin/sh

echo Creating ZIP file..
cd bin
zip -r OpenRaider.zip OpenRaider.app ../COPYING ../README.md ../README.old
echo Archive ready!
