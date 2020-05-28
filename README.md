
# Amlib
### General purpose C library, providing portable and efficient utilities

## Contents

- Data Structures
    * List-like
        - `<am/data/list.h>`
            * Intrusive, doubly-linked, circular linked list
        - `<am/data/hlist.h>` 
            * Intrusive, doubly-linked, non-circular linked list
            * Mainly used for implementing the hashtable
    * Hashtable
        - `<am/data/hashtable>`
            * Intrusive, constant-sized, chaining hashtable
            * Great when the number of elements is estimatable
            * Fnva1-hash and Murmurhash are available in am/data/hash.h
    * Concurrent
        - `<am/concurrent/ring_buffer>`
            * Lock-free implementation from ConcurrencyKit
            * Provides optimized functions for single-consumer, single-producer work
            * Barebones functions designed for wrapping
-   Portable utilities
    * Atomics (`<am/atomic.h>`)
        - Provides atomics in an ANSI-compliant manner, modeled after C11 atomics
        - Implementations for C11, GCC, and MSVC
    * Threads (`<am/threads.h>`)
        - Provides threads, mutexes, spinlocks, condition variables, once flags
        - Modeled after C11 threads
        - Implementations for pthreads, Win32, c11 threads
    * Function attributes + more (`<am/macros.h>`)
        - Generally useful macro utilities, such as static assertions and `_Pragma`
        - Portable `inline` and `restrict`, and type-safe `container_of` (if possible)
        - ANSI-compliant alternatives are available for all utilities
- Programming utilities
    * Allocators (`<am/alloc.h>`)
        - Stack allocators, free-list allocators, concurrent allocators, aligned allocators
        - API that allows for efficient swapping out of different allocators within user data structures
    * Structured logging
        - Logging API modeled after glib's structured logging facitilies
        - Allows for programs to filter logs from libraries without recompilation
        - Threadsafe logging
        - Provided log writers for standard FILE streams as well as journald

## Target Platforms

- [x] x86_64 Linux
- [ ] x86_32 Linux
- [ ] Windows (Mingw64)
- [ ] Windows (MSVC)
- [ ] OSX (clang)
- [ ] Emscripten
- [ ] Android
- [ ] iOS

## Building

### CMake

    $ mkdir _build
    $ cd _build
    $ cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr/local \
        -G "Unix Makefiles" \
        -DBUILD_TESTING=ON \
        -DBUILD_SHARED=ON
    $ cmake --build . --target am
    $ cmake --build . --target install

#### CMake Options
- CMAKE\_BUILD\_TYPE
    - Controls compiler optimization options
    - Debug, Release, RelWithDebInfo, MinSizeRel
- CMAKE\_INSTALL\_PREFIX
    - Controls where to install the library
- BUILD\_TESTING
    - Controls whether or not to build tests (default: ON)
- BUILD\_SHARED
    - Controls whether Amlib is built as a shared library (default: ON)

## Concepts

- Relocatable
  * A *relocatable* object is one that can be moved around via `memcpy`.
  * Ex. The head of a singly-linked list is relocatable, but the head of a doubly-linked list is not,
    since the head of a doubly-linked list is pointed to by the last element.
  * A *relocate* function is one that fixes up these sorts of issues caused by `memcpy`.
    - `void relocate(void *data, void *userdata)`
    - `void relocate_dlist(struct am_list_node *n) { n->prev->next = n; n->next->prev = n; }`
  * When writing a relocate function, for example to use `am_vector`, the old pointer location is passed as an argument.
    This pointer does not point to valid memory - use only for comparisons, not for dereference.

