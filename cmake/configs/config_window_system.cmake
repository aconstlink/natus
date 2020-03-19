
set( NATUS_WINDOW_SYSTEM_CONFIGURED FALSE )

if( NOT NATUS_OS_CONFIGURED )
    message( FATAL_ERROR "Configured Target OS required." )
endif()

set( NATUS_WINDOW_SYSTEM_NULL ON )
set( NATUS_WINDOW_SYSTEM_WIN32 OFF )
set( NATUS_WINDOW_SYSTEM_XLIB OFF )
set( NATUS_WINDOW_SYSTEM_WAYLAND OFF )

if( NATUS_TARGET_OS_WIN )
    
    set( NATUS_TARGET_WINDOW_SYSTEM_API "win32" CACHE STRING 
    "Select the window system api to be compiled into the package." )

    set_property(CACHE NATUS_TARGET_WINDOW_SYSTEM_API PROPERTY STRINGS "null" "win32" )

    if( NATUS_TARGET_WINDOW_SYSTEM_API STREQUAL "null" )
        
        set( NATUS_WINDOW_SYSTEM_NULL ON )
        set( NATUS_WINDOW_SYSTEM_DEFINES 
          -DNATUS_WINDOW_SYSTEM_NULL )
    elseif( NATUS_TARGET_WINDOW_SYSTEM_API STREQUAL "win32" )
    
        set( NATUS_WINDOW_SYSTEM_NULL OFF )
        set( NATUS_WINDOW_SYSTEM_WIN32 ON )
        set( NATUS_WINDOW_SYSTEM_DEFINES 
          -DNATUS_WINDOW_SYSTEM_WIN32 )
    else()
        message( FATAL_ERROR "Invalid window system parameter" )
    endif()
    
    
    
elseif( NATUS_TARGET_OS_LIN )

    set( NATUS_TARGET_WINDOW_SYSTEM_API "null" CACHE STRING 
    "Select the window system api to be compiled into the package." )

    set_property(CACHE NATUS_TARGET_WINDOW_SYSTEM_API PROPERTY STRINGS 
        "null" "x11" "wayland" )

    if( NATUS_TARGET_WINDOW_SYSTEM_API STREQUAL "null" )
        
        set( NATUS_WINDOW_SYSTEM_NULL ON )
        set( NATUS_WINDOW_SYSTEM_DEFINES 
          -DNATUS_WINDOW_SYSTEM_WIN32 )
        message( "Null window system driver used" )

    elseif( NATUS_TARGET_WINDOW_SYSTEM_API STREQUAL "x11" )

        set( NATUS_WINDOW_SYSTEM_XLIB ON )
        set( NATUS_WINDOW_SYSTEM_DEFINES 
          -DNATUS_WINDOW_SYSTEM_XLIB )
        message( "X11 window system driver used" )
        message( FATAL_ERROR "Not supported at the moment" )
    
    elseif( NATUS_TARGET_WINDOW_SYSTEM_API STREQUAL "wayland" )
        
        set( NATUS_WINDOW_SYSTEM_WAYLAND ON )
        set( NATUS_WINDOW_SYSTEM_DEFINES 
          -DNATUS_WINDOW_SYSTEM_WAYLAND )
        message( "Wayland window system driver used" )
        message( FATAL_ERROR "Not supported at the moment" )

    else()
        message( FATAL_ERROR "Invalid window system parameter" )
    endif()

else()
    message( FATAL_ERROR "Window System Required" )
endif()


#
# Print Info
# 
if( NATUS_WINDOW_SYSTEM_WIN32 )
    message( STATUS "[Window System] : Win32    " )
elseif( NATUS_WINDOW_SYSTEM_XLIB )
    message( STATUS "[Window System] : XLib" )
elseif( NATUS_WINDOW_SYSTEM_WAYLAND )
    message( STATUS "[Window System] : Wayland" )
else()
    message( STATUS "[Window System] : Unknown" )
endif()

set( NATUS_WINDOW_SYSTEM_CONFIGURED TRUE )


