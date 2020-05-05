
set( WIN32_SOURCES 
    platform/win32/win32_application.h
    platform/win32/win32_application.cpp
    platform/win32/win32_window.h
    platform/win32/win32_window.cpp
    )

set( XLIB_SOURCES 
    platform/xlib/xlib_application.h
    platform/xlib/xlib_application.cpp
    platform/xlib/xlib_window.h
    platform/xlib/xlib_window.cpp
    )

set( WGL_SOURCES
    platform/wgl/wgl_window.h
    platform/wgl/wgl_window.cpp
    platform/wgl/wgl_context.h
    platform/wgl/wgl_context.cpp
)

set( GLX_SOURCES
    platform/glx/glx_window.h
    platform/glx/glx_window.cpp
    platform/glx/glx_context.h
    platform/glx/glx_context.cpp
)

set( sources 
    api.h
    protos.h
    result.h
    typedefs.h
    
    app.h
    app.cpp
    global.h
    global.cpp
    platform/platform_window.h
    platform/platform_application.h
    platform/platform_application.cpp
    platform/window_info.h   
    platform/window_message.h
    platform/window_message_receiver.hpp
    platform/gl_info.h
    platform/gfx_context.h
    )

if( NATUS_GRAPHICS_WGL )
    set( sources ${sources} ${WGL_SOURCES} )
elseif( NATUS_GRAPHICS_GLX )
    set( sources ${sources} ${GLX_SOURCES} )
endif()

if( NATUS_WINDOW_SYSTEM_WIN32 )
    set( sources ${sources} ${WIN32_SOURCES} )
    add_definitions( -DWIN32_LEAN_AND_MEAN )
elseif( NATUS_WINDOW_SYSTEM_XLIB )
    set( sources ${sources} ${XLIB_SOURCES} )
else()
    message( FATAL_ERROR "Window System: No Os specified." )
endif()

natus_vs_src_dir( sources )

add_library( ${cur_lib_name} ${NATUS_LIBRARY_BUILD_TYPE} ${sources} )
add_library( ${cur_alias_name} ALIAS ${cur_lib_name} )
target_compile_options( ${cur_lib_name} PUBLIC ${NATUS_WINDOW_SYSTEM_DEFINES} )

target_link_libraries( ${cur_lib_name}
    PUBLIC natus_graphics_options 
    PUBLIC natus::system
    #PUBLIC natus::gpx
   PUBLIC natus::gpu
    PUBLIC natus::concurrent
    PUBLIC natus::log
    PUBLIC natus::memory
    
    #PUBLIC natus::device
    PUBLIC natus::io
) 

if( TARGET natus::ogl )
    target_link_libraries( ${cur_lib_name} 
      PUBLIC gl_headers 
      PUBLIC natus::ogl )
endif()

###########################################################
# SECTION: Build Tree
###########################################################

#natus_export( ${CUR_LIB_NAME} )

###########################################################
# SECTION: Install
###########################################################

install_headers( "${sources}" "include/${PROJECT_NAME}/${cur_lib_name}" )
install_library( ${cur_lib_name} ${PROJECT_NAME}-targets )
