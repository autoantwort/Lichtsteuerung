#!/bin/bash

if [[ "$OSTYPE" == "msys" ]]; then
  make="mingw32-make.exe"
else
  make="make"
fi

export make="$make -j$(getconf _NPROCESSORS_ONLN)"
