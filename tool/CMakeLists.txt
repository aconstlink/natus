
set( sources 
    "api.h"
    "result.h"
    "protos.h"
    "typedefs.h"
    "structs.h"

    "imgui/imgui.h"
    "imgui/imgui.cpp"
    "imgui/sprite_editor.h"
    "imgui/sprite_editor.cpp"
    "imgui/custom_widgets.h"
    "imgui/custom_widgets.cpp"
    "imgui/imgui_custom.h"
    "imgui/imgui_custom.cpp"

    "imgui/timeline.h"
    "imgui/timeline.cpp"
    "imgui/keyframe_editor.h"
    "imgui/keyframe_editor.cpp"
    "imgui/player_controller.h"
    "imgui/player_controller.cpp"
    )

natus_vs_src_dir( sources )

add_library( ${cur_lib_name} ${NATUS_LIBRARY_BUILD_TYPE} ${sources} )
add_library( ${cur_alias_name} ALIAS ${cur_lib_name} ) 

target_link_libraries( ${cur_lib_name}
  PRIVATE stb
  PUBLIC imgui
  PUBLIC natus::gfx
  PUBLIC natus::format
  PUBLIC natus::io
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

