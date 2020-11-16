# - Try to find Qt5Androidextras in sysroot, only for cross-compiling setup.
#
# After run, sets
#  QT5_FOUND - system has Qt5
#
# If found, exports the QT:Qt5 import library with includes, libraries
# and definitions. The individual parts are available as Qt5::Core,
# Qt5::Widgets etc.
#

if (TARGET Qt5::Qt5)
  return ()
endif ()

if (QT5_INCLUDE_DIRS AND QT5_LIBRARIES)
  set(Qt5_FIND_QUIETLY TRUE)  # in cache already
endif ()


# Given a component name set up variables for library names,
# the include subdir, the upper-case basename and the header
# to look for, all of which with a _comp prefix.
macro(_set_comp_vars comp)
  set(_comp_libs Qt5${comp} Qt5${comp}_${_qt_platform})
  set(_comp_incl_subdir "Qt${comp}")
  set(_comp_basename QT${comp})
  string(TOUPPER ${_comp_basename} _comp_basename)
  if (${comp} STREQUAL "AndroidExtras")
    set(_comp_header qandroidbinder.h)
  elseif (${comp} STREQUAL "Core")
    set(_comp_header qcoreapplication.h)
  elseif (${comp} STREQUAL "OpenGL")
    set(_comp_header qgl.h)
  elseif (${comp} STREQUAL "Widgets")
    set(_comp_header qabstractbutton.h)
  else ()
    message(FATAL_ERROR "FindQt5: Unsupported component: ${comp}")
  endif ()
endmacro ()


# Set up _qt_platform, QT name of android ABI.
if ("${PLATFORM}" STREQUAL "aarch64-linux-android")
  set(_qt_platform arm64-v8a)
elseif ("${PLATFORM}" STREQUAL "arm-linux-androideabi")
  set(_qt_platform armeabi-v7a)
else ()
  message(WARNING "Unknown PLATFORM: \"${PLATFORM}\"")
  set(_qt_platform not-a-library-suffix)
endif ()


# Loop through all given components, check if they are there,
# collect headers and libs. Abort if required component is missing.
# Add an interface library for each component.
set(Qt5_INCLUDE_DIRS)
set(Qt5_LIBRARIES)
foreach (c ${Qt5_FIND_COMPONENTS})
  _set_comp_vars(${c})
  find_path(${_comp_basename}_INCLUDE_DIR 
    NAMES ${_comp_header}
    HINTS ${Qt5_DIR}
    PATH_SUFFIXES usr/include  include include/${_comp_incl_subdir}
  )
  find_library(${_comp_basename}_LIBRARIES 
    NAMES ${_comp_libs}
    HINTS ${Qt5_DIR}
    PATH_SUFFIXES lib lib/${PLATFORM}
  )
  string(TOUPPER Qt${c} _basename)
  if (${_basename}_INCLUDE_DIR AND ${_basename}_LIBRARIES)
    list(APPEND Qt5_INCLUDE_DIRS ${_basename}_INCLUDE_DIR)
    list(APPEND Qt5_LIBRARIES ${_basename}_LIBRARIES)
  elseif (Qt5_FIND_REQUIRED_${c})
    message(FATAL_ERROR "Qt5: Required component ${c} not found")
  endif ()
  if (NOT _${c}_lib_done)
    add_library(_${c} INTERFACE)
    target_link_libraries(_${c} INTERFACE ${${_basename}_LIBRARIES})
    target_include_directories(_${c} INTERFACE ${${_basename}_INCLUDE_DIRS})
    add_library(Qt5::${c} ALIAS _${c})
    set(_${c}_lib_done 1)
  endif ()
endforeach ()

# Wrap up: create interface directory
list(LENGTH "${Qt5_LIBRARIES}" _lib_count)
if ("${_lib_count}" GREATER 0)
  add_library(_QT5 INTERFACE)
  target_link_libraries(_QT5 INTERFACE ${QT5_LIBRARIES})
  target_include_directories(_QT5 INTERFACE ${QT5_INCLUDE_DIRS})
  add_library(Qt5::Qt5 ALIAS _QT5)
endif ()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  Qt5 DEFAULT_MSG Qt5_INCLUDE_DIRS Qt5_LIBRARIES
)
mark_as_advanced(Qt5_INCLUDE_DIRS Qt5_LIBRARIES )
