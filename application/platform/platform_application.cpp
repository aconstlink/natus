
#include "platform_application.h"
#include "../app.h"

#include <natus/device/global.h>
#include <natus/memory/global.h>
#include <natus/log/global.h>

using namespace natus ;
using namespace natus::application ;

//******************************************************
platform_application::platform_application( void_t ) 
{
    _sd = natus::memory::global::alloc< this_t::shared_data >() ;
}

//******************************************************
platform_application::platform_application( this_rref_t rhv )
{
    _thr = ::std::move( rhv._thr ) ;
    natus_move_member_ptr( _sd, rhv ) ;
    _app = ::std::move( rhv._app ) ;
}

//******************************************************
platform_application::platform_application( natus::application::app_res_t app ) 
{
    this_t::set( app ) ;
    _sd = natus::memory::global::alloc< this_t::shared_data >() ;
}

//******************************************************
platform_application::~platform_application( void_t )
{
    if( natus::core::is_not_nullptr(_sd) )
    {
        natus::memory::global_t::dealloc( _sd ) ;
    }
}

//******************************************************
natus::application::result platform_application::set( natus::application::app_res_t app )
{
    if( _app.is_valid() )
        return natus::application::result::failed ;

    _app = app ;

    return natus::application::result::ok ;
}

//******************************************************
natus::application::result platform_application::start_update_thread( void_t ) 
{
    if( natus::core::is_not( _app.is_valid() ) )
    {
        natus::log::global_t::error( natus_log_fn("No app object") ) ;
        return natus::application::result::no_app ;
    }

    _sd->update_running = true ;
    
    _thr = natus::concurrent::thread_t( [=]( void_t )
    {
        _app->on_init() ;

        while( _sd->update_running )
        {
            natus::device::global_t::system()->update() ;

            if( _app->before_update() )
            {
                _app->on_update() ;
                _app->after_update() ;
            }

            if( _app->before_render() ) 
            {
                _app->on_render() ;
                _app->after_render() ;
            }
        }
        
        _app->on_shutdown() ;

    } ) ;

    return natus::application::result::ok ;
}

//******************************************************
void_t platform_application::stop_update_thread( void_t ) 
{
    if( natus::core::is_not( _sd->update_running ) )
        return ;

    _sd->update_running = false ;
    _thr.join() ;
}

//******************************************************
int_t platform_application::exec( void_t )
{
    {
        auto const res = this_t::start_update_thread() ;
        if( natus::application::no_success( res ) )
        {
            natus::log::global_t::warning( natus_log_fn(
                "No update thread started. Proceed anyway.") ) ;
        }
    }

    // virtual call
    {
        this->on_exec() ;
    }

    this_t::stop_update_thread() ;

    return 0 ;
}
