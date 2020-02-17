
include(FindPackageHandleStandardArgs)
find_package(PkgConfig)

pkg_check_modules(PKG_GLIB2 glib-2.0)

find_library(GLIB2_LIBRARY
    glib-2.0
    HINTS ${GLIB2_LIBRARIES})

find_path(GLIB2_MAIN_INCLUDE_DIR
    glib.h
    PATH_SUFFIXES glib-2.0
    HINTS ${PKG_GLIB2_INCLUDE_DIRS})

find_path(GLIB2_CONF_INCLUDE_DIR
    glibconfig.h
    PATH_SUFFIXES glib-2.0
    HINTS ${PKG_GLIB2_INCLUDE_DIRS} ${CMAKE_SYSTEM_LIBRARY_PATH})

set(GLIB2_INCLUDE_DIRS ${GLIB2_MAIN_INCLUDE_DIR} ${GLIB2_CONF_INCLUDE_DIR})

if(PKG_GLIB2_VERSION)
    set(GLIB2_VERSION ${PKG_GLIB2_VERSION})
elseif(EXISTS "${GLIB2_CONF_INCLUDE_DIR}/glibconfig.h")
    file(STRINGS "${GLIB2_CONF_INCLUDE_DIR}/glibconfig.h" vsnstr
        REGEX "GLIB_(MAJOR|MINOR|MICRO)_VERSION")
    string(REGEX REPLACE ".*#define GLIB_MAJOR_VERSION ([0-9]+).*" "\\1" major "${vsnstr}")
    string(REGEX REPLACE ".*#define GLIB_MINOR_VERSION ([0-9]+).*" "\\1" minor "${vsnstr}")
    string(REGEX REPLACE ".*#define GLIB_MICRO_VERSION ([0-9]+).*" "\\1" micro "${vsnstr}")
    set(GLIB2_VERSION ${major}.${minor}.${micro})
    unset(vsnstr)
    unset(major)
    unset(minor)
    unset(micro)
endif()

find_package_handle_standard_args(GLIB2
    REQUIRED_VARS GLIB2_LIBRARY GLIB2_INCLUDE_DIRS
    VERSION_VAR GLIB2_VERSION)

if(GLIB2_FOUND AND NOT TARGET GLIB2::GLIB2)
    add_library(GLIB2::GLIB2 UNKNOWN IMPORTED)
    set_target_properties(GLIB2::GLIB2 PROPERTIES
        IMPORTED_LOCATION "${GLIB2_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${GLIB2_INCLUDE_DIRS}")
endif()

