#!/bin/bash
set -e
source ../../scripts/set_env.sh
# see https://github.com/madler/zlib/issues/268

GIT_DIR=zlib
# add or update git
../../scripts/clone_or_pull.sh $GIT_DIR https://github.com/madler/zlib.git && exit 0
cd $GIT_DIR
# we are in the "$GIT_DIR" now

# with the default gcc compiler we need that: 
# apply fix from here: https://github.com/madler/zlib/issues/268
#if [[ -f "gzguts.h" ]]; then
#   sed -i 's/defined(_WIN32) || defined(__CYGWIN__)/defined(_WIN32)/g' gzguts.h
#   sed -i 's/gzopen_w//g' win32/zlib.def
#fi
sh configure --prefix=../

export BINARY_PATH=../bin
export INCLUDE_PATH=../include
export LIBRARY_PATH=../lib

$make install -fwin32/Makefile.gcc SHARED_MODE=1