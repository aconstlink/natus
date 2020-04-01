
set( WIN32_SOURCES 
    application/win32/win32_application.h
    application/win32/win32_application.cpp
    window/win32/win32_window_handle.h
    window/win32/win32_window_handle.cpp
    window/win32/win32_window.h
    window/win32/win32_window.cpp
    window/win32/win32_info.h
    )

set( XLIB_SOURCES 
    application/xlib/xlib_application.h
    application/xlib/xlib_application.cpp
    window/xlib/xlib_window_handle.h
    window/xlib/xlib_window_handle.cpp
    window/xlib/xlib_window.h
    window/xlib/xlib_window.cpp
    window/xlib/xlib_info.h
    )

set( WGL_SOURCES
    window/wgl/wgl_context.h
    window/wgl/wgl_context.cpp
    window/wgl/wgl_window.h
    window/wgl/wgl_window.cpp
    window/wgl/wgl_info.h
)

set( GLX_SOURCES
    window/glx/glx_window.h
    window/glx/glx_window.cpp
    window/glx/glx_context.h
    window/glx/glx_context.cpp
)

set( sources 
    api.h
    protos.h
    result.h
    typedefs.h

    global.h
    global.cpp

    application/iapplication.h
    application/application.h
    application/application_info.h
    application/window_application.h
    application/window_application.cpp
        
    window/window_type.h
    window/irender_context.h
    window/gl_info.h
    window/gapi_info.h
    window/gl_window.h
    window/iwindow_handle.h
    window/window.h
    window/iwindow.h
    window/window_info.h
    window/window_message.h
    window/decorator_window.h
    window/decorator_window.cpp
    window/render_context_info.h
    window/iwindow_message_listener.h
    window/window_message_receiver.h
    window/window_message_receiver.cpp
    window/null/null_window.h
    window/null/null_window.cpp
    window/null/null_window_handle.h
    window/null/null_window_handle.cpp

    event/toggle_window.h
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
    #PUBLIC natus::gpx
    #PUBLIC natus::gpu
    PUBLIC natus::concurrent
    PUBLIC natus::log
    PUBLIC natus::memory
    
    #PUBLIC natus::device
    PUBLIC natus::io
) 

if( TARGET natus::ogl )
    target_link_libraries( ${cur_lib_name} PUBLIC natus::ogl )
endif()

###########################################################
# SECTION: Build Tree
###########################################################

#natus_export( ${CUR_LIB_NAME} )

###########################################################
# SECTION: Install
###########################################################

#install_headers( "${sources}" "include/${PROJECT_NAME}/${CUR_LIB_NAME_SHORT}" )
#install_library( ${CUR_LIB_NAME} )
