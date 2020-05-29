#!/bin/bash

if ! [ -x "$(command -v cmake)" ]; then
  echo "#############################################"
  echo "################### ERROR ###################"
  echo "## cmake must be installed and on the PATH ##"
  echo "######## https://cmake.org/download/ ########"
  echo "#############################################"
  echo "#############################################"
  echo "PS: You have to restart the console after installing to 'reload' the PATH variable"
  exit 1
fi

source ../scripts/set_env.sh

mkdir -p lib
mkdir -p include

GIT_DIR=segvcatch.git
# add or update git
../scripts/clone_or_pull.sh $GIT_DIR https://github.com/Plaristote/segvcatch.git && exit 0
cd $GIT_DIR
# we are in the "$GIT_DIR" now

mkdir -p release
cd release

# build
if [[ "$OSTYPE" == "msys" ]] || ! [[ -z "$GITLAB_CI" ]]; then
    # we are on windows
    # from build_mingw_release.bat
    cmake -G "MSYS Makefiles" -DCMAKE_BUILD_TYPE=Release ../lib
    $make
    cd ..
else
    cmake ..
    $make 
    cd ..
    EXTRA_DIR="lib"
    # we are on linux and cross compile
fi
cd ..
cp segvcatch.git/release/$EXTRA_DIR/libsegvcatch.a lib
cp segvcatch.git/lib/segvcatch.h include
echo "Installation complete"
