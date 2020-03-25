
set( sources
    "api.h"
    "typedefs.h"
    "protos.h"
    "res.hpp"
    "soil.cpp"
    "rptr.hpp"
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

install_headers( "${sources}" "include/${PROJECT_NAME}/${cur_lib_name}" )
install( TARGETS ${cur_lib_name} 
        RUNTIME DESTINATION bin 
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        )
install( TARGETS ${cur_lib_name} EXPORT ${PROJECT_NAME}-targets )  