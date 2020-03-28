#include "window_application.h"

#include "../window/iwindow.h"
#include "../window/window_message_receiver.h"

//#include <natus/device/global.h>
//#include <natus/device/system/idevice_system.h>

#include <natus/io/global.h>
#include <natus/concurrent/global.h>
#include <natus/concurrent/scheduler.h>
#include <natus/log/global.h>
#include <natus/core/macros/move.h>

using namespace natus::application ;

//***********************************************************************
window_application::window_application( void_t ) 
{
    _app_state_ptr = natus::application::memory::alloc( application_state_t(),
        "[window_application::window_application] : application_state" ) ;
}

//***********************************************************************
window_application::window_application( this_rref_t rhv )
{
    _windows = ::std::move( rhv._windows ) ;
    
    _update_thread = ::std::move( rhv._update_thread ) ;    

    natus_move_member_ptr( _app_state_ptr, rhv ) ;
}

//***********************************************************************
window_application::~window_application( void_t )
{
    shutdown_and_kill_all_windows() ;
    natus::application::memory::dealloc( _app_state_ptr ) ;
}

//***********************************************************************
natus::application::result window_application::register_window( natus::application::iwindow_ptr_t wptr )
{
    if( wptr == nullptr ) 
        return natus::application::result::invalid_argument ;

    auto iter = ::std::find_if( _windows.begin(), _windows.end(), [&]( window_data const & data )
        { return data.window_ptr == wptr ; } ) ;

    if( iter != _windows.end() ) 
        return natus::application::result::ok ;

    _windows.push_back( window_data { wptr } ) ;

    return natus::application::result::ok ;
}

//***********************************************************************
natus::application::result window_application::exec( void_t ) 
{
    return this_t::exec( [=]( application_state_ptr_t as )
    { 
        while( as->is_running() )
        {
            // @todo rawinput
            //natus_device::global::device_system()->update() ;
            //std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) ) ;
        }        
        as->update_finised() ;  
    } ) ;
}

//***********************************************************************
natus::application::result window_application::exec( ::std::function< void_t( application_state_ptr_t ) > update_funk )
{
    if( _windows.size() == 0 )
        return natus::application::result::ok ;

    _app_state_ptr->update_begin() ;

    _update_thread = natus::concurrent::thread_t( [=]( void_t )
    {
        update_funk( _app_state_ptr ) ;
    } ) ;

    auto const res = this->exec_derived() ;
    {
        this_t::shutdown_and_kill_all_windows() ;
    }

    return res ;
}

//***********************************************************************
natus::application::result window_application::shutdown_and_kill_all_windows( void_t ) 
{
    natus::application::result res = shutdown_update_thread() ;
    natus::log::global_t::error( natus::application::no_success(res), 
        "[window_application::~window_application] : failed to shutdown update thread." ) ;

    for( auto & data : _windows )
    {
        data.window_ptr->destroy() ;
    }
    _windows.clear() ;

    if( _update_thread.joinable() )
        _update_thread.join()  ;

    return natus::application::result::ok ;
}

//***********************************************************************
natus::application::result window_application::shutdown_update_thread( void_t ) 
{
    if( natus::core::is_nullptr( _app_state_ptr ) )
        return natus::application::result::ok ;

    if( natus::core::is_not( _app_state_ptr->is_running() ) ) 
        return natus::application::result::ok ;

    _app_state_ptr->stop() ;
    _app_state_ptr->wait_for_finish() ;

    return natus::application::result::ok ;
}

