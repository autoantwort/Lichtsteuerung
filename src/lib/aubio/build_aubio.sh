#!/bin/bash
source ../scripts/set_env.sh
# anleitung von hier : https://aubio.org/manual/latest/installing.html#git-repository

GIT_DIR="aubio.git"

# add or update git
../scripts/clone_or_pull.sh $GIT_DIR https://github.com/aubio/aubio.git && exit 0
cd $GIT_DIR
# we are in the "$GIT_DIR" now

# get waf to build
./scripts/get_waf.sh
# build
if [[ "$OSTYPE" == "msys" ]]; then
    # we are on windows
    ./waf configure --disable-wavwrite --disable-wavread -v --prefix= ""
else
    if ! [[ -z "$GITLAB_CI" ]]; then
        # we are on the ci
        TARGET="--with-target-platform=win64"
    fi
    ./waf configure --disable-wavwrite --disable-wavread $TARGET --prefix= ""
fi
./waf install --disable-tests --disable-examples --destdir "$PWD/../"