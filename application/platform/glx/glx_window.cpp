
#include "glx_window.h"
#include "glx_context.h"
#include "../xlib/xlib_application.h"

#include <natus/log/global.h>

using namespace natus::application ;
using namespace natus::application::glx ;

//***********************************************************************
window::window( void_t ) 
{}

//***********************************************************************
window::window( gl_info_cref_t gli, window_info_cref_t wi ) 
{
    Window glx_wnd = this_t::create_glx_window( wi ) ;

    _window = natus::application::xlib::window_res_t(
                natus::application::xlib::window_t(
                  natus::application::xlib::xlib_application_t::get_display(),
                  glx_wnd ) ) ;

    _context = glx::context_t( gli, glx_wnd, 
                 natus::application::xlib::xlib_application_t::get_display()  ) ;

    _vsync = gli.vsync_enabled ;

    if( wi.show )
    {
        XMapWindow( _window->get_display(), glx_wnd );
        XFlush( _window->get_display() ) ;
    }

    XSync( _window->get_display(), False ) ;

    // give it a test
    {
        _context->activate() ;
        glClearColor( 0,0.5,0.0,1.0);
        glClear( GL_COLOR_BUFFER_BIT ) ;
        glXSwapBuffers( _window->get_display(), glx_wnd ) ;
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
    _context = glx::context_res_t() ;
}

//***********************************************************************
xlib::window_res_t window::create_window( gl_info_in_t, window_info_in_t ) 
{

}

//***********************************************************************
GLXFBConfig window::get_config( void_t ) 
{
    static GLXFBConfig *fbc = nullptr ;
    if( fbc != nullptr )
    {
        return fbc[0] ;
    }

    int_ptr_t visual_attribs = natus::memory::global_t::alloc_raw<int_t>( 24, 
            "[glx_window::create_glx_window] : visual_attribs" ) ;

    {
        struct va_pair{
            int_t flag ;
            int_t value ;
        };

        va_pair * va_pairs = (va_pair*)visual_attribs ;
        va_pairs[0] = {GLX_X_RENDERABLE, True} ;
        va_pairs[1] = {GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT} ;
        va_pairs[2] = {GLX_RENDER_TYPE, GLX_RGBA_BIT} ;
        va_pairs[3] = {GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR} ;
        va_pairs[4] = {GLX_RED_SIZE, 8} ;
        va_pairs[5] = {GLX_GREEN_SIZE, 8} ;
        va_pairs[6] = {GLX_BLUE_SIZE, 8} ;
        va_pairs[7] = {GLX_ALPHA_SIZE, 8} ;
        va_pairs[8] = {GLX_DEPTH_SIZE, 24} ;
        va_pairs[9] = {GLX_STENCIL_SIZE, 8} ;
        va_pairs[10] = {GLX_DOUBLEBUFFER, True} ;
        va_pairs[11] = {None, None} ;
    }

    Display * display = natus::application::xlib::xlib_application_t::get_display() ;

    int fbcount ;
    fbc = glXChooseFBConfig( 
            display, DefaultScreen( display ),
            visual_attribs, &fbcount ) ;

    if( fbc == nullptr || fbcount == 0 ) 
    {
        natus::log::global_t::error( 
             "[glx_window::create_glx_window] : glXChooseFBConfig" ) ;

        return 0 ;
    }

    GLXFBConfig fbconfig = fbc[0] ;

    //XFree( fbc ) ;
    //natus::memory::global_t::dealloc( visual_attribs ) ;

    return fbconfig ;
}

//***********************************************************************
Window window::create_glx_window( window_info_in_t wi ) 
{
    auto const status = XInitThreads() ;
    natus::log::global_t::warning( status == 0, 
           natus_log_fn("XInitThreads") ) ;

    Display * display = natus::application::xlib::xlib_application_t::get_display() ;
    GLXFBConfig fbconfig = this_t::get_config() ;
    XVisualInfo * vi = glXGetVisualFromFBConfig( display, fbconfig ) ;

    Colormap cm ;
    XSetWindowAttributes swa ;

    swa.colormap = cm = XCreateColormap( display, 
       RootWindow( display, vi->screen ), vi->visual, AllocNone ) ;
    swa.background_pixmap = None ;
    swa.border_pixel = 0 ;
    swa.event_mask = ExposureMask | StructureNotifyMask | ResizeRedirectMask |
        KeyPressMask ;

    Window window = XCreateWindow( display, 
            RootWindow( display, vi->screen ), 0, 0,
            100,//WidthOfScreen( DefaultScreenOfDisplay( display ) ),
            100,//HeightOfScreen( DefaultScreenOfDisplay( display ) ),
            0, vi->depth, InputOutput, vi->visual, 
            CWBorderPixel | CWColormap | CWEventMask, &swa ) ;

    if( natus::log::global_t::error( !window, 
            "[glx_window::create_glx_window] : XCreateWindow" ) )
    {
        return 0 ;
    }

    //XFree( vi ) ;

    XStoreName( display, window, wi.window_name.c_str() ) ;

    return window ;
}
