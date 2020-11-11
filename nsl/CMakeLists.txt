

set( sources 
    "api.h"
    "typedefs.h"
    "header_only.cpp"
    "enums.hpp"

    "database.hpp"
    "monitor.hpp"
    "parser.hpp"
    "ast.hpp"
    "typedefs.h"
    "symbol_table.hpp"
    "generator.hpp"
    "generator_structs.hpp"
    "symbol.hpp"
    "dependency_resolver.hpp"

    "api/glsl/generator.hpp"
    "api/glsl/function_signature_analyser.hpp"
    "api/hlsl/function_signature_analyser.hpp"
    "api/hlsl/generator.hpp"

    "parser_structs.hpp"
    "ast/nodes.hpp"
    "ast/interfaces.hpp"
    "ast/components.hpp"
    "ast/visitors/visitor.hpp"
    "ast/visitors/gl3_generator.hpp"
    "ast/visitors/es3_generator.hpp"
    "ast/visitors/version_generator.hpp"
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
