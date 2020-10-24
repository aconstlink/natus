#include "d3d_context.h"

#include <natus/core/assert.h>

#include <natus/ntd/string/split.hpp>

#include <natus/log/global.h>

#include <d3d11.h>

using namespace natus::application ;
using namespace natus::application::d3d ;

//***********************************************************************
context::context( void_t )
{}

//***********************************************************************
context::context( HWND hwnd ) 
{
    this_t::create_context( hwnd ) ;
}

//***********************************************************************
context::context( HWND hwnd, HGLRC ctx ) 
{
    _hwnd = hwnd ;
    _hrc = ctx ;
}

//***********************************************************************
context::context( this_rref_t rhv )
{
    *this = ::std::move( rhv ) ;
}

//***********************************************************************
context::~context( void_t )
{
    this_t::deactivate() ;

    //if( _hrc != NULL )
      //  wglDeleteContext( _hrc ) ;
}

//***********************************************************************
context::this_ref_t context::operator = ( this_rref_t rhv )
{
    _hwnd = rhv._hwnd ;
    rhv._hwnd = NULL ;
    _hdc = rhv._hdc ;
    rhv._hdc = NULL ;
    _hrc = rhv._hrc ;
    rhv._hrc = NULL ;

    return *this ;
}

//***********************************************************************
natus::application::result context::activate( void_t ) 
{
    natus_assert( _hdc == NULL ) ;

    _hdc = GetDC( _hwnd ) ;

    // make current
        
    return natus::application::result::ok ;
}

//***********************************************************************
natus::application::result context::deactivate( void_t ) 
{
    if( _hdc == NULL ) return natus::application::result::ok ;

    // here 

    if( natus::log::global::error( ReleaseDC( _hwnd, _hdc ) == FALSE,
        natus_log_fn( "ReleaseDC" ) ) )
        return natus::application::result::failed_wgl ;
    
    _hdc = NULL ;

    return natus::application::result::ok ;
}

//***********************************************************************
natus::application::result context::vsync( bool_t const on_off ) 
{

    return natus::application::result::ok ;
}

//***********************************************************************
natus::application::result context::swap( void_t ) 
{
    if( _hdc == NULL ) 
        return natus::application::result::invalid_win32_handle ;

    if( natus::log::global::error( SwapBuffers( _hdc ) == FALSE, 
        "[context::swap] : SwapBuffers") ) 
        return natus::application::result::failed_wgl ;
    
    return natus::application::result::ok ;
}

//***********************************************************************
natus::application::result context::create_context( HWND hwnd ) 
{   
    _hwnd = hwnd ;
    
    if( natus::log::global::error( _hwnd == NULL, 
        "[context::create_context] : Window handle is no win32 handle." ) )
        return natus::application::result::invalid_argument ;

    return this_t::create_the_context( natus::application::gl_info_t() ) ;
}


//***********************************************************************
void_t context::clear_now( natus::math::vec4f_t const & /*vec*/ ) 
{
    #if 0
    natus::ogl::glClearColor( vec.x(), vec.y(), vec.z(), vec.w() ) ;
    natus::ogl::glClear( GL_COLOR_BUFFER_BIT ) ;
    
    GLenum const gler = natus::ogl::glGetError() ;
    natus::log::global::error( gler != GL_NO_ERROR, "[context::clear_now] : glClear" ) ;
    #endif
}

//***********************************************************************
natus::application::result context::create_the_context( gl_info_cref_t gli ) 
{
    typedef ::std::chrono::high_resolution_clock local_clock_t ;
    auto t1 = local_clock_t::now() ;

    HDC hdc = GetDC( _hwnd ) ;

    
    ReleaseDC( _hwnd, hdc ) ;
 
    {
        this_t::activate() ;
        
        natus::log::global::warning( natus::application::no_success( this_t::vsync( gli.vsync_enabled ) ), 
            "[context::create_the_context] : vsync setting failed." ) ;
        
        this_t::deactivate() ;
    }

    // timing end
    {
        size_t const milli = size_t( ::std::chrono::duration_cast<::std::chrono::milliseconds>(
            local_clock_t::now() - t1).count()) ;

        natus::log::global::status( natus_log_fn( "created [" + ::std::to_string(milli) +" ms]" ) ) ;
    }

    return natus::application::result::ok ;
}
