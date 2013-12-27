#!/bin/sh

echo Creating ZIP file..
cd bin
zip -r OpenRaider.zip OpenRaider.app ../BUGS ../ChangeLog ../COPYING ../README.md ../README.old ../Requirements ../TODO
echo Archive ready!
