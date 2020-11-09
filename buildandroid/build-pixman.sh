#!/bin/bash
set -x
LIB_HOME=${LIB_HOME:-$PWD}
NDK_HOME=${NDK_HOME:-$HOME/lib/android-ndk-r21d}
SYSROOT=${SYSROOT:-$HOME/lib/android-sysroot}
cd $LIB_HOME

ARCH=aarch64-linux-android
TOOLCHAINS=$NDK_HOME/toolchains/aarch64-linux-android-4.9/prebuilt/linux-x86_64
LLVM=$NDK_HOME/toolchains/llvm/prebuilt/linux-x86_64

export PATH=$LLVM/bin:$TOOLCHAINS/bin:$TOOLCHAINS/$ARCH/bin:$PATH

export CC="aarch64-linux-android21-clang"
export CXX="aarch64-linux-android21-clang++"
autoreconf -fi

CFLAGS="-Icontrib/android/include" \
PNG_LIBS="-lpng -lz" \
LDFLAGS="-Wl,-rpath-link,$SYSROOT/usr/lib/$ARCH" \
PKG_CONFIG_LIBDIR=$SYSROOT/usr/lib/$ARCH/pkgconfig \
./configure \
    --host=$ARCH \
    --build=x86_64-linux-gnu \
    --prefix=/usr \
    --libdir=/usr/lib/$ARCH \
    --with-sysroot=$SYSROOT

make 
make install
rm -f $DESTDIR/usr/lib/$ARCH/libpixman-1.la
