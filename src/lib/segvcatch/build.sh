#!/bin/bash

mkdir -p lib
mkdir -p include

GIT_DIR=segvcatch.git
# add or update git
if [ ! -d "$GIT_DIR" ]; then # if folder "GIT_DIR" does not exists
  git clone https://github.com/Plaristote/segvcatch.git "$GIT_DIR"
  cd "$GIT_DIR"
else
  cd "$GIT_DIR"
  if [[ $(git pull) = "Already up to date." ]]; then
    # we can skip recompiling, because the last build is already up to date
    echo "Already up to date."
    exit 0
  fi
fi
# we are in the "$GIT_DIR" now

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
    EXTRA_DIR="lib"
    # we are on linux and cross compile
fi
cd ..
cp segvcatch.git/release/$EXTRA_DIR/libsegvcatch.a lib
cp segvcatch.git/lib/segvcatch.h include
echo "Installation complete"
