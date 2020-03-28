#include "null_window.h"

#include <natus/core/macros/move.h>

#include <thread>
#include <chrono>

using namespace natus::application ;
using namespace natus::application::natus_null ;

//***********************************************************************
null_window::null_window( void_t )
{
    _handle = natus::application::memory::alloc( null_window_handle_t(), 
        "[null_window::null_window] : null_window_handle" ) ;

    // @todo gpu
    //_api = natus::gpu::natus_null::null_api::create( 
      //  "[null_window::null_window] : null_api" ) ;
}

//***********************************************************************
null_window::null_window( this_rref_t rhv )
{
    natus_move_member_ptr( _handle, rhv ) ;
    natus_move_member_ptr( _api, rhv ) ;
}

//***********************************************************************
null_window::~null_window( void_t )
{
    if( natus::core::is_not_nullptr( _handle ) )
    {
        natus::application::memory::dealloc( _handle ) ;
    }

    // @todo gpu
    #if 0
    if( natus::core::is_not_nullptr( _api ) )
    {
        natus::gpu::natus_null::null_api::destroy( _api ) ;
    }
    #endif
}

//***********************************************************************
null_window::this_ptr_t null_window::create( natus::memory::purpose_cref_t p )
{
    return natus::application::memory::alloc( this_t(), p ) ;
}

//***********************************************************************
void_t null_window::destroy( this_ptr_t ptr )
{
    natus::application::memory::dealloc( ptr ) ;
}

//***********************************************************************
natus::application::result null_window::subscribe( iwindow_message_listener_ptr_t )
{
    return natus::application::result::not_implemented ;
}

//***********************************************************************
natus::application::result null_window::unsubscribe( iwindow_message_listener_ptr_t )
{
    return natus::application::result::not_implemented ;
}

//***********************************************************************
natus::application::result null_window::destroy( void_t )
{
    this_t::destroy( this ) ;
    return natus::application::result::ok ;
}

//***********************************************************************
iwindow_handle_ptr_t null_window::get_handle( void_t )
{
    return _handle ;
}

//***********************************************************************
natus::std::string_cref_t null_window::get_name( void_t ) const
{
    return _name ;
}

//***********************************************************************
void_t null_window::send_close( void_t )
{
}

//***********************************************************************
void_t null_window::render_thread_begin( void_t )
{

}

//***********************************************************************
void_t null_window::render_thread_end( void_t )
{

}

//***********************************************************************
void_t null_window::end_frame( void_t )
{
    //natus_log::global::status( "[null_window::end_frame] : end_frame" ) ;
    ::std::this_thread::sleep_for( ::std::chrono::milliseconds(100) ) ;
}

// @todo gpu
#if 0
//***********************************************************************
natus::gpu::iapi_ptr_t null_window::get_api( void_t )
{
    return _api ;
}

//***********************************************************************
natus::gpu::api_type null_window::get_api_type( void_t )
{
    return natus_gpu::api_type::null ;
}
#endif