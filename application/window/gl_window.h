#pragma once

#if defined( NATUS_WINDOW_SYSTEM_WIN32 ) 
#if defined( NATUS_TARGET_GRAPHICS_OPENGL )
#include "wgl/wgl_window.h"
#else
#include "null/null_window.h"
#endif
#elif defined( NATUS_WINDOW_SYSTEM_XLIB )
#include "glx/glx_window.h"
#else
#error "No Window system specified."
#endif

namespace natus::application
{
#if defined( NATUS_WINDOW_SYSTEM_WIN32 )
    
#if defined( NATUS_TARGET_GRAPHICS_OPENGL )
    natus_typedefs( natus::application::win32::wgl_window, gl_window ) ;
#else
    natus_typedefs( natus::application::null::null_window, gl_window ) ;
#endif

#elif defined( NATUS_WINDOW_SYSTEM_XLIB )
    natus_typedefs( natus::application::xlib::glx_window, gl_window ) ;
#endif
}