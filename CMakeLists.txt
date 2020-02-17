
cmake_minimum_required(VERSION 3.16)

project(LibAM
    VERSION 0.1.0
    HOMEPAGE_URL "www.github.com/skyb0rg007/Libam"
    LANGUAGES C)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(Doxygen MODULE OPTIONAL_COMPONENTS dot) # For docs
find_package(GLIB2) # For testing
find_package(Systemd) # For testing

if(NOT CMAKE_USE_PTHREADS_INIT)
    message(FATAL_ERROR "Only pthreads is currently supported")
endif()

# configure_file(
    # ${CMAKE_CURRENT_LIST_DIR}/include/am/ma
    # ${CMAKE_CURRENT_BINARY_DIR}/include
    # @ONLY)

add_library(am
    include/am/alloc.h
    include/am/atomic.h
    include/am/logging.h
    include/am/macros.h
    include/am/threads.h
    include/am/utils.h

    # include/concurrent/array.h
    # include/concurrent/fifo.h
    # include/concurrent/hashtable.h
    include/concurrent/ring_buffer.h

    include/data/hash.h
    include/data/hashtable.h
    include/data/hlist.h
    include/data/list.h

    src/logging.c
    src/alloc.c
    src/concurrent-ring_buffer.c
    )
target_link_libraries(am
    PUBLIC
    Threads::Threads)
target_include_directories(am
    PUBLIC
    include
    ${CMAKE_CURRENT_BINARY_DIR}/include)

# Testing

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

# Documentation

if(DOXYGEN_FOUND)
    set(DOXYGEN_GENERATE_HTML TRUE)
    set(DOXYGEN_EXTRACT_STATIC TRUE)
    set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs)
    set(DOXYGEN_STRIP_FROM_PATH "include,${CMAKE_CURRENT_BINARY_DIR}")
    doxygen_add_docs(docs
        include
        ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating Doxygen documentation")
endif()


