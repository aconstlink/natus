//------------------------------------------------------------
// snakeoil (c) Alexis Constantin Link
// Distributed under the MIT license
//------------------------------------------------------------
#include "glx_window.h"

#include "../xlib/xlib_window.h"
#include "../../application/xlib/xlib_application.h"

#include "../../log.h"

#include <snakeoil/gli/glx/glx.h>
#include <snakeoil/gli/gl/gl.h>
#include <snakeoil/gpu/driver/gl/gl_driver.h>


using namespace so_app ;
using namespace so_app::so_xlib ;

//***********************************************************************
glx_window::glx_window( gl_info_cref_t gli, window_info_cref_t wi ) 
{
    Display * display = so_xlib::xlib_application::get_display() ;
    
    // we need to create the xlib Window id here, because
    // the glx window requires special window parameters.
    Window wnd = this_t::create_glx_window( display, gli, wi ) ;
    
    // in the last step, the xlib window id was created.
    // now create the window itself.
    xlib_window_ptr_t wnd_ptr = xlib_window::create( 
            xlib_window( display, wnd ),
            "[glx_window::glx_window] : The render window." ) ;

    this_t::set_decorated( wnd_ptr ) ;
    
    if( wi.show )
    {
        XMapWindow( display, wnd );
        XFlush( display ) ;
    }
    
    if( so_app::log::error( wnd_ptr == nullptr, 
            "[glx_window::glx_window] : unable to create xlib window." ) )
    {        
        throw 1 ;
    }

    XSync( display, False );
    
    {
        _context_ptr->activate() ;
        so_gli::gl::init() ;
        _context_ptr->deactivate() ;
    }
    
    // create and initialize the gl driver
    {
        _driver_ptr = so_gpu::so_driver::so_gl::gl_driver::create( 
            so_gpu::so_driver::so_gl::gl_driver(_context_ptr), "[glx_window::glx_window] : gl_driver" ) ;
        
        _context_ptr->activate() ;
        _driver_ptr->initialize() ;
        _context_ptr->deactivate() ;
    }

    // test the GL out by just clearing the 
    // window's color buffer.
    {
        _context_ptr->activate() ;
        _context_ptr->clear_now( so_math::vec4f_t(1.0f,0.5f,0.25f,1.0f) ) ;
        _context_ptr->swap() ;
        _context_ptr->deactivate() ;
    }
    
    // could be delivered via gl_info
    size_t const num_supports = 2 ;

    // create support drivers and contexts
    for( size_t i=0; i<num_supports; ++i )
    {
        so_app::so_xlib::glx_context_ptr_t sctx = _context_ptr->create_shared() ;
        if( sctx != nullptr ) _support_contexts.push_back( sctx ) ;

        auto * sdrv = so_gpu::so_driver::so_gl::gl_driver::create( 
            so_gpu::so_driver::so_gl::gl_driver(sctx), "[glx_window::glx_window] : support gl_driver" ) ;
        if( sdrv != nullptr ) _support_drivers.push_back( sdrv ) ;
    }

    base_t::init_and_start_render_threads(num_supports) ;
}

//***********************************************************************
glx_window::glx_window( this_rref_t rhv ) : render_window( std::move(rhv) )
{
    _context_ptr = rhv._context_ptr ;
    rhv._context_ptr = nullptr ;
    _driver_ptr = rhv._driver_ptr ;
    rhv._driver_ptr = nullptr ;

    _support_contexts = std::move( rhv._support_contexts ) ;
    _support_drivers = std::move( rhv._support_drivers ) ;
}

//***********************************************************************
glx_window::~glx_window( void_t ) 
{
    /// since this starts the rendering thread, it
    /// is also stopping it. @see init_and_start_render_threads()
    base_t::stop_rendering_and_join() ;

    for( size_t i=0; i<_support_contexts.size(); ++i )
    {
        so_gpu::so_driver::so_gl::gl_driver::destroy( _support_drivers[i] ) ;
        _context_ptr->destroy_shared( _support_contexts[i] ) ;
    }

    so_gpu::so_driver::so_gl::gl_driver::destroy( _driver_ptr ) ;
    so_app::memory::dealloc( _context_ptr ) ;

    // if this has been moved, the decorated is moved with it.
    // if this still has the decorated, it needs to be destroyed here
    // because it is created in this ctor.
    if( this_t::has_decorated() )
    {
        xlib_window::destroy( (xlib_window_ptr_t)this_t::get_decorated() ) ;
        this_t::set_decorated( nullptr ) ;
    }
}

