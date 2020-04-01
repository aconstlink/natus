
set( gl_sources
    gl/gl.h
    gl/gl.cpp
)

set( wgl_sources
    wgl/wgl.h
    wgl/wgl.cpp
)

set( glx_sources
    glx/glx.h
    glx/glx.cpp
)

set( sources 
    api.h
    result.h
    typedefs.h
    protos.h

    ${gl_sources}
)

if( NATUS_TARGET_OS_WIN )
    set( sources ${sources} ${wgl_sources} )
    add_definitions( -DWIN32_LEAN_AND_MEAN )
elseif( NATUS_TARGET_OS_LIN )
    set( sources ${sources} ${glx_sources} )
endif()

natus_vs_src_dir( sources )

add_library( ${cur_lib_name} ${NATUS_LIBRARY_BUILD_TYPE} ${sources} )
add_library( ${cur_alias_name} ALIAS ${cur_lib_name} )

target_link_libraries( ${cur_lib_name}
  PUBLIC OpenGL::GL 
  PUBLIC gl_headers
  PUBLIC natus::std
  PUBLIC natus::log
  PUBLIC natus::memory
)

###########################################################
# SECTION: Build Tree
###########################################################

#so_export( ${cur_lib_name} )

###########################################################
# SECTION: Install
###########################################################

#install_headers( "${SOURCES}" "include/${PROJECT_NAME}/${CUR_LIB_NAME_SHORT}" )
#install_library( ${cur_lib_name} )
