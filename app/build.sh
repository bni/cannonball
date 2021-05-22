#!/bin/sh

# clean
rm ./OutRun.app/Contents/MacOS/cannonball
rm ./OutRun.app/Contents/libs/libSDL2-2.0.0.dylib

# Then copy executable ...
cp ../cmake/cmake-build-debug/cannonball OutRun.app/Contents/MacOS/

# ... and bundle SDL2 dylib
../../macdylibbundler/dylibbundler -of -b -x ./OutRun.app/Contents/MacOS/cannonball -d ./OutRun.app/Contents/libs/
otool -L OutRun.app/Contents/MacOS/cannonball
