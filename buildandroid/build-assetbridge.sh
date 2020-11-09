#!/bin/bash
set -x
NDK_HOME=${NDK_HOME:-$HOME/lib/android-ndk-r21d}
SYSROOT=${SYSROOT:-$HOME/lib/android-sysroot}

ARCH=aarch64-linux-android
TOOLCHAINS=$NDK_HOME/toolchains/aarch64-linux-android-4.9/prebuilt/linux-x86_64
LLVM=$NDK_HOME/toolchains/llvm/prebuilt/linux-x86_64

export PATH=$TOOLCHAINS/bin:$TOOLCHAINS/$ARCH/bin:$PATH
export PATH=$LLVM/bin:$PATH

export CC=aarch64-linux-android21-clang
export CXX=aarch64-linux-android21-clang++

cd $(readlink -fn $(dirname $0))/assetbridge
$NDK_HOME/ndk-build
cp libs/*/libassetbridge.so $SYSROOT/lib/android-sysroot/usr/lib/$ARCH

