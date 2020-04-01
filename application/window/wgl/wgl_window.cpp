
#include "wgl_window.h"

#include "../win32/win32_window.h"

//#include <natus/gpu/api/gl/gl_33/gl_33_api.h>
#include <natus/log/global.h>

using namespace natus::application ;
using namespace natus::application::win32 ;

//***********************************************************************
wgl_window::wgl_window( gl_info_cref_t gli, window_info_cref_t wi ) 
{
    natus::soil::res< win32_window_t > window ( win32_window::create( win32_window( wi ),
        "[wgl_window::wgl_window] : The render window.") ) ;

    win32_window_rptr_t wnd_ptr = win32_window_rptr_t( window ) ;

    this_t::set_decorated( wnd_ptr ) ;

    if( !wnd_ptr.is_valid() )
    {
        natus::log::global_t::error( "[wgl_window::wgl_window] : unable to create win32 window. aborting." ) ;
        return ;
    }
    
    _context_ptr = natus::application::memory::alloc( wgl_context(), "[wgl_window::wgl_window] : wgl_context" ) ;

    auto ctx_res = _context_ptr->create_context( gli, wnd_ptr->get_handle() ) ;
    if( natus::application::no_success( ctx_res ) )
    {
        natus::log::global_t::error( "[wgl_window::wgl_window] : Failed to create GL context." ) ;
        return ;
    }

    // create and initialize the gl api object
    /*{
        _gl_api_ptr = natus_gpu::natus_gl::gl_33_api_t::create( natus_gpu::natus_gl::gl_33_api_t( _context_ptr ),
            "[wgl_window::wgl_window] : gl_33_api" ) ;
    }*/

    _vsync = gli.vsync_enabled ;

#if 0
    // create and initialize the gl driver
    {
        _driver_ptr = natus_gpu::natus_gl::gl_33_api::create( natus_gpu::natus_gl::gl_33_api(_context_ptr), 
            "[wgl_window::wgl_window] : gl_driver" ) ;
        
        _context_ptr->activate() ;
        _driver_ptr->initialize() ;
        _context_ptr->deactivate() ;
    }
#endif

    // test the GL out by just clearing the 
    // window's color buffer.
    {
        _context_ptr->activate() ;
        _context_ptr->clear_now( natus::math::vec4f_t(0.0f,0.0f,0.0f,1.0f) ) ;
        _context_ptr->swap() ;
        _context_ptr->clear_now( natus::math::vec4f_t(0.1f,0.1f,0.1f,1.0f) ) ;
        _context_ptr->swap() ;
        _context_ptr->deactivate() ;
    }

#if 0

    // could be delivered via gl_info
    size_t const num_supports = 2 ;

    // create support drivers and contexts
    for( size_t i=0; i<num_supports; ++i )
    {
        natus::application::natus_win32::wgl_context_ptr_t sctx = _context_ptr->create_shared() ;
        if( sctx != nullptr ) _support_contexts.push_back( sctx ) ;

        auto * sdrv = natus_gpu::natus_gl::gl_33_api::create( 
            natus_gpu::natus_gl::gl_33_api(sctx), "[wgl_window::wgl_window] : support gl_driver" ) ;
        if( sdrv != nullptr ) _support_drivers.push_back( sdrv ) ;
    }


    base_t::init_and_start_render_threads(num_supports) ;
#endif
}

//***********************************************************************
wgl_window::wgl_window( this_rref_t rhv ) : decorator_window( ::std::move(rhv) )
{
    natus_move_member_ptr( _context_ptr, rhv ) ;
    //natus_move_member_ptr( _gl_api_ptr, rhv ) ;

    _vsync = rhv._vsync ;
}

//***********************************************************************
wgl_window::~wgl_window( void_t ) 
{
    //if( natus::core::is_not_nullptr( _gl_api_ptr ) )
      //  _gl_api_ptr->destroy() ;

    natus::application::memory::dealloc( _context_ptr ) ;

    // if this has been moved, the decorated is moved with it.
    // if this still has the decorated, it needs to be destroyed here
    // because it is created in this ctor.
    if( this_t::has_decorated() )
    {
        auto dec = this_t::get_decorated() ;
        // @todo force deletion and inform everyone connected
        // old code
        //win32_window::destroy( (win32_window_ptr_t)this_t::get_decorated() ) ;
        this_t::set_decorated( iwindow_rptr_t() ) ;
    }
}

//***********************************************************************
wgl_window::this_ptr_t wgl_window::create( this_rref_t rhv ) 
{
    return natus::application::memory::alloc( ::std::move(rhv), "[gl_window::create] : create" ) ;
}

//***********************************************************************
void_t wgl_window::destroy( this_ptr_t ptr ) 
{
    natus::application::memory::dealloc( ptr ) ;
}

//***********************************************************************
natus::application::result wgl_window::destroy( void_t )
{
    this_t::destroy( this ) ;

    return natus::application::result::ok ;
}

//***********************************************************************
irender_context_ptr_t wgl_window::get_render_context( void_t )
{
    return _context_ptr ;
}

//***********************************************************************
/*natus::gpu::iapi_ptr_t wgl_window::get_driver( void_t )
{
    return nullptr ;
}*/

//***********************************************************************
irender_context_ptr_t wgl_window::get_support_context( size_t i ) 
{
    return nullptr ;
}

//***********************************************************************
/*natus::gpu::iapi_ptr_t wgl_window::get_support_driver( size_t i )
{
    return nullptr ;
}*/

//***********************************************************************
void_t wgl_window::render_thread_begin( void_t )
{
    _context_ptr->activate() ;
}

//***********************************************************************
void_t wgl_window::render_thread_end( void_t )
{
    _context_ptr->deactivate() ;
}

//***********************************************************************
void_t wgl_window::end_frame( void_t )
{
    if( _tgl_vsync )
        _context_ptr->vsync( _vsync ) ;

    _context_ptr->swap() ;
}

//***********************************************************************
/*natus_gpu::iapi_ptr_t wgl_window::get_api( void_t )
{
    return _gl_api_ptr ;
}*/

//***********************************************************************
/*natus_gpu::api_type wgl_window::get_api_type( void_t )
{
    return natus_gpu::api_type::gl_33 ;
}*/

//***********************************************************************
void_t wgl_window::send_toggle( natus::application::toggle_window_in_t di )
{
    if( di.toggle_vsync )
    {
        _tgl_vsync = true ;
        _vsync = !_vsync ;
    }

    return decorator_window::send_toggle( di ) ;
}