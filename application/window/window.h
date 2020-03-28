#pragma once

#if defined( NATUS_WINDOW_SYSTEM_WIN32 )
#include "win32/win32_window.h"
#elif defined( NATUS_WINDOW_SYSTEM_XLIB )
#include "xlib/xlib_window.h"
#else
#error "No window system defined"
#endif

namespace natus::application
{
#if defined(NATUS_WINDOW_SYSTEM_WIN32)
    natus_typedefs( natus::application::win32::win32_window, window) ;
#elif defined(NATUS_WINDOW_SYSTEM_XLIB)
    natus_typedefs( natus::application::xlib::xlib_window, window) ;
#endif    
}
