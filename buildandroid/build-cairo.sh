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

PKG_CONFIG_SYSROOT_DIR=$SYSROOT \
PKG_CONFIG_LIBDIR=$SYSROOT/usr/lib/$ARCH/pkgconfig \
./configure \
    --host=$ARCH \
    --build=x86_64-linux-gnu \
    --prefix=/usr \
    --libdir=/usr/lib/$ARCH \
    --with-sysroot=$SYSROOT \
    --enable-shared=yes \
    --enable-static=no \
    --enable-gtk-doc-html=no \
    --enable-xlib=no \
    --enable-xlib-xrender=no \
    --enable-gl=no

make -j12
make install
rm -f $DESTDIR/usr/lib/$ARCH/libcairo.la
