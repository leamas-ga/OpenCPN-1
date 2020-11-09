#!/bin/bash
set -x
LIB_HOME=${LIB_HOME:-$PWD}
NDK_HOME=${NDK_HOME:-$HOME/lib/android-ndk}
cd $LIB_HOME/expat

SYSROOT=${SYSROOT:-$HOME/lib/android-sysroot}

ARCH=aarch64-linux-android
TOOLCHAINS=$NDK_HOME/toolchains/aarch64-linux-android-4.9/prebuilt/linux-x86_64
LLVM=$NDK_HOME/toolchains/llvm/prebuilt/linux-x86_64

export PATH=$TOOLCHAINS/bin:$TOOLCHAINS/$ARCH/bin:$PATH
export PATH=$LLVM/bin:$PATH

toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android21-clang
export CC=aarch64-linux-android21-clang
export CXX=aarch64-linux-android21-clang++

autoreconf -fi

./configure \
    --host=$ARCH \
    --build=x86_64-linux-gnu \
    --prefix=/usr \
    --libdir=/usr/lib/$ARCH \
    --with-sysroot=$SYSROOT


make -j12
make install
rm -f $DESTDIR/usr/lib/$ARCH/libexpat.la
