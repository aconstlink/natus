
set( sources 
    "api.h"
    "result.h"
    "protos.h"
    "typedefs.h"

    "camera/icamera.h"
    "camera/ilens.h"
    "camera/pinhole_camera.h"
    "camera/generic_camera.h"
    "camera/generic_camera.cpp"
    "camera/pinhole_lens.h"

    "font/text_render_2d.h"
    "font/text_render_2d.cpp"

    "primitive/line_render_2d.h"
    "primitive/line_render_2d.cpp"
    "primitive/tri_render_2d.h"
    "primitive/tri_render_2d.cpp"
    
    )

natus_vs_src_dir( sources )

add_library( ${cur_lib_name} ${NATUS_LIBRARY_BUILD_TYPE} ${sources} )
add_library( ${cur_alias_name} ALIAS ${cur_lib_name} ) 

target_link_libraries( ${cur_lib_name}
  PRIVATE stb
  PUBLIC imgui
  PUBLIC natus::font
  PUBLIC natus::device
  PUBLIC natus::graphics
  PUBLIC natus::memory
  PUBLIC natus::log 
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

