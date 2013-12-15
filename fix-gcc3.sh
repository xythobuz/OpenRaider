#!/bin/sh

if [ -f src/endian.h ]
then
    echo GCC 3.x fix...
    mv src/endian.h src/mendian.h

    for i in `grep -l "endian.h" src/*.* src/*/*.*`; do mv $i $i.old && sed -e "s/endian.h/mendian.h/" $i.old > $i; done
else
    echo Removing GCC 3.x fix... 
    mv src/mendian.h src/endian.h

    for i in `grep -l "mendian.h" src/*.* src/*/*.*`; do mv $i $i.old && sed -e "s/mendian.h/endian.h/" $i.old > $i; done

#    for i in `find . -name "*.old"`; do echo $i > sed.0 && mv $i `sed -e "s/.old//" sed.0`; done
#
#    rm -d sed.0
fi
