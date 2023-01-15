

set( sources 
    "api.h"
    "typedefs.h"
    "header_only.cpp"
    "enums.hpp"

    "database.hpp"
    "monitor.hpp"
    "parser.h"
    "parser.cpp"
    "typedefs.h"
    "symbol_table.hpp"
    "generator.h"
    "generator.cpp"
    "generator_structs.hpp"
    "symbol.hpp"
    "dependency_resolver.hpp"
    "function_declaration_analyser.hpp"
    "function_signature_analyser.hpp"
    "nodes.h"
    "nodes.cpp"

    "api/glsl/generator.h"
    "api/glsl/generator.cpp"
    "api/hlsl/generator.h"
    "api/hlsl/generator.cpp"

    "parser_structs.hpp"
    )

natus_vs_src_dir( sources )

add_library( ${cur_lib_name} ${NATUS_LIBRARY_BUILD_TYPE} ${sources} )
add_library( ${cur_alias_name} ALIAS ${cur_lib_name} )


target_link_libraries( ${cur_lib_name}
        PUBLIC natus::concurrent
        PUBLIC natus::log
        PUBLIC natus::memory
        PUBLIC natus::ntd
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
