#!/bin/bash

GIT_DIR="libbacktrace.git"

# add or update git
if [ ! -d "$GIT_DIR" ]; then # if folder "GIT_DIR" does not exists
  git clone https://github.com/ianlancetaylor/libbacktrace.git "$GIT_DIR"
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
cp $GIT_DIR/.libs/libbacktrace.a lib
mkdir -p include
cp $GIT_DIR/backtrace.h include
echo "Installation complete"
