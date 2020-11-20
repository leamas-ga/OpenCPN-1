## OpenCPN Android build setup.


The android setup eventually creates an APK file which can be installed
in an Android device. The build uses a sysroot which contains a mirror
of what's installed in the Android runtime. The cross-compilation
links to the sysroot in build time and to the android installed libs
in runtime.

The three steps is to create the sysroot, build OpenCPN and create the APK.


### Sysroot

#### Install the ndk, create initial sysroot
Check https://developer.android.com/ndk/downloads and download the ndk
file. Used version: `android-ndk-r21d-linux-x86_64.zip`. Unzip the
archive to some work dir like ~/lib, creating `~/lib/android-ndk-r21d`.
Then, set up the initial sysroot, a copy from the ndk:

    $ cd ~/lib
    $ cp -ar android-ndk-r21d/sysroot .
    $ mv sysroot android-sysroot


#### Install sdk

Download the command line tools: visit https://developer.android.com/studio,
search for Command Line Tools. Download zip file and unzip it to the work
directory, creating `~/lib/cmdline-tools`.

Now, create an empty directory for the sdk:

    $ cd ~/lib
    $ mkdir android_sdk

List available stuff:

    $ alias sdkmanager=$HOME/lib/cmdline-tools/bin/sdkmanager
    $ sdkmanager --sdk_root=$HOME/lib/android_sdk --list

Install platform and build tools:

    $ sdkmanager \
          --sdk_root=$HOME/lib/android_sdk --install "platforms;android-21"
    $ sdkmanager \
          --sdk_root=$HOME/lib/android_sdk --install "build-tools;21.1.2"

