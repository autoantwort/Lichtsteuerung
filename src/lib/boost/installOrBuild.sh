#!/bin/bash

if [[ "$OSTYPE" == "darwin"* ]]; then
    # Mac OSX
    echo "You are on macOS. Check if brew is installed..."
    if ! [ -x "$(command -v brew)" ]; then
        echo 'Error: you need brew to install boost. See https://brew.sh/.' >&2
        exit 1
    fi
    if [[ -z "$(brew ls --versions boost)" ]]; then
        echo "Installing boost..."
        brew install boost
    else
        echo "Updating boost..."
        brew upgrade boost
    fi
    if [[ -z "$(brew ls --versions boost-build)" ]]; then
        echo "Installing boost-build..."
        brew install boost-build
    else
        echo "Updating boost-build..."
        brew upgrade boost-build
    fi
    echo "Boost installation complete"
elif [[ "$OSTYPE" == "msys" ]]; then
    # windows 
    echo "You are on Windows"
    GIT_DIR="boost.git"
    if [! -d "$GIT_DIR" ]; then
        echo "We have to clone the repo, this will take approximately 6.5 minutes"
        git clone --depth 1 --shallow-submodules --recurse-submodules https://github.com/boostorg/boost.git "$GIT_DIR"
        cd "$GIT_DIR"
        echo "Init the build"
        ./bootstrap.sh
    else
        cd "$GIT_DIR"
        git pull
    fi
    echo "Build boost, this will take approximately 3 minutes, if boost is not already build."
    ./b2.exe --prefix=../ --layout=tagged --with-coroutine --with-context toolset=gcc link=shared threading=multi runtime-link=shared address-model=64 install
    echo "Boost build is finished"
else
    # linux?
    echo "You are on Linux"
    echo "Linux is currently not supported (no mashine to test). You must install boost yourself. Maybe look at the windows version in this file." >&2
fi


