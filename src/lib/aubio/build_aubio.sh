#!/bin/bash

# anleitung von hier : https://aubio.org/manual/latest/installing.html#git-repository

# add and update git submodule
git submodule init
git submodule update
# cd into git repo
cd aubio.git
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