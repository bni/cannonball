#!/bin/sh

# First build using CMake. I use CLion for this.
#cmake -DCMAKE_BUILD_TYPE=Debug -DTARGET=macos.cmake -G "CodeBlocks - Unix Makefiles" ./cmake
#cmake --build ./cmake/cmake-build-debug --target all -- -j 6

# Then copy executable ...
cp ../cmake/cmake-build-debug/cannonball OutRun.app/Contents/MacOS/

# ... and bundle SDL2 dylib
otool -L OutRun.app/Contents/MacOS/cannonball
../../macdylibbundler/dylibbundler -of -b -x ./OutRun.app/Contents/MacOS/cannonball -d ./OutRun.app/Contents/libs/
otool -L OutRun.app/Contents/MacOS/cannonball
