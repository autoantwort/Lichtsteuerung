#!/bin/bash

echo "Build segvcatch"
cd segvcatch
./build.sh
cd ..

echo $'\n\nBuild libbacktrace'
cd libbacktrace
./build.sh
cd ..

echo $'\n\nBuild boost'
cd boost
./installOrBuild.sh
cd ..

echo $'\n\nBuild Aubio'
cd aubio
./build_aubio.sh
cd ..

echo $'\n\nBuild RtAudio'
cd RtAudio
./build_rtAudio.sh
cd ..

echo "Lib installation complete"
