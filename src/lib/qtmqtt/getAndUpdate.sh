#!/bin/bash

set -e

if [[ "$OSTYPE" == "msys" ]]; then # windows
  # on windows we have to install perl first :(
  if [[ $(cmd "/c perl -v") == *"is not recognized"* ]]; then
    echo "########################################"
    echo "################## ERROR ###############"
    echo "########################################"
    echo "Perl is not installed on your system (in cmd). You have to install Perl!"
    echo "Visit http://strawberryperl.com and download and install Perl."
    exit 3
  fi
fi

# first we have to find the Qt dir
if [ ! -z "${QT_DIR}" ]; then # from env variable
  QT="${QT_DIR}"
  if [ ! -d "$QT" ]; then
    echo "The dir saved in env variable QT_DIR does not exists: '$QT'"
    echo "Execute 'export QT_DIR=your/path/to/qt' bevor execute this script to set the QT dir."
    exit 2
  fi
elif [ -d "/c/Qt" ]; then # windows
  QT="/c/Qt"
elif [ -d ~/Qt ]; then # mainly mac and linux
  QT=~/Qt
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
echo "Use QT Version $LATEST"

GIT_DIR="mqtt"
CUR_DIR=$(pwd)
# add or update git
if [ ! -d "$GIT_DIR" ]; then # if folder "GIT_DIR" does not exists
  git clone https://github.com/qt/qtmqtt.git "$GIT_DIR"
  cd "$GIT_DIR"
else
  cd "$GIT_DIR"
  if [[ $(git pull) = "Already up to date." ]]; then
    # we can skip copying 
    echo "Already up to date."
    exit 0
  fi
fi
# we are in the "$GIT_DIR" now.
# get the latest 5.x branch, but we now use the latest installed qt version
# last=$(git branch -a | grep -oh '5\.[0-9]\+.[0-9]\+' | tail -1)

git checkout "$LATEST" 

cd ..
# to build there must be no space in th path.
# copy to different dir to change files
TEMP_DIR=$(mktemp) # creates empty file
rm $TEMP_DIR
mkdir $TEMP_DIR
cp -r $GIT_DIR $TEMP_DIR/$GIT_DIR
cd $TEMP_DIR/$GIT_DIR

# we have to change the includes.
for file in ./src/mqtt/*
do
  [ -d "$file" ] && continue
  echo "Process $file";
  sed "s/<QtMqtt\/\([a-z]\+\)\.h>/\"\1.h\"/" "$file" > "temp";
  cp "temp" "$file";
done

# build the project
if [ ! -z "$GITLAB_CI" ]; then # in gitlab CI
  /usr/src/mxe/usr/x86_64-w64-mingw32.shared.posix/qt5/bin/qmake
else
  BUILD=$(ls "$QT/$LATEST/" | grep _64)
  "$QT/$LATEST/$BUILD/bin/qmake"
fi
make -j$(getconf _NPROCESSORS_ONLN)

# go back and copy the relevant files
cd "$CUR_DIR"
[ -d include ] && rm -rf include
cp -r $TEMP_DIR/$GIT_DIR/include include
cp $GIT_DIR/src/mqtt/*.h include/QtMqtt
[ -d lib ] && rm -rf lib
cp -r $TEMP_DIR/$GIT_DIR/lib lib

rm -rf $TEMP_DIR
echo "Installation complete"