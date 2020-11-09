#!/bin/bash
set -x
LIB_HOME=${LIB_HOME:-$PWD}
NDK_HOME=${NDK_HOME:-$HOME/lib/android-ndk-r21d}
cd $LIB_HOME

SYSROOT=${SYSROOT:-$HOME/lib/android-sysroot}

ARCH=aarch64-linux-android
TOOLCHAINS=$NDK_HOME/toolchains/aarch64-linux-android-4.9/prebuilt/linux-x86_64
LLVM=$NDK_HOME/toolchains/llvm/prebuilt/linux-x86_64

#export PATH=$TOOLCHAINS/bin:$TOOLCHAINS/$ARCH/bin:$PATH
export PATH=$TOOLCHAINS/bin:$PATH
export PATH=$LLVM/bin:$PATH

export CC=aarch64-linux-android21-clang
export CXX=aarch64-linux-android21-clang++

PKG_CONFIG_SYSROOT_DIR=$SYSROOT \
PKG_CONFIG_LIBDIR=$SYSROOT/usr/lib/$ARCH/pkgconfig \
./configure \
    --prefix=/usr \
    --libdir=/usr/lib/$ARCH \
    --build=x86_64-linux-gnu \
    --host=aarch64-linux-android \
    --without-zlib \
    --with-bzip2=no \
    --with-png=auto \
    --with-harfbuzz=no \
    --with-brotli=no


make -j12
make install
rm -f $DESTDIR/usr/lib/$ARCH/libfreetype.la
