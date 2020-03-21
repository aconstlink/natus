
set( sources
    "api.h"
    "typedefs.h"
    "protos.h"
    "id.hpp"
    )

natus_vs_src_dir( sources )

add_library( ${cur_lib_name} ${NATUS_LIBRARY_BUILD_TYPE} ${sources} )
add_library( ${cur_alias_name} ALIAS ${cur_lib_name} )

target_link_libraries( ${cur_lib_name} 
    PUBLIC natus::log 
    PUBLIC natus::concurrent
    PUBLIC natus::io
    PUBLIC natus::memory
    PUBLIC natus::std
    PUBLIC natus::core 
    )

###########################################################
# SECTION: Build Tree
###########################################################

#natus_export( ${cur_lib_name} )

###########################################################
# SECTION: Install
###########################################################

#install_headers( "${SOURCES}" "include/${PROJECT_NAME}/${CUR_LIB_NAME_SHORT}" )
#install_library( ${cur_lib_name} )

