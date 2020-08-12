
set( sources
    "api.h"
    "typedefs.h"
    "protos.h"
    
    "handle.hpp"
    "allocator.hpp"
    "global.h"
    "global.cpp"
    "res.hpp"
    "macros.h"

    "macros/handle.h"
    
    "utility/uint8_split_merge.h"
    "utility/uint8_msb_stream.h"

    "manager/imanager.h"
    "manager/manager.h"
    "manager/manager.cpp"
    
    "guards/malloc_guard.hpp"
    
    "arena/exp_arena.hpp"
    )

natus_vs_src_dir( sources )

add_library( ${cur_lib_name} ${NATUS_LIBRARY_BUILD_TYPE} ${sources} )
add_library( ${cur_alias_name} ALIAS ${cur_lib_name} )

target_link_libraries( ${cur_lib_name} 
    PUBLIC natus::log 
    )

###########################################################
# SECTION: Build Tree
###########################################################

#natus_export( ${cur_lib_name} )

###########################################################
# SECTION: Install
###########################################################

install_headers( "${sources}" "include/${PROJECT_NAME}/${cur_lib_name}" )
install_library( ${cur_lib_name} ${PROJECT_NAME}-targets )

