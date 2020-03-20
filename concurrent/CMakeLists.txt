

set( sources
    "api.h"
    "result.h"
    "typedefs.h"
    "protos.h"
    "global.h"
    "global.cpp"

    "primitive/sync_object.hpp"
    "helper.h"
    "helper/helper_os.h"

    "mrsw.hpp"
    "semaphore.hpp"
    "semaphore_guard.hpp"
    "mutex.hpp"
    "scheduler.h"
    "scheduler.cpp"

    "container/vector.hpp"
    "container/double_buffered/item.hpp"
    "container/double_buffered/map.hpp"
    "container/double_buffered/vector.hpp"

    "task/itask.h"
    "task/task.h"
    "task/task.cpp"
    "task/task_graph.h"
    "task/task_graph.cpp"
    "task/tasks.h"
    "task/tasks.cpp"
    "task/serial_executor.h"
    "task/serial_executor.cpp"
    "task/async_executor.h"
    "task/async_executor.cpp"
    "task/itask_scheduler.h"
    "task/task_scheduler.h"
    "task/task_scheduler.cpp"
        
    "command_system/command.h"
    "command_system/command_list.h"
    "command_system/command_list.cpp"
    "command_system/command_system.h"
    "command_system/command_system.cpp"
    "command_system/command_system_state.h"

    "parallel_for.hpp"
    "range/range_1d.hpp"
    
    "job/job.h"
    "job/job_funk.h"
    "job/job_scheduler.h"
    "job/job_scheduler.cpp"
    )

set( unix_sources
    "helper/unix/pthread_helper.h"
    )

set( win32_sources 
    "helper/win32/win32_helper.h"
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

###########################################################
# SECTION: Build Tree
###########################################################

#natus_export( ${cur_lib_name} )

###########################################################
# SECTION: Install
###########################################################

#install_headers( "${sources}" "include/${PROJECT_NAME}/${CUR_LIB_NAME_SHORT}" )
#install_library( ${cur_lib_name} )


