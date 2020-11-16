SET(CMAKE_SYSTEM_NAME Android)

if ("${SYSROOT}" STREQUAL "")
  set(SYSROOT "$ENV{HOME}/lib/android-sysroot")
endif ()
if ("${NDK_HOME}" STREQUAL "")
  set(NDK_HOME "$ENV{HOME}/lib/android-ndk")
endif ()
set(PLATFORM "aarch64-linux-android")


# Set up compilers and path to use proper build tools (as, ld etc.)
string(PREPEND $ENV{PATH} 
  "${NDK_HOME}/toolchains/llvm/prebuilt/linux-x86_64:"
  "${NDK_HOME}/toolchains/aarch64-linux-android-4.9/prebuilt/linux-x86_64:"
)
set(CMAKE_C_COMPILER aarch64-linux-android21-clang)
set(CMAKE_CXX_COMPILER aarch64-linux-android21-clang++)
set(CMAKE_ANDROID_NDK  ${NDK_HOME})

# pkg-config reconfigureation
set(ENV{PKG_CONFIG_LIBDIR} ${SYSROOT}/usr/lib/${PLATFORM}/pkgconfig)
set(ENV{PKG_CONFIG_SYSROOT_DIR} ${SYSROOT})
set(ENV{PKG_CONFIG_PATH} "${SYSROOT}/usr/lib/${PLATFORM}/pkgconfig")

set(Qt5_DIR ${SYSROOT}/usr/Qt5 CACHE STRING "Qt5 install dir base")
set(wxWidgets_CONFIG_EXECUTABLE "${SYSROOT}/usr/bin/wx-config" 
  CACHE STRING "WxWidgets configuration tool location (wx-config)."
)

# Target environment location
SET(CMAKE_FIND_ROOT_PATH ${SYSROOT}/usr/lib/${PLATFORM} ${SYSROOT})

# Adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
