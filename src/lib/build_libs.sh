#!/bin/bash
set -e
set -o xtrace
cd $(dirname "$0")

source ./scripts/set_env.sh

echo "Build segvcatch"
cd segvcatch
./build.sh
cd ..

echo $'\n\nUpdate Qt MQTT'
cd qtmqtt
./getAndUpdate.sh
cd ..


echo "Lib installation complete"
