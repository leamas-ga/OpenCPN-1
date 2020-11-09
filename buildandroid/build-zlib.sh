#!/bin/bash
set -x
LIB_HOME=${LIB_HOME:-$PWD}
NDK_HOME=${NDK_HOME:-$HOME/lib/android-ndk-r21d}
cd $LIB_HOME

SYSROOT=${SYSROOT:-$HOME/lib/android-sysroot}

ARCH=aarch64-linux-android
TOOLCHAINS=$NDK_HOME/toolchains/aarch64-linux-android-4.9/prebuilt/linux-x86_64
LLVM=$NDK_HOME/toolchains/llvm/prebuilt/linux-x86_64

export PATH=$LLVM/bin:$TOOLCHAINS/bin:$TOOLCHAINS/$ARCH/bin:$PATH

export CC=aarch64-linux-android21-clang
export CXX=aarch64-linux-android21-clang++

./configure \
    --prefix=/usr \
    --libdir=/usr/lib/$ARCH 

make -j12
make install
