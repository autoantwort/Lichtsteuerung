#!/bin/bash

GIT_DIR="libbacktrace.git"

# wenn es schon da ist, einfach löschen
rm -f -r $GIT_DIR
git clone https://github.com/ianlancetaylor/libbacktrace.git $GIT_DIR

cd $GIT_DIR

# build from http://boostorg.github.io/stacktrace/stacktrace/configuration_and_build.html#stacktrace.configuration_and_build.mingw_and_mingw_w64_specific_not
./configure
make

cd ..
# move important files to new folder
mkdir -p lib
mv $GIT_DIR/.libs/libbacktrace.a lib
mkdir -p include
mv $GIT_DIR/backtrace.h include
echo "Installation complete"