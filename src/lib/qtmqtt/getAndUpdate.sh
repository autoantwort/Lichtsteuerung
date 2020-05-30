#!/bin/bash

set -e
source ../scripts/set_env.sh

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

GIT_DIR="mqtt"
CUR_DIR=$(pwd)
# add or update git
../scripts/clone_or_pull.sh $GIT_DIR https://github.com/qt/qtmqtt.git && exit 0
cd $GIT_DIR
# we are in the "$GIT_DIR" now.

git checkout "$QT_LATEST" 

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
$qmake
$make

# go back and copy the relevant files
cd "$CUR_DIR"
[ -d include ] && rm -rf include
cp -r $TEMP_DIR/$GIT_DIR/include include
cp $GIT_DIR/src/mqtt/*.h include/QtMqtt
[ -d lib ] && rm -rf lib
cp -r $TEMP_DIR/$GIT_DIR/lib lib

rm -rf $TEMP_DIR
echo "Installation complete"