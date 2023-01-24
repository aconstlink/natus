
#include "global.h"

#include <natus/device/global.h>
#include <natus/concurrent/global.h>
#include <natus/format/global.h>
#include <natus/audio/global.h>
#include <natus/io/global.h>

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
platform_application_res_t global::create_application( void_t )  noexcept
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
platform_application_res_t global::create_application( natus::application::app_res_t app )  noexcept
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

//*********************************************************
int_t global::create_and_exec_application( natus::application::app_res_t app ) noexcept
{
    auto r = this_t::create_application( std::move( app ) )->exec() ;

    natus::format::global_t::deinit() ;
    natus::device::global_t::deinit() ;
    natus::concurrent::global_t::deinit() ;
    natus::audio::global_t::deinit() ;
    natus::io::global_t::deinit() ;

    natus::memory::global_t::dump_to_std() ;

    return r ;
}