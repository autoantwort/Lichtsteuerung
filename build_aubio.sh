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
./waf configure --disable-wavwrite --disable-wavread --msvc_version='mingw' --prefix ""
./waf build 
./waf install --destdir "$PWD/../src/lib/aubio"