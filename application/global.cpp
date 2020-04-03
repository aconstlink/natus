#include "global.h"

#include <natus/system/global.h>

#ifdef NATUS_GRAPHICS_WGL
#include "platform/win32/win32_window.h"
#include "platform/wgl/wgl_window.h"
#endif

#ifdef NATUS_GRAPHICS_GLX
#include "window/glx/glx_window.h"
#include "window/xlib/xlib_window.h"
#endif

#ifdef NATUS_GRAPHICS_EGL
#include "window/egl/egl_window.h"
#include "window/xlib/xlib_window.h"
#endif

using namespace natus::application ;

//****************************************************
iwindow_rptr_t global::create_window( void_t ) noexcept
{

    return iwindow_rptr_t() ;
}

//****************************************************
iwindow_rptr_t global::create_render_window( void_t ) noexcept
{
#ifdef NATUS_GRAPHICS_WGL
    /*natus::application::gl_info_t gli ; 
    natus::application::window_info_t wi ;
    
    return natus::application::win32::wgl_window_t::res( 
        natus::application::win32::wgl_window_t( gli, wi ) ) ;*/

    
    
#endif
    return iwindow_rptr_t() ;
}