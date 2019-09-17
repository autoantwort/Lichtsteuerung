#!/bin/bash

GIT_DIR="libbacktrace.git"

# wenn es schon da ist, einfach löschen
rm -f -r $GIT_DIR
git clone https://github.com/ianlancetaylor/libbacktrace.git $GIT_DIR

cd $GIT_DIR

# build from http://boostorg.github.io/stacktrace/stacktrace/configuration_and_build.html#stacktrace.configuration_and_build.mingw_and_mingw_w64_specific_not
if [[ -z "$GITLAB_CI" ]]; then
    echo "We are not in the gitlab CI"
    ./configure 
else
    # we are in the CI and want to cross compile
    echo "We are in the gitlab CI"
    ./configure --host=mingw32
fi
make

cd ..
# move important files to new folder
mkdir -p lib
echo "$GIT_DIR:"
ls -al $GIT_DIR
echo "$GIT_DIR/.libs:"
ls -al $GIT_DIR/.libs
mv $GIT_DIR/.libs/libbacktrace.a lib
mkdir -p include
mv $GIT_DIR/backtrace.h include
echo "Installation complete"