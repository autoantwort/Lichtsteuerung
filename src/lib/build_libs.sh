#!/bin/bash

echo "Build segvcatch"
cd segvcatch
chmod +x build.sh
./build.sh
cd ..

echo $'\n\nBuild libbacktrace'
cd libbacktrace
chmod +x build.sh
./build.sh
cd ..

echo "Lib installation complete"