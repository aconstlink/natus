
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

async::this_ref_t async::capture( natus::audio::capture_object_res_t cap, bool_t const do_capture,
    natus::audio::result_res_t res ) noexcept
{
    {
        natus::concurrent::lock_guard_t lk( _captures_mtx ) ;
        _captures.push_back( { res, cap, do_capture } ) ;
    }
    return *this ;
}

async::this_ref_t async::configure( natus::audio::buffer_object_res_t po,
    natus::audio::result_res_t res ) noexcept 
{
    {
        natus::concurrent::lock_guard_t lk( _buffer_configs_mtx ) ;
        _buffer_configs.push_back( this_t::buffer_config_data( { res, po, false } ) ) ;
    }
    return *this ;
}

async::this_ref_t async::update( natus::audio::buffer_object_res_t po,
    natus::audio::result_res_t res ) noexcept 
{
    {
        natus::concurrent::lock_guard_t lk( _buffer_configs_mtx ) ;
        _buffer_configs.push_back( this_t::buffer_config_data( { res, po, true } ) ) ;
    }
    return *this ;
}

async::this_ref_t async::execute( natus::audio::buffer_object_res_t po, 
    natus::audio::backend::execute_detail_cref_t det,
    natus::audio::result_res_t res ) noexcept 
{
    if( res.is_valid() ) *res = natus::audio::result::in_progress ;

    {
        natus::concurrent::lock_guard_t lk( _plays_mtx ) ;
        _plays.push_back( { res, po, det } ) ;
    }
    return *this ;
}

void_t async::enter_thread( void_t ) noexcept
{
    _backend->init() ;
}

void_t async::leave_thread( void_t ) noexcept
{
    _backend->release() ;
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
            if( prc.res.is_valid() ) *prc.res = res ;
        }
    }

    // buffer objects
    {
        this_t::buffer_configs_t preps ;
        {
            natus::concurrent::lock_guard_t lk( _buffer_configs_mtx ) ;
            preps = ::std::move( _buffer_configs ) ;
        }
        for( auto& prc : preps )
        {
            if( !prc.update_only )
            {
                auto const res = _backend->configure( std::move( prc.obj ) ) ;
                if( prc.res.is_valid() ) prc.res = res ;
            }
            else
            {
                auto const res = _backend->update( std::move( prc.obj ) ) ;
                if( prc.res.is_valid() ) prc.res = res ;
            }
        }
    }


    /////////////////////////////////////////////////
    /////// render/capture/..

    {
        _backend->begin() ;

        {
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
        }

        {
            this_t::plays_t plays ;
            {
                natus::concurrent::lock_guard_t lk( _plays_mtx ) ;
                plays = ::std::move( _plays ) ;
            }

            for( auto& rnd : plays )
            {
                auto const res = _backend->execute( std::move( rnd.obj ), rnd.ed ) ;
                if( rnd.res.is_valid() ) rnd.res = res ;
            }
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