
set( sources 
    "api.h"
    "typedefs.h"

    "global.h"
    "global.cpp"
    "module_factory.hpp"
    "module_registry.hpp"

    "imodule.h"
    
    "obj/obj_module.cpp"
    "obj/obj_module.h"

    "stb/stb_module.cpp"
    "stb/stb_module.h"

    "wav/wav_module.cpp"
    "wav/wav_module.h"

    )

natus_vs_src_dir( sources )

add_library( ${cur_lib_name} ${NATUS_LIBRARY_BUILD_TYPE} ${sources} )
add_library( ${cur_alias_name} ALIAS ${cur_lib_name} ) 

target_link_libraries( ${cur_lib_name}
  PRIVATE stb
  PUBLIC natus::graphics
  PUBLIC natus::io
    )

if( NATUS_COMPILER_GNU )
  target_compile_options( ${cur_lib_name} PUBLIC -Wno-pessimizing-move )
endif()

###########################################################
# SECTION: Build Tree
###########################################################

##natus_export( ${cur_lib_name} )

###########################################################
# SECTION: Install
###########################################################

install_headers( "${sources}" "include/${PROJECT_NAME}/${cur_lib_name}" )
install_library( ${cur_lib_name} ${PROJECT_NAME}-targets )

