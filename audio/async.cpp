
#include "async.h"

#include <natus/concurrent/typedefs.h>

using namespace natus::audio ;

async::async( void_t ) 
{
}

async::async( backend_res_t bke ) : _backend( bke )
{
}

async::async( this_rref_t rhv ) 
{
    _backend = std::move( rhv._backend ) ;
    _captures = std::move( rhv._captures ) ;
    _capture_configs = std::move( rhv._capture_configs ) ;
    _num_enter = rhv._num_enter ;
}

async::~async( void_t ) 
{
}

async::this_ref_t async::configure( natus::audio::capture_type const ct, natus::audio::capture_object_res_t config, natus::audio::result_res_t res ) noexcept
{
    {
        natus::concurrent::lock_guard_t lk( _capture_configs_mtx ) ;
        _capture_configs.push_back( this_t::capture_config_data( { res, ct, config } ) ) ;
    }
    return *this ;
}

void_t async::system_update( void_t ) noexcept
{
    /////////////////////////////////////////////////
    /////// Configure

    // capture configs
    {
        this_t::capture_configs_t preps ;
        {
            natus::concurrent::lock_guard_t lk( _capture_configs_mtx ) ;
            preps = ::std::move( _capture_configs ) ;
        }
        for( auto& prc : preps )
        {
            auto const res = _backend->configure( prc.ct, ::std::move( prc.config ) ) ;
            if( prc.res.is_valid() ) prc.res = res ;
        }
    }


    /////////////////////////////////////////////////
    /////// render/capture/..

    {
        _backend->begin() ;

        this_t::captures_t captures ;
        {
            natus::concurrent::lock_guard_t lk( _captures_mtx ) ;
            captures = ::std::move( _captures ) ;
        }

        for( auto& rnd : captures )
        {
            auto const res = _backend->capture( ::std::move( rnd.config ), rnd.begin_capture ) ;
            if( rnd.res.is_valid() ) rnd.res = res ;
        }

        _backend->end() ;
    }

    {
        natus::concurrent::lock_guard_t lk( _frame_mtx ) ;
        _num_enter = 0 ;
    }
}

bool_t async::enter_frame( void_t )
{
    natus::concurrent::lock_guard_t lk( _frame_mtx ) ;
    if( _num_enter > 0 ) return false ;
    return true ;
}

void_t async::leave_frame( void_t )
{
    {
        natus::concurrent::lock_guard_t lk( _frame_mtx ) ;
        _num_enter = 1 ;
    }
    _frame_cv.notify_all() ;
}

void_t async::wait_for_frame( void_t )
{
    natus::concurrent::ulock_t lk( _frame_mtx ) ;
    while( _num_enter == 0 ) _frame_cv.wait( lk ) ;
}