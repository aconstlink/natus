
#include "async.h"

using namespace natus::gpu ;

//****
async::async( void_t ) 
{}

//****
async::async( backend_res_t rptr ) : _backend( rptr ) 
{}

//****
async::async( this_rref_t rhv ) 
{
    _backend = ::std::move( rhv._backend ) ;
    _renders = ::std::move( rhv._renders ) ;
    _rconfigs = ::std::move( rhv._rconfigs ) ;
    _gconfigs = ::std::move( rhv._gconfigs ) ;
}

//****
async::~async( void_t ) 
{}

//****
natus::gpu::result async::configure( natus::gpu::async_id_res_t aid, 
    natus::gpu::geometry_configuration_in_t gconfig ) noexcept
{
    auto const res = aid->swap( natus::gpu::async_result::in_transit ) ;
    if( res != natus::gpu::async_result::in_transit )
    {
        _gconfigs.push_back( gconfig_data( { aid, gconfig } ) ) ;
    }
    return natus::gpu::result::ok ;
}

//****
natus::gpu::result async::configure( natus::gpu::async_id_res_t aid, natus::gpu::render_configurations_in_t rc ) noexcept 
{
    auto const res = aid->swap( natus::gpu::async_result::in_transit ) ;
    if( res != natus::gpu::async_result::in_transit ) 
    {
        _rconfigs.push_back( rconfig_data( {aid, rc } ) ) ;
    }
    
    return natus::gpu::result::ok ;
}

//****
natus::gpu::result async::render( natus::gpu::async_id_res_t aid ) noexcept 
{
    auto const res = aid->swap( natus::gpu::async_result::in_transit ) ;
    if( res != natus::gpu::async_result::in_transit )
    {
        _renders.push_back( { aid } ) ;
    }
    return natus::gpu::result::ok ;
}

//****
void_t async::system_update( void_t ) noexcept 
{
    // geometry configs
    {
        this_t::gconfigs_t preps ;
        {
            natus::concurrent::lock_guard_t lk( _gconfigs_mtx ) ;
            preps = ::std::move( _gconfigs ) ;
        }
        for( auto& prc : preps )
        {
            natus::gpu::id_t id = _backend->configure( prc.aid->id(), ::std::move( prc.config ) ) ;
            id = prc.aid->set( ::std::move( id ), natus::gpu::async_result::ok ) ;
            natus::log::global_t::error( id.is_valid(), natus_log_fn(
                "gpu resource need to be released." ) ) ;
            prc.aid->swap_config( natus::gpu::async_result::ok ) ;
        }
    }

    // render configs
    {
        this_t::rconfigs_t preps ;
        {
            natus::concurrent::lock_guard_t lk( _rconfigs_mtx ) ;
            preps = ::std::move( _rconfigs ) ;
        }
        for( auto& prc : preps )
        {
            natus::gpu::id_t id = _backend->configure( prc.aid->id(), ::std::move( prc.config ) ) ;
            id = prc.aid->set( ::std::move( id ), natus::gpu::async_result::ok ) ;
            natus::log::global_t::error( id.is_valid(), natus_log_fn(
                "gpu resource need to be released." ) ) ;
        }
    }

    {
        this_t::renders_t renders ;
        {
            natus::concurrent::lock_guard_t lk( _renders_mtx ) ;
            renders = ::std::move( _renders ) ;
        }

        for( auto& rnd : renders )
        {
            natus::gpu::id_t id = _backend->render( rnd.aid->id() ) ;

            id = rnd.aid->set( ::std::move( id ), natus::gpu::async_result::ok ) ;
            natus::log::global_t::error( id.is_valid(), natus_log_fn(
                "gpu resource need to be released." ) ) ;
        }
    }
}

//***
bool_t async::enter_frame( void_t ) 
{
    natus::concurrent::lock_guard_t lk( _frame_mtx ) ;
    if( natus::core::is_not( _ready ) ) return false ;
    _frame_ready = false ;
    return true ;
}

//***
void_t async::leave_frame( void_t ) 
{
    {
        natus::concurrent::lock_guard_t lk( _frame_mtx ) ;
        _frame_ready = true ;
    }
    _frame_cv.notify_all() ;
}

//***
void_t async::wait_for_frame( void_t ) 
{
    natus::concurrent::ulock_t lk( _frame_mtx ) ;
    while( _frame_ready ) _frame_cv.wait( lk ) ;
    _ready = false ;
}

//***
void_t async::set_ready( void_t ) 
{
    natus::concurrent::lock_guard_t lk( _frame_mtx ) ;
    _ready = true ;
}