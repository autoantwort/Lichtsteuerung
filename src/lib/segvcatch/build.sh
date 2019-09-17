#!/bin/bash

mkdir -p lib
mkdir -p include
# wenn es schon da ist, einfach löschen
rm -f -r segvcatch.git
git clone https://github.com/Plaristote/segvcatch.git segvcatch.git

cd segvcatch.git

mkdir release
cd release

# build
if [ "$(expr substr $(uname -s) 1 5)" == "MINGW" ]; then
    # we are on windows
    # from build_mingw_release.bat
    cmake -G "MSYS Makefiles" -DCMAKE_BUILD_TYPE=Release ../lib
    mingw32-make
    cd ..
else
    cmake ..
    make 
    cd ..
    # we are on linux and cross compile
fi
cd ..
mv segvcatch.git/release/libsegvcatch.a lib
mv segvcatch.git/lib/segvcatch.h include
echo "Installation complete"