
#include "global.h"

#if defined( NATUS_GRAPHICS_WGL )

#include <natus/application/platform/wgl/wgl_window.h>
#include <natus/application/platform/win32/win32_application.h>

#elif defined( NATUS_GRAPHICS_GLX )

#include <natus/application/platform/glx/glx_window.h>
#include <natus/application/platform/xlib/xlib_application.h>

#elif defined( NATUS_GRAPHICS_EGL )

#endif

using namespace natus ;
using namespace natus::application ;

//*********************************************************
platform_application_res_t global::create_application( void_t ) 
{
    #if defined( NATUS_GRAPHICS_WGL )

    return natus::application::win32::application_res_t(
        natus::application::win32::win32_application_t() ) ;
    
    #elif defined( NATUS_GRAPHICS_GLX )
    
    return natus::application::xlib::application_res_t(
        natus::application::xlib::xlib_application_t() ) ;

    #elif defined( NATUS_GRAPHICS_EGL )

    return natus::application::xlib::application_res_t(
        natus::application::xlib::xlib_application_t() ) ;

    #endif
    
    return platform_application_res_t() ;
}

//*********************************************************
platform_application_res_t global::create_application( natus::application::app_res_t app ) 
{
    #if defined( NATUS_GRAPHICS_WGL )

    return natus::application::win32::application_res_t(
        natus::application::win32::win32_application_t( app ) ) ;

    #elif defined( NATUS_GRAPHICS_GLX )

    return natus::application::xlib::application_res_t(
        natus::application::xlib::xlib_application_t( app ) ) ;

    #elif defined( NATUS_GRAPHICS_EGL )

    return natus::application::xlib::application_res_t(
        natus::application::xlib::xlib_application_t( app ) ) ;

    #endif

    return platform_application_res_t() ;
}
