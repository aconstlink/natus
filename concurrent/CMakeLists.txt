

set( sources
    "api.h"
    "result.h"
    "typedefs.h"
    "global.h"
    "global.cpp"
    "protos.h"

    "isleep.hpp"
    "sync_object.hpp"
    "mrsw.hpp"
    "semaphore.hpp"
    "semaphore_guard.hpp"
    "mutex.hpp"
    "thread_pool.hpp"

    "parallel_for.hpp"
    "range_1d.hpp"

    "scheduler.h"
    "scheduler.cpp"
    "task/task.hpp"
    "task/loose_thread_scheduler.hpp"
    "task/fixed_thread_scheduler.hpp"
    )
    
if( NATUS_TARGET_OS_WIN )
    list( APPEND sources ${win32_sources} )
elseif( NATUS_TARGET_OS_LIN )
    list( APPEND sources ${unix_sources} )
endif()

natus_vs_src_dir( sources )

add_library( ${cur_lib_name} ${NATUS_LIBRARY_BUILD_TYPE} ${sources} )
add_library( ${cur_alias_name} ALIAS ${cur_lib_name} )

if( NATUS_TARGET_OS_WIN )
    target_compile_definitions( ${cur_lib_name} PRIVATE -DWIN32_LEAN_AND_MEAN )
endif()

target_link_libraries( ${cur_lib_name}
        PUBLIC natus::log
        PUBLIC natus::memory
    )
if( NATUS_TARGET_OS_LIN )
  target_link_libraries( ${cur_lib_name}
    PUBLIC pthread )
endif()
###########################################################
# SECTION: Build Tree
###########################################################

#natus_export( ${cur_lib_name} )

###########################################################
# SECTION: Install
###########################################################

install_headers( "${sources}" "include/${PROJECT_NAME}/${cur_lib_name}" )
install_library( ${cur_lib_name} ${PROJECT_NAME}-targets )


