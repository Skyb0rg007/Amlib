
include(FindPackageHandleStandardArgs)
find_package(PkgConfig)

pkg_check_modules(PKG_SYSTEMD systemd)

find_library(SYSTEMD_LIBRARY
    systemd
    HINTS ${PKG_SYSTEMD_LIBRARIES})

find_path(SYSTEMD_INCLUDE_DIR
    systemd/sd-journal.h
    HINTS ${PKG_SYSTEMD_INCLUDE_DIRS})

if(PKG_SYSTEMD_VERSION)
    set(SYSTEMD_VERSION ${PKG_SYSTEMD_VERSION})
else()
    execute_process(
        COMMAND systemctl --version
        OUTPUT_VARIABLE output)
    string(REGEX REPLACE ".*systemd ([0-9]+).*" "\\1" SYSTEMD_VERSION "${output}")
    unset(output)
endif()

find_package_handle_standard_args(Systemd
    REQUIRED_VARS SYSTEMD_LIBRARY SYSTEMD_INCLUDE_DIR
    VERSION_VAR SYSTEMD_VERSION)

if (Systemd_FOUND AND NOT TARGET Systemd::systemd)
    add_library(Systemd::systemd UNKNOWN IMPORTED)
    set_target_properties(Systemd::systemd PROPERTIES
        IMPORTED_LOCATION "${SYSTEMD_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${SYSTEMD_INCLUDE_DIR}")
endif()

