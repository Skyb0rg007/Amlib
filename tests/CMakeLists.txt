
function(am_test name src)
    string(REGEX REPLACE "(.*)/[a-zA-Z_-]+\\.c" "\\1" outdir "${src}")
    add_executable(${name}
        ${src})
    target_link_libraries(${name}
        PRIVATE
        ${ARGN})
    set_property(TARGET ${name} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${outdir})
    add_test(${name} ${outdir}/${name})
endfunction(am_test)

# threads
am_test(thread_test
    threads/thread-test.c
    am)

# logging
am_test(log_test
    logging/log-test.c
    am)
if(GLIB2_FOUND)
    am_test(glib_control
        logging/glib-control.c
        GLIB2::GLIB2)
endif()
if(SYSTEMD_FOUND)
    am_test(journal_control
        logging/journal-control.c
        Systemd::systemd)
endif()
am_test(message_test
    logging/message-test.c
    )
target_include_directories(message_test PRIVATE ../include)

# data
am_test(list_test
    data/list-test.c
    am)
am_test(hashtable_test
    data/hashtable-test.c
    am)
am_test(hlist_test
    data/hlist-test.c
    am)
am_test(rbtree_test
    data/rbtree-test.c
    am)
am_test(vector_test
    data/vector-test.c
    am)
am_test(string_test
    data/string-test.c
    am)

# concurrent
am_test(ring_test
    concurrent/ring-test.c
    am)

# alloc
am_test(alloc_test
    alloc/alloc-test.c
    am)
