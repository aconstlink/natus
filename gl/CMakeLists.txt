
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

if( SNAKEOIL_TARGET_OS_WIN )
    set( SOURCES ${sources} ${WGL_SOURCES} )
    add_definitions( -DWIN32_LEAN_AND_MEAN )
elseif( SNAKEOIL_TARGET_OS_LIN )
    set( SOURCES ${sources} ${GLX_SOURCES} )
endif()

natus_vs_src_dir( sources )

add_library( ${cur_lib_name} ${SNAKEOIL_LIBRARY_BUILD_TYPE} ${sources} )
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
