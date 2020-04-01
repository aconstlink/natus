
set( sources
    "api.h"
    "typedefs.h"
    "protos.h"
    "global.h"
    "global.cpp"
    )

natus_vs_src_dir( sources )

add_library( ${cur_lib_name} ${NATUS_LIBRARY_BUILD_TYPE} ${sources} )
add_library( ${cur_alias_name} ALIAS ${cur_lib_name} )

target_compile_definitions( ${cur_lib_name} 
    PRIVATE -DNATUS_OS_WINDOWS=${NATUS_TARGET_OS_WIN}) 

target_link_libraries( ${cur_lib_name} 
    PUBLIC natus::log 
    PUBLIC natus::concurrent
    PUBLIC natus::io
    PUBLIC natus::memory
    PUBLIC natus::std
    PUBLIC natus::core 
    PRIVATE natus_graphics_options 
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
