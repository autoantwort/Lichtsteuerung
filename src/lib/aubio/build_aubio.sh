#!/bin/bash

# anleitung von hier : https://aubio.org/manual/latest/installing.html#git-repository


GIT_DIR="aubio.git"

# add or update git
if [ ! -d "$GIT_DIR" ]; then # if folder "GIT_DIR" does not exists
  git clone https://github.com/aubio/aubio.git "$GIT_DIR"
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

# get waf to build
./scripts/get_waf.sh
# build
if [ "$(expr substr $(uname -s) 1 5)" == "MINGW" ]; then
    # we are on windows
    ./waf configure --disable-wavwrite --disable-wavread --msvc_version='mingw' --prefix= ""
else
    if ! [[ -z "$GITLAB_CI" ]]; then
        # we are on the ci
        TARGET="--with-target-platform=win64"
    fi
    ./waf configure --disable-wavwrite --disable-wavread $TARGET --prefix= ""
fi
./waf install --disable-tests --disable-examples --destdir "$PWD/../"