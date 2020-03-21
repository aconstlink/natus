

set( sources 
    "assert.h"
    "types.hpp"
    "boolean.h"
    "checks.h"    
    "break_if.hpp"
    "execute_if.hpp"
    "cast.hpp"
    "execute_result.h"
    "typedefs.hpp"

    "macros/common.h"
    "macros/typedef.h"
    "macros/function.h"
    "macros/move.h"

    "core.cpp"
    )

natus_vs_src_dir( sources )

add_library( ${cur_lib_name} ${NATUS_LIBRARY_BUILD_TYPE} ${sources} )
add_library( ${cur_alias_name} ALIAS ${cur_lib_name} )

target_compile_definitions( ${cur_lib_name} 
    PUBLIC 
        $<$<CONFIG:Debug>:NATUS_DEBUG>
        $<$<CONFIG:MinSizeRel>:NATUS_RELEASE>
        $<$<CONFIG:Release>:NATUS_RELEASE>
        $<$<CONFIG:RelWithDebInfo>:NATUS_RELEASE>
    )

###########################################################
# SECTION: Build Tree
###########################################################

#natus_export( ${CUR_LIB_NAME} )

###########################################################
# SECTION: Install
###########################################################

#install_headers( "${SOURCES}" "include/${PROJECT_NAME}/${CUR_LIB_NAME_SHORT}" )
