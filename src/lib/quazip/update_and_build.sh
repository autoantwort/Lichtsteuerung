#!/bin/bash
set -e
# set -o xtrace to debug commands
# see https://stachenov.github.io/quazip/

if ! [[ -z "$GITLAB_CI" ]];then
  # in the ci we can skip this step because quazip is already installed via mxe
  exit 0
fi

source ../scripts/set_env.sh

# if we are on windows, we first have to build zlib
if [[ "$OSTYPE" == "msys" ]]; then
  cd zlib_windows
  ./build_windows.sh
  cd ..
fi

GIT_DIR=quazip
# add or update git
../scripts/clone_or_pull.sh $GIT_DIR https://github.com/stachenov/quazip.git && exit 0
cd $GIT_DIR
# we are in the "$GIT_DIR" now

if [[ "$OSTYPE" == "msys" ]]; then
  qmakeOptions=("LIBS += -L'\$\$PWD/../../zlib_windows/bin' -lzlib1" "INCLUDEPATH += '\$\$PWD../../zlib_windows/include'" PREFIX=../../dist quazip.pro)
else
  qmakeOptions=("LIBS += -lz" PREFIX=../../dist quazip.pro)
fi

# see https://unix.stackexchange.com/questions/459367/using-shell-variables-for-command-options for "${qmakeOptions[@]}"

echo "Qmake options" "${qmakeOptions[@]}"

# we only want to build quazip and not the tests
cd quazip
# build the project

$qmake "${qmakeOptions[@]}"

$make install
