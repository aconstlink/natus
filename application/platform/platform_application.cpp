
#include "platform_application.h"
#include "../app.h"

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
platform_application::~platform_application( void_t )
{
    if( natus::core::is_not_nullptr(_sd) )
    {
        natus::memory::global_t::dealloc( _sd ) ;
    }
}

//******************************************************
natus::application::result platform_application::set( natus::application::app_rptr_t app )
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
        return natus::application::result::no_app ;

    _sd->update_running = true ;

    _thr = natus::concurrent::thread_t( [=]( void_t )
    {
        _app->init() ;

        while( _sd->update_running )
        {
            _app->update() ;
            _app->render() ;
        }
        
        _app->shutdown() ;

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
natus::application::result platform_application::exec( void_t ) 
{
    {
        auto const res = this_t::start_update_thread() ;
        if( natus::application::no_success( res ) )
            return res ;
    }

    // virtual call
    {
        this->on_exec() ;
    }

    this_t::stop_update_thread() ;

    return result::ok ;
}