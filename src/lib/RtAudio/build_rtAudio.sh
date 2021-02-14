#!/bin/bash
set -e
# see https://github.com/thestk/rtaudio/blob/master/install.txt

source ../scripts/set_env.sh

GIT_DIR=rtaudio
# add or update git
../scripts/clone_or_pull.sh $GIT_DIR https://github.com/thestk/rtaudio.git && exit 0
cd $GIT_DIR
# we are in the "$GIT_DIR" now

# build
if [[ "$OSTYPE" == "msys" ]] || ! [[ -z "$GITLAB_CI" ]]; then
    # we are on windows or on the gitlab ci
    ./autogen.sh --no-configure
    ./configure --with-wasapi --host=mingw32 
    FILES_TO_COPY="librtaudio.a"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS
    ./autogen.sh --with-core
    FILES_TO_COPY="librtaudio.dylib librtaudio.6.dylib"
else
    #linux 
    if ! dpkg -s libasound2-dev autoconf libtool automake >/dev/null 2>&1; then
        echo "You have to install the packages libasound2-dev autoconf libtool automake! Run 'sudo apt install libasound2-dev autoconf libtool automake'"
        exit 1
    fi
    ./autogen.sh --with-alsa
    FILES_TO_COPY="librtaudio.so*"
fi
$make
cd ..

#copy headers and lib
mkdir -p lib
cd $GIT_DIR/.libs/
cp $FILES_TO_COPY ../../lib
cd ../../
mkdir -p include/rtaudio
cp $GIT_DIR/RtAudio.h include/rtaudio
