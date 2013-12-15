#!/bin/sh
rm -f depend
make depend 2> /dev/null
make $@
