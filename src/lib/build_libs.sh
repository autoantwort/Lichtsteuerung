#!/bin/bash

echo "Build segvcatch"
cd segvcatch
./build.sh
cd ..

echo $'\n\nBuild libbacktrace'
cd libbacktrace
./build.sh
cd ..

echo "Lib installation complete"
