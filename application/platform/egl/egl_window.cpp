
#include "egl_window.h"
#include "egl_context.h"
#include "../xlib/xlib_application.h"

#include <natus/ogl/egl/egl.h>
#include <natus/log/global.h>
#include <GLES3/gl3.h>

using namespace natus::application ;
using namespace natus::application::egl ;

//***********************************************************************
window::window( void_t ) 
{}

//***********************************************************************
window::window( gl_info_cref_t gli, window_info_cref_t wi ) 
{
    EGLNativeWindowType wnd = this_t::create_egl_window( wi ) ;

    _window = natus::application::xlib::window_res_t(
                natus::application::xlib::window_t(
                  natus::application::xlib::xlib_application_t::get_display(),
                  wnd ) ) ;

    _context = egl::context_t( gli, wnd, 
                 natus::application::xlib::xlib_application_t::get_display()  ) ;

    _vsync = gli.vsync_enabled ;
    
    if( wi.show )
    {
        //XMapWindow( _window->get_display(), glx_wnd );
        XMapRaised( _window->get_display(), wnd ) ;
        XFlush( _window->get_display() ) ;
    }

    XSync( _window->get_display(), False ) ;

    // give it a test
    {
        _context->activate() ;
        glClearColor( 0,0.5,0.0,1.0);
        glClear( GL_COLOR_BUFFER_BIT ) ;
        //eglSwapBuffers( _window->get_display(), /*surface here*/ ) ;
        _context->clear_now( natus::math::vec4f_t(0.0f,0.0f,0.0f,1.0f) ) ;
        _context->swap() ;
        _context->clear_now( natus::math::vec4f_t(0.5f,0.5f,1.0f,1.0f) ) ;
        _context->swap() ;
        _context->deactivate() ;
    }
}

//***********************************************************************
window::window( this_rref_t rhv ) : platform_window( ::std::move( rhv ) )
{
    _window = ::std::move( rhv._window ) ;
    _context = ::std::move( rhv._context ) ;

    _vsync = rhv._vsync ;
}

//***********************************************************************
window::~window( void_t ) 
{
    _window = xlib::window_res_t() ;
    _context = egl::context_res_t() ;
}

//***********************************************************************
xlib::window_res_t window::create_window( gl_info_in_t, window_info_in_t ) 
{
}

//***********************************************************************
EGLNativeWindowType window::create_egl_window( window_info_in_t wi ) 
{
    auto const status = XInitThreads() ;
    natus::log::global_t::warning( status == 0, 
           natus_log_fn("XInitThreads") ) ;

    Display * display = natus::application::xlib::xlib_application_t::get_display() ;
    Window root = DefaultRootWindow( display ) ;

    XSetWindowAttributes swa ;
    swa.event_mask = ExposureMask | PointerMotionMask | 
        KeyPressMask | StructureNotifyMask ;

    Window window = XCreateWindow( display, 
            root, wi.x, wi.y,
            wi.w,//WidthOfScreen( DefaultScreenOfDisplay( display ) ),
            wi.h,//HeightOfScreen( DefaultScreenOfDisplay( display ) ),
            0, CopyFromParent, InputOutput, CopyFromParent, 
            CWEventMask, &swa ) ;

    if( natus::log::global_t::error( !window, 
            "[glx_window::create_glx_window] : XCreateWindow" ) )
    {
        return 0 ;
    }

    {
        EGLNativeDisplayType ndt = (EGLNativeDisplayType)display ;
        auto const res = eglInitialize( display , NULL, NULL ) ;
        if( res != EGL_TRUE )
        {
            natus::log::global_t::error( natus_log_fn("eglInitialize") ) ;
            return 0 ;
        }
    }
    
    XStoreName( display, window, wi.window_name.c_str() ) ;

    return (EGLNativeWindowType)window ;
}
