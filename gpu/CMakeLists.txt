
set( sources 
    "api.h"
    "result.h"
    "protos.h"
    "typedefs.h"

    "global.h"
    "global.cpp"

    "backend/id.hpp"
    "backend/types.h"
    "backend/backend.h"
    "backend/backend.cpp"
    "async.h"
    "async.cpp"
    "async_id.hpp"
    
    "backend/gl3/gl3.h"
    "backend/gl3/gl3.cpp"
    "backend/es3/es3.h"
    "backend/es3/es3.cpp"
    "backend/null/null.h"
    "backend/null/null.cpp"
   
    "shader/vertex_shader.hpp"
    "shader/geometry_shader.hpp"
    "shader/pixel_shader.hpp"

    "configuration/render_configuration.h"

   
    )

natus_vs_src_dir( sources )

add_library( ${cur_lib_name} ${NATUS_LIBRARY_BUILD_TYPE} ${sources} )
add_library( ${cur_alias_name} ALIAS ${cur_lib_name} ) 

target_link_libraries( ${cur_lib_name} 
  PUBLIC natus::memory
  PUBLIC natus::log 
  PUBLIC natus_graphics_options 
    )

if( TARGET natus::ogl )
    target_link_libraries( ${cur_lib_name} 
      PUBLIC gl_headers 
      PUBLIC natus::ogl )
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

