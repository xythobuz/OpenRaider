#!/bin/sh

echo Creating ZIP file..
cd bin
zip -r OpenRaider.zip OpenRaider.app ../ChangeLog ../COPYING ../README.md ../README.old
echo Archive ready!
