#!/bin/bash

if [ ! -z "${QT_LATEST}" ];then
    return 0 # already set
fi

# first we have to find the Qt dir for qmake
if [ ! -z "${QT_DIR}" ]; then # from env variable
  QT="${QT_DIR}"
  if [ ! -d "$QT" ]; then
    echo "The dir saved in env variable QT_DIR does not exists: '$QT'"
    echo "Execute 'export QT_DIR=your/path/to/qt' bevor execute this script to set the QT dir."
    exit 2
  fi
elif [ -d "/c/Qt" ]; then # windows
  QT="C:\\Qt"
elif [ -d ~/Qt ]; then # mainly mac and linux
  QT="~/Qt"
elif [ -d "/opt/Qt" ]; then # some stackoverflow answers recommends that
  QT="/opt/Qt"
elif [ ! -z "$GITLAB_CI" ]; then
  echo "Use Gitlab CI"
else
  if [ ! -z "${QT_DIR}" ]; then
    QT="${QT_DIR}"
    if [ ! -d "$QT" ]; then
      echo "The dir saved in env variable QT_DIR does not exists: '$QT'"
      exit 2
    fi
  else
    echo "No Qt installation found."
    echo "Execute 'export QT_DIR=your/path/to/qt' bevor execute this script to set the QT dir."
    exit 1 
  fi
fi
export QT_DIR="$QT"

if [ ! -z "$GITLAB_CI" ]; then # in gitlab CI
  LATEST=$(/usr/src/mxe/usr/x86_64-w64-mingw32.shared.posix/qt5/bin/qmake --version | grep -oh '5\.[0-9]\+.[0-9]\+')
else
  LATEST=$(ls "$QT" | grep 5. | tail -1)
  if [ -z $LATEST ]; then
    echo "No installed Qt Version like 5.xx.x found in '$QT'"
    echo "Execute 'export QT_DIR=your/path/to/qt' bevor execute this script to set the QT dir."    
    exit 1
  fi
fi
export QT_LATEST="$LATEST"

BUILD=$(ls "$QT/$LATEST/" | grep _64)  

# on windows we have to set the path to use the right compiler
if [[ "$OSTYPE" == "msys" ]];then
    compilerDir=$(echo $BUILD | sed s/_/0_/ ) # build is e.g. mingw73_64, but the compiler dir is than mingw730_64
    export PATH="$QT\\$LATEST\\$BUILD\\bin;$QT\\Tools\\$compilerDir\\bin;$PATH"
    QT_N=$(echo $QT | sed 's/\\/\//' ) # here we need normal slashed, but in path backslashes -.-
    export CC="$QT_N/Tools/$compilerDir/bin/gcc.exe"
    export CXX="$QT_N/Tools/$compilerDir/bin/g++.exe"
fi


if [ ! -z "$GITLAB_CI" ]; then # in gitlab CI
  qmake=/usr/src/mxe/usr/x86_64-w64-mingw32.shared.posix/qt5/bin/qmake
else
  qmake="$QT/$LATEST/$BUILD/bin/qmake"
fi
export qmake=$qmake

if [[ "$OSTYPE" == "msys" ]] || ! [[ -z "$GITLAB_CI" ]]; then
  make="mingw32-make.exe"
else
  make="make"
fi

export make="$make -j$(getconf _NPROCESSORS_ONLN)"
