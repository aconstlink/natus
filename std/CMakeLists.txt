
set( sources 
    "typedefs.h"

    "vector.hpp"
    "list.hpp"
    "map.hpp"
    "stack.hpp"

    "filesystem.hpp"
    "string.hpp"

    "string/utf8.hpp"
    "string/split.hpp"
    )

natus_vs_src_dir( sources )

add_library( ${cur_lib_name} ${SNAKEOIL_LIBRARY_BUILD_TYPE} ${sources} )
add_library( ${cur_alias_name} ALIAS ${cur_lib_name} )

target_link_libraries( ${cur_lib_name}
        PUBLIC natus::log
        PUBLIC natus::memory
        PUBLIC ghc_filesystem )

###########################################################
# SECTION: Build Tree
###########################################################

#natus_export( ${to_be_included} )

###########################################################
# SECTION: Install
###########################################################

#install_headers( "${sources}" "include/${PROJECT_NAME}/${CUR_LIB_NAME_SHORT}" )


