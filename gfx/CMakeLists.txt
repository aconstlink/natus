
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

    "imgui/imgui.h"
    "imgui/imgui.cpp"
    )

natus_vs_src_dir( sources )

add_library( ${cur_lib_name} ${NATUS_LIBRARY_BUILD_TYPE} ${sources} )
add_library( ${cur_alias_name} ALIAS ${cur_lib_name} ) 

target_link_libraries( ${cur_lib_name}
  PUBLIC imgui
  PUBLIC natus::memory
  PUBLIC natus::log 
  PUBLIC natus_graphics_options 
    )

###########################################################
# SECTION: Build Tree
###########################################################

##natus_export( ${cur_lib_name} )

###########################################################
# SECTION: Install
###########################################################

install_headers( "${sources}" "include/${PROJECT_NAME}/${cur_lib_name}" )
install_library( ${cur_lib_name} ${PROJECT_NAME}-targets )

