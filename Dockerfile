FROM dockcross/base:latest
MAINTAINER Leander Schulten "leander.Schulten@rwth-aachen.de"

ENV WINEARCH win64
ARG MXE_TARGET_ARCH=x86_64
ARG MXE_TARGET_THREAD=.posix
ARG MXE_TARGET_LINK=shared

#
# Before including this script, make sure to set:
#
# WINEARCH environment variable to either "win64" or "win32"
# MXE_TARGET_ARCH argument to either "x86_64" or "i686". See http://mxe.cc/
# MXE_TARGET_THREAD argument to either "" or ".posix". Default is win32. See http://mxe.cc/
# MXE_TARGET_LINK argument to either "static" or "shared"
#
# For example:
#
#  ENV WINEARCH win64
#  ARG MXE_TARGET_ARCH=x86_64
#  ARG MXE_TARGET_THREAD=
#  ARG MXE_TARGET_LINK=shared
#

ENV CMAKE_TOOLCHAIN_FILE /usr/src/mxe/usr/${MXE_TARGET_ARCH}-w64-mingw32.${MXE_TARGET_LINK}${MXE_TARGET_THREAD}/share/cmake/mxe-conf.cmake

ARG DEBIAN_FRONTEND=noninteractive

# next line from https://github.com/romaricp/kit-starter-symfony-4-docker/issues/10
RUN sudo rm -rf /var/lib/apt/lists/* && sudo apt update

#
# WINE is used as an emulator for try_run and tests with CMake.
#
# Other dependencies are from the listed MXE requirements:
#   http://mxe.cc/#requirements
# 'cmake' is omitted because it is installed from source in the base image
#
RUN \
  apt-get update && \
  apt-get install --no-install-recommends --yes \
    autoconf \
    automake \
    autopoint \
    bash \
    bison \
    bzip2 \
    flex \
    gettext \
    git \
    g++ \
    g++-multilib \
    gperf \
    intltool \
    libffi-dev \
    libgdk-pixbuf2.0-dev \
    libtool-bin \
    libltdl-dev \
    libssl-dev \
    libxml-parser-perl \
    libc6-dev-i386 \
    lzip \
    make \
    openssl \
    p7zip-full \
    patch \
    perl \
    pkg-config \
    python \
    python-mako \
    ruby \
    scons \
    sed \
    unzip \
    wget \
    wine \
    xz-utils \
  && \
  #
  # Install Wine
  #
  dpkg --add-architecture i386 && \
  apt-get update && \
  apt-get install -y wine32 && \
  wine hostname && \
  #
  # Download MXE sources
  #
  cd /usr/src && \
  git clone https://github.com/mxe/mxe.git && \
  cd mxe && \
  # git checkout ${MXE_GIT_TAG} && \
  #
  # Configure "settings.mk" required to build MXE
  #
  cd /usr/src/mxe && \
  echo "MXE_TARGETS := ${MXE_TARGET_ARCH}-w64-mingw32.${MXE_TARGET_LINK}${MXE_TARGET_THREAD}" > settings.mk && \
  echo "LOCAL_PKG_LIST := cc cmake"                                              >> settings.mk && \
  echo ".DEFAULT local-pkg-list:"                                                >> settings.mk && \
  echo "local-pkg-list: \$(LOCAL_PKG_LIST)"                                      >> settings.mk && \
  echo "MXE_PLUGIN_DIRS := plugins/gcc8"                                      >> settings.mk && \
  #
  # Build MXE
  #
  cd /usr/src/mxe && \
  make -j$(nproc) && \
  #
  # Cleanup: By keeping the MXE build system (Makefile, ...), derived images will be able to install
  #          additional packages.
  #
  rm -rf log pkg && \
  #
  # Update MXE toolchain file
  #
  echo 'set(CMAKE_CROSSCOMPILING_EMULATOR "/usr/bin/wine")' >> ${CMAKE_TOOLCHAIN_FILE} && \
  #
  # Replace cmake and cpack binaries
  #
  cd /usr/bin && \
  rm cmake cpack && \
  ln -s /usr/src/mxe/usr/bin/${MXE_TARGET_ARCH}-w64-mingw32.${MXE_TARGET_LINK}${MXE_TARGET_THREAD}-cmake cmake && \
  ln -s /usr/src/mxe/usr/bin/${MXE_TARGET_ARCH}-w64-mingw32.${MXE_TARGET_LINK}${MXE_TARGET_THREAD}-cpack cpack

ENV PATH ${PATH}:/usr/src/mxe/usr/bin
ENV CROSS_TRIPLE ${MXE_TARGET_ARCH}-w64-mingw32.${MXE_TARGET_LINK}${MXE_TARGET_THREAD}
ENV AS=/usr/src/mxe/usr/bin/${CROSS_TRIPLE}-as \
    AR=/usr/src/mxe/usr/bin/${CROSS_TRIPLE}-ar \
    CC=/usr/src/mxe/usr/bin/${CROSS_TRIPLE}-gcc \
    CPP=/usr/src/mxe/usr/bin/${CROSS_TRIPLE}-cpp \
    CXX=/usr/src/mxe/usr/bin/${CROSS_TRIPLE}-g++ \
    LD=/usr/src/mxe/usr/bin/${CROSS_TRIPLE}-ld \
    FC=/usr/src/mxe/usr/bin/${CROSS_TRIPLE}-gfortran


# Build-time metadata as defined at http://label-schema.org
ARG BUILD_DATE
ARG IMAGE=dockcross/windows-shared-x64-posix
ARG VERSION=latest
ARG VCS_REF
ARG VCS_URL
LABEL org.label-schema.build-date=$BUILD_DATE \
      org.label-schema.name=$IMAGE \
      org.label-schema.version=$VERSION \
      org.label-schema.vcs-ref=$VCS_REF \
      org.label-schema.vcs-url=$VCS_URL \
      org.label-schema.schema-version="1.0"
ENV DEFAULT_DOCKCROSS_IMAGE ${IMAGE}:${VERSION}

# install qt packages
WORKDIR /usr/src/mxe
RUN make qtbase
RUN make qtdeclarative
RUN make qtimageformats
RUN make qtnetworkauth
RUN make qtquickcontrols2
RUN make qtserialport
RUN make qttools
RUN make qtwebsockets
RUN make zlib
RUN make quazip

#set path to mxe compiler
RUN export PATH=/usr/src/mxe/usr/bin:$PATH
