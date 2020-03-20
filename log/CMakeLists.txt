
set( sources 
    "api.h"
    "result.h"
    "protos.h"
    "typedefs.h"
    
    "global.h"
    "global.cpp"
    "log_level.h"

    "system/isystem.h"
    "system/system.h"
    "system/system.cpp"
    
    "logger/logger.h"
    "logger/std_cerr_logger.h"
    "logger/std_cerr_logger.cpp"
    "logger/store_logger.h"
    "logger/store_logger.cpp"
    )

natus_vs_src_dir( sources )

add_library( ${cur_lib_name} ${NATUS_LIBRARY_BUILD_TYPE} ${sources} )
add_library( ${cur_alias_name} ALIAS ${cur_lib_name} ) 

###########################################################
# SECTION: Build Tree
###########################################################

#natus_export( ${cur_lib_name} )

###########################################################
# SECTION: Install
###########################################################

#install_headers( "${sources}" "include/${PROJECT_NAME}/${cur_lib_name_short}" )
#install_library( ${cur_lib_name_short} )


