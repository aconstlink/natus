

set( sources 
    "api.h"
    "result.h"
    "protos.h"
    "typedefs.h"
    "global.h"
    "global.cpp"

    "handle.h"
    "handle.cpp"
   
    "system.h"
    "system.cpp"
    )

natus_vs_src_dir( sources )

add_library( ${cur_lib_name} ${NATUS_LIBRARY_BUILD_TYPE} ${sources} )
add_library( ${cur_alias_name} ALIAS ${cur_lib_name} )

target_link_libraries( ${cur_lib_name}
        PUBLIC natus::concurrent
        PUBLIC natus::log
        PUBLIC natus::memory
        PUBLIC natus::std
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