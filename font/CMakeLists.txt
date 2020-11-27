
set( sources 
    "api.h"
    "result.h"
    "typedefs.h"
    "structs.h"

    "glyph_atlas.h"
    "glyph_atlas.cpp"
    "stb/stb_glyph_atlas_creator.h"
    "stb/stb_glyph_atlas_creator.cpp"
    )

natus_vs_src_dir( sources )

add_library( ${cur_lib_name} ${NATUS_LIBRARY_BUILD_TYPE} ${sources} )
add_library( ${cur_alias_name} ALIAS ${cur_lib_name} ) 

target_link_libraries( ${cur_lib_name}
  PRIVATE stb
  PUBLIC imgui
  PUBLIC natus::device
  PUBLIC natus::graphics
  PUBLIC natus::memory
  PUBLIC natus::log 
  PUBLIC natus::io
  PUBLIC natus_graphics_options 
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

