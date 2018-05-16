#!/bin/bash

FILE="DataBook.dat"
if [ -f $FILE ]; then
    rm -f $FILE
fi

# for Mac exec:
./testProtoBuf.app/Contents/MacOS/testProtoBuf ../../../carta/scriptedClient/tests/data/aH.fits

# for Linux exec:
# ./testProtoBuf ../../../carta/scriptedClient/tests/data/aH.fits

