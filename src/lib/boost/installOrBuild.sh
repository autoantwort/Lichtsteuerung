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
elif [[ "$OSTYPE" == "msys" ]] || ! [[ -z "$GITLAB_CI" ]]; then
    # windows or gitlab ci
    echo "You are on Windows"
    GIT_DIR="boost.git"
    if ! [[ -z "$GITLAB_CI" ]]; then
        # use the cross compiler on the ci
        TOOLSET="gcc-mxe"
    else
        TOOLSET="gcc"
    fi

    if ! [ -d "$GIT_DIR" ]; then
        echo "We have to clone the repo, this will take approximately 6.5 minutes"
        git clone --depth 1 --shallow-submodules --recurse-submodules --jobs 32 https://github.com/boostorg/boost.git "$GIT_DIR"
        cd "$GIT_DIR"
        echo "Init the build"
        if ! [[ -z "$GITLAB_CI" ]]; then
            # in the ci we need the normal compiler to build the b2 tool
            OLD_CXX="$CXX"
            export CXX="/usr/bin/g++"
        fi
        # configure the build
        ./bootstrap.sh
        if ! [[ -z "$GITLAB_CI" ]]; then
            # set to old value, so that the cross compiling works
            export CXX="$OLD_CXX"
            # copied from https://github.com/mxe/mxe/blob/90da28981e48b9c8e51d9ad20c715f1869a9e67b/src/boost.mk#L30
            TARGET="x86_64-w64-mingw32.shared.posix"
            echo "using gcc : mxe : ${TARGET}-g++ : <rc>${TARGET}-windres <archiver>${TARGET}-ar <ranlib>${TARGET}-ranlib ;" > 'project-config.jam'
        fi
    else
        cd "$GIT_DIR"
        git pull
    fi
    if [[ -z "$GITLAB_CI" ]]; then
        EXE=".exe" # windows need the .exe postfix
    fi
    echo "Build boost, this will take approximately 3 minutes, if boost is not already build."
    ./b2$EXE --prefix=../ -q --layout=tagged --with-coroutine --with-context abi=ms architecture=x86 binary-format=pe target-os=windows toolset=$TOOLSET link=shared threading=multi runtime-link=shared address-model=64 install
    echo "Boost build is finished"
else
    # linux?
    echo "You are on Linux"
    echo "Linux is currently not supported (no mashine to test). You must install boost yourself. Maybe look at the windows version in this file."
fi