//***********************************************************************
glx_window::this_ptr_t glx_window::create( this_rref_t rhv ) 
{
    return so_app::memory::alloc( std::move(rhv), "[gl_window::create] : create" ) ;
}

//***********************************************************************
void_t glx_window::destroy( this_ptr_t ptr ) 
{
    so_app::memory::dealloc( ptr ) ;
}

//***********************************************************************
so_app::result glx_window::destroy( void_t )
{
    this_t::destroy( this ) ;

    return so_app::ok ;
}

//***********************************************************************
irender_context_ptr_t glx_window::get_render_context( void_t )
{
    return _context_ptr ;
}

//***********************************************************************
render_window::driver_ptr_t glx_window::get_driver( void_t ) 
{
    return _driver_ptr ;
}

//***********************************************************************
irender_context_ptr_t glx_window::get_support_context( size_t i ) 
{
    so_assert( i<_support_contexts.size() ) ;
    return _support_contexts[i] ;
}

//***********************************************************************
render_window::driver_ptr_t glx_window::get_support_driver( size_t i ) 
{
    so_assert( i<_support_drivers.size() ) ;
    return _support_drivers[i] ;
}

//***********************************************************************
Window glx_window::create_glx_window( Display * display, gl_info_cref_t gli, window_info_cref_t wi ) 
{    
    int_ptr_t visual_attribs = so_app::memory::alloc_raw<int_t>( 24, 
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
            
    int fbcount ;
    GLXFBConfig *fbc = glXChooseFBConfig( 
            display, DefaultScreen( display ),
            visual_attribs, &fbcount ) ;

    if( fbc == nullptr || fbcount == 0 ) 
    {
        so_app::log::error( "[glx_window::create_glx_window] : glXChooseFBConfig" ) ;
        return failed_gfx_context_creation ;
    }

    GLXFBConfig fbconfig = fbc[0] ;
    XFree( fbc ) ;
    
    XVisualInfo * vi = glXGetVisualFromFBConfig( display, fbconfig ) ;

    Colormap cm ;
    XSetWindowAttributes swa ;
    
    swa.colormap = cm = XCreateColormap( display, RootWindow( display, vi->screen ), vi->visual, AllocNone ) ;
    swa.background_pixmap = None ;
    swa.border_pixel = 0 ;
    swa.event_mask = StructureNotifyMask | ResizeRedirectMask ;

    Window window = XCreateWindow( display, RootWindow( display, vi->screen ), 0, 0,
            100,//WidthOfScreen( DefaultScreenOfDisplay( display ) ),
            100,//HeightOfScreen( DefaultScreenOfDisplay( display ) ),
            0, vi->depth, InputOutput, vi->visual, CWBorderPixel | CWColormap | CWEventMask, &swa ) ;

    if( so_app::log::error( !window, 
            "[glx_window::create_glx_window] : XCreateWindow" ) )
    {
        return 0 ;
    }
    
    XFree( vi ) ;

    XStoreName( display, window, wi.window_name.c_str() ) ;

    //
    // Create a glx context
    //
    {        
        auto res = so_gli::so_glx::glx::init( display, DefaultScreen( display ) ) ;
        if( so_app::log::error(so_gli::no_success(res), "[glx_window::create_glx_window] : init glx") )
        {
            return so_app::failed ;
        }
        
        int glx_major, glx_minor ;

        if( !glXQueryVersion( display, &glx_major, &glx_minor ) ) 
        {
            so_app::log::error("[glx_window::create_glx_window] : glXQueryVersion") ;
            return 0 ;
        }

        if( glx_major < 1 ) return so_app::failed_glx_version ;
        if( glx_minor < 3 ) return so_app::failed_glx_version ;
        
        
        int context_attribs[] =
        {
            GLX_CONTEXT_MAJOR_VERSION_ARB, gli.version.major,
            GLX_CONTEXT_MINOR_VERSION_ARB, gli.version.minor,
            None
        } ;

        GLXContext context = so_gli::so_glx::glx::glXCreateContextAttribs( display, 
                fbconfig, 0, True, context_attribs );

        if( so_app::log::error(!context, "[glx_window::create_glx_window] : glXCreateContextAttribs") ) 
        {
            return 0 ;
        }        

        glx_context_ptr_t glx_ctx_ptr = so_app::memory::alloc( 
            glx_context(), "[glx_window::glx_window] : glx_context" ) ;
        
        glx_ctx_ptr->create_context( display, window, context ) ;
        
        _context_ptr = glx_ctx_ptr ;
        
        
        glXMakeCurrent( display, window, context );
    }
            
    return window ;
}

