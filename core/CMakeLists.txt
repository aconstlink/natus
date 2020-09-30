

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
    "uint8_split_merge.hpp"

    "macros/common.h"
    "macros/typedef.h"
    "macros/function.h"
    "macros/move.h"

    "header_only.cpp"
    )

natus_vs_src_dir( sources )


add_library( ${cur_lib_name} ${NATUS_LIBRARY_BUILD_TYPE} ${sources} )
add_library( ${cur_alias_name} ALIAS ${cur_lib_name} )

target_compile_definitions( ${cur_lib_name} PUBLIC -DNATUS_LITTLE_ENDIAN=0 )
target_compile_definitions( ${cur_lib_name} PUBLIC -DNATUS_BIG_ENDIAN=1 )
target_compile_definitions( ${cur_lib_name} PUBLIC -DNATUS_BYTE_ORDER=${NATUS_TARGET_ARCH_BIG_ENDIAN} )

###########################################################
# SECTION: Build Tree
###########################################################

#natus_export( ${CUR_LIB_NAME} )

###########################################################
# SECTION: Install
###########################################################

install_headers( "${sources}" "include/${PROJECT_NAME}/${cur_lib_name}" )
install_library( ${cur_lib_name} ${PROJECT_NAME}-targets )