Link platform libraries into the sysroot:

    $ platform=$HOME/lib/android-ndk/toolchains/llvm/prebuilt/linux-x86_64
    $ cd ~/lib/android-sysroot/usr/lib/aarch64-linux-android
    $ for f in $platform/sysroot/usr/lib/aarch64-linux-android/lib*; do \
         ln -sf $f . ; \
      done
    $ for f in $platform/sysroot/usr/lib/aarch64-linux-android/21/*; do \
         ln -sf $f . ; \
      done
    $ find $platform/sysroot/usr/include -type f \
         -exec cp -ar {} ~/lib/android-sysroot/usr/include \;



#### Build Qt5 and install into sysroot

Building from git is documented in https://wiki.qt.io/Building_Qt_5_from_Git.
After installing dependencies as documented there, do:

        -sysroot $HOME/lib/android-sysroot \

    $ git clone git://code.qt.io/qt/qt5.git
    $ cd qt5
    $ git checkout v5.14.2
    $ ./init-repository --module-subset essential,qtandroidextras  --branch
    $ ./configure -xplatform android-clang \
        -opensource -confirm-license  \
        -android-abis arm64-v8a \
        -android-ndk $HOME/lib/android-ndk-r21d \
        -android-sdk $HOME/lib/android_sdk \
        -android-ndk-host linux-x86_64 \
        -extprefix $HOME/lib/android-sysroot/usr/Qt5 \
        -prefix /usr/Qt5 \
        -disable-rpath \
        -nomake tests -nomake examples \
        -no-warnings-are-errors
    $ make -j12
    $ rm -rf $HOME/lib/android-sysroot/usr/Qt5
    $ make install
    $ make -C qtandroidextras install

Note: The logical way to use -sysroot when building qt does not work, it's
incompatible when using the ndk. We trust that our own sysroot is a
superset of the files in ndk's one.

Install some build compat links:

    $ cd ~/lib/android-sysroot/usr/lib/aarch64-linux-android
    $ ln -s ../../Qt5/lib/libQt5Core_arm64-v8a.so libQt5Core.so
    $ ln -s ../../Qt5/lib/libQt5Widgets_arm64-v8a.so libQt5Widgets.so
    $ ln -s ../../Qt5/lib/libQt5Gui_arm64-v8a.so libQt5Gui.so
    $ ln -s ../../Qt5/lib/libQt5OpenGL_arm64-v8a.so libQt5OpenGL.so
    $ ln -s ../../Qt5/lib/libQt5Test_arm64-v8a.so libQt5Test.so
    $ ln -s ../../Qt5/lib/libQt5AndroidExtras_arm64-v8a.so libQt5AndroidExtras.so


#### Build libcurl and install into sysroot

Download the sources from https://curl.se/download.html. Using version
7.73, unzip the downloaded file creating for example ~/lib/curl-7.73

Build using something like:

    $ LIB_HOME=~/lib/curl-7.73.0 \
    NDK_HOME=~/lib/android-ndk-r21d/ \
    DESTDIR=~/lib/android-sysroot/ \
    SYSROOT=~/lib/android-sysroot \
    ./build-curl.sh

TODO: Add ssl libs.


#### Build libexpat and install into sysroot

Using the git sources and working ~/lib:

    $ cd ~/lib
    $ git clone https://github.com/libexpat/libexpat.git
    $ cd libexpat
    $ git checkout git checkout R_2_2_10

Build using something like this in buildandroid/ directory:

    $ LIB_HOME=~/lib/libexpat \
    NDK_HOME=~/lib/android-ndk-r21d/ \
    DESTDIR=~/lib/android-sysroot/ \
    SYSROOT=~/lib/android-sysroot \
    ./build-expat.sh


#### Build libarchive and install into sysroot

Download the sources from https://www.libarchive.org/ and unpack into
working directory ~/lib, creating ~/lib/libarchive-3.4.3 or so.

    $ LIB_HOME=~/lib/libarchive-3.4.3 \
    NDK_HOME=~/lib/android-ndk-r21d/ \
    DESTDIR=~/lib/android-sysroot/ \
    SYSROOT=~/lib/android-sysroot \
    ./build-libarchive.sh


#### Build libpng and install into sysroot

Download the sources from http://www.libpng.org/pub/png/libpng.html and
unpack into working directory ~/lib, creating ~/lib/libpng-1.6.37 or so.

    $ LIB_HOME=~/lib/libpng-1.6.37 \
    NDK_HOME=~/lib/android-ndk-r21d/ \
    DESTDIR=~/lib/android-sysroot/ \
    SYSROOT=~/lib/android-sysroot \
    ./build-libpng.sh


#### Build zlib and install into sysroot

zlib is included in the ndk sysroot. However, we need the devel support 
(pkg-config and header).

Download the sources from https://zlib.net/, unpack into working directory
 ~/lib, creating ~/lib/zlib-1.2.11 or so.  Build and install using 

    $ LIB_HOME=~/lib/zlib-1.2.11 \
    NDK_HOME=~/lib/android-ndk-r21d/ \
    DESTDIR=~/lib/android-sysroot \
    SYSROOT=~/lib/android-sysroot \
    ./build-zlib.sh


#### Build pixman and install into sysroot

Download the sources from https://www.cairographics.org/releases/,
unpack into working directory ~/lib, creating ~/lib/pixman-0.40.0 or so.

Build and install using:

    $ LIB_HOME=~/lib/pixman-0.40.0 \
    NDK_HOME=~/lib/android-ndk-r21d/ \
    DESTDIR=~/lib/android-sysroot \
    SYSROOT=~/lib/android-sysroot \
    ./build-pixman.sh


#### Build freetype and install into sysroot

Download the sources from https://download.savannah.gnu.org/releases/freetype
unpack into working directory ~/lib, creating ~/lib/freetype-2.10.4 or so.

Build and install using 

    $ LIB_HOME=~/lib/freetype-2.10.4 \
    NDK_HOME=~/lib/android-ndk-r21d/ \
    DESTDIR=~/lib/android-sysroot \
    SYSROOT=~/lib/android-sysroot \
    ./build-freetype2.sh


#### Build cairo and install into sysroot

Download the sources from https://www.cairographics.org/releases/
unpack into working directory ~/lib, creating ~/lib/cairo-1.16.0 or so.

Build and install using 

    $ LIB_HOME=~/lib/cairo-1.16.0 \
    NDK_HOME=~/lib/android-ndk-r21d/ \
    DESTDIR=~/lib/android-sysroot \
    SYSROOT=~/lib/android-sysroot \
    ./build-cairo.sh


#### Build and install the assetbridge support library 

The assetbridge library is used by C++ code to access resources normally
only visible in Java. For OpenCPN, this is about accessing our fixed assets
from C++. This used to live in https://bitbucket.org/smh377/assetbridge,
but upstream seems dead. The files under buildandroid/assetbridge is a
subset of the original code (?).

Build and install in the buildandroid/ directory using

    $ NDK_HOME=~/lib/android-ndk-r21d/ \
    DESTDIR=~/lib/android-sysroot/ \
    ./build-assetbridge.sh

The resulting asset files will be found by the C++ code at the location:
/data/data/org.opencpn.opencpn/cache/


#### Build wxWidgets and install into sysroot

Checkout the wxWidgets sources

    $ cd ~/lib
    $ git clone  https://github.com/wxWidgets/wxWidgets.git
    $ cd wxWidgets
    $ git checkout v3.1.4
    $ for m in src/expat src/tiff src/png src/zlib src/jpeg 3rdparty/catch; do \
        git submodule update --init $m; \
      done

Apply (unfortunately required) patches:

    $ git am wherever-this-README.md-lives/patches/wxWidgets/*

Configure, build and install into sysroot:

    $ QT5_CUSTOM_DIR=$HOME/lib/android-sysroot/usr/Qt5 \
      ../configure --with-qt \
        --prefix=/usr \
        --libdir=/usr/lib/aarch64-linux-android \
        --build=x86_64-unknown-linux-gnu  --host=aarch64-linux-android \
        --disable-shared \
        --disable-compat28 --disable-shared --disable-arttango --enable-image \
        --disable-dragimage --disable-sockets --with-libtiff=no \
        --without-opengl  --disable-xrc \
        --without-liblzma \
        --with-cairo=yes \
        --disable-detect_sm \
        --disable-cmdline --disable-miniframe --disable-mdi --enable-debug \
        --disable-stc --disable-ribbon --disable-propgrid --disable-timepick \
        --disable-datepick --disable-xlocale --disable-intl
    $ make -j12
    $ make DESTDIR=$HOME/lib/android-sysroot install

Fix wx-config broken symlink with absolute path:

    $ cd ~/lib/android-sysroot/usr/bin/
    $ ln -sf ../lib/aarch64-linux-android/wx/config/aarch64-linux-android-qt-unicode-static-* wx-config 


TODO:
  - Make the sysroot acceptable by the ndk tools (basically, build the sysroot inside the ndk)
  - Adapt to circlei defaults:
      + ndk and sdk installations at /opt/android/
      + Use API 28 and sdk r20:

           $ ls /opt/android
           android-ndk-r20  sdk

           $ ls /opt/android/sdk/platforms/
           android-28
