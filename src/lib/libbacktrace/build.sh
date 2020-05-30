#!/bin/bash

source ../scripts/set_env.sh

GIT_DIR="libbacktrace.git"

# add or update git
../scripts/clone_or_pull.sh $GIT_DIR https://github.com/ianlancetaylor/libbacktrace.git && exit 0
cd $GIT_DIR
# we are in the "$GIT_DIR" now

# build from http://boostorg.github.io/stacktrace/stacktrace/configuration_and_build.html#stacktrace.configuration_and_build.mingw_and_mingw_w64_specific_not
if [[ -z "$GITLAB_CI" ]]; then
    echo "We are not in the gitlab CI"
    ./configure 
else
    # we are in the CI and want to cross compile
    echo "We are in the gitlab CI"
    ./configure --host=mingw32
fi
$make

cd ..
# move important files to new folder
mkdir -p lib
cp $GIT_DIR/.libs/libbacktrace.a lib
mkdir -p include
cp $GIT_DIR/backtrace.h include
echo "Installation complete"
