
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
natus::gpu::result async::set_window_info( natus::gpu::backend_t::window_info_cref_t wi ) noexcept 
{
    natus::concurrent::lock_guard_t lk( _window_info_mtx ) ;
    _window_info_set = true ;
    _window_info = wi ;
    return natus::gpu::result::ok ;
}

//****
natus::gpu::result async::configure( natus::gpu::async_id_res_t aid, 
    natus::gpu::geometry_configuration_res_t gconfig ) noexcept
{
    //auto const res = aid->swap( natus::gpu::async_result::in_transit ) ;
    //if( res != natus::gpu::async_result::in_transit )
    {
        natus::concurrent::lock_guard_t lk( _gconfigs_mtx ) ;
        _gconfigs.push_back( gconfig_data( { aid, gconfig } ) ) ;
    }
    return natus::gpu::result::ok ;
}

//****
natus::gpu::result async::configure( natus::gpu::async_id_res_t aid, natus::gpu::render_configurations_t configs ) noexcept
{
    natus::gpu::render_configuration_res_t config ;
    if( natus::core::is_not( configs.find_configuration(_backend->get_type(), config ) ) )
    {
        return natus::gpu::result::invalid_argument ;
    }

    return this_t::configure( aid, config ) ;
}

//****
natus::gpu::result async::configure( natus::gpu::async_id_res_t aid, natus::gpu::render_configuration_res_t rc ) noexcept 
{
    //auto const res = aid->swap( natus::gpu::async_result::in_transit ) ;
    //if( res != natus::gpu::async_result::in_transit ) 
    {
        natus::concurrent::lock_guard_t lk( _rconfigs_mtx ) ;
        _rconfigs.push_back( rconfig_data( {aid, rc } ) ) ;
    }
    
    return natus::gpu::result::ok ;
}

//****
natus::gpu::result async::connect( natus::gpu::async_id_res_t aid, natus::gpu::variable_set_res_t vs ) noexcept 
{
    {
        natus::concurrent::lock_guard_t lk( _connects_mtx ) ;
        _connects.push_back( connect_data( { aid, vs } ) ) ;
    }
    return natus::gpu::result::ok ;
}

//****
natus::gpu::result async::render( natus::gpu::async_id_res_t aid ) noexcept 
{
    //auto const res = aid->swap( natus::gpu::async_result::in_transit ) ;
    //if( res != natus::gpu::async_result::in_transit )
    {
        natus::concurrent::lock_guard_t lk( _renders_mtx ) ;
        _renders.push_back( { aid } ) ;
    }
    return natus::gpu::result::ok ;
}

//****
void_t async::system_update( void_t ) noexcept 
{
    // window info
    if( _window_info_set )
    {
        natus::gpu::backend_t::window_info_t wi ;
        {
            natus::concurrent::lock_guard_t lk( _window_info_mtx ) ;
            wi = _window_info ;
            _window_info_set = false ;
        }
        _backend->set_window_info( wi ) ;
    }

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

    // connects
    {
        this_t::connects_t preps ;
        {
            natus::concurrent::lock_guard_t lk( _connects_mtx ) ;
            preps = ::std::move( _connects ) ;
        }
        for( auto& prc : preps )
        {
            natus::gpu::id_t id = _backend->connect( prc.aid->id(), ::std::move( prc.vs ) ) ;
            id = prc.aid->set( ::std::move( id ), natus::gpu::async_result::ok ) ;
            natus::log::global_t::error( id.is_valid(), natus_log_fn(
                "gpu resource need to be released." ) ) ;
        }
    }

    {
        _backend->render_begin() ;

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

        _backend->render_end() ;
    }

    {
        natus::concurrent::lock_guard_t lk( _frame_mtx ) ;
        _frame_ready = false ;
    }
}

//***
bool_t async::enter_frame( void_t ) 
{
    natus::concurrent::lock_guard_t lk( _frame_mtx ) ;
    if( natus::core::is_not( _ready )  ) return false ;
    _frame_ready = false ;
    return true ;
}

//***
void_t async::leave_frame( bool_t const rendered ) 
{
    {
        natus::concurrent::lock_guard_t lk( _frame_mtx ) ;
        _frame_ready = rendered ;
    }
    if( rendered ) _frame_cv.notify_all() ;
}

//***
void_t async::wait_for_frame( void_t ) 
{
    //natus::log::global_t::status("wait for frame" ) ;

    natus::concurrent::ulock_t lk( _frame_mtx ) ;
    while( natus::core::is_not(_frame_ready) ) _frame_cv.wait( lk ) ;    
    _ready = false ;
    //natus::log::global_t::status("wait for frame end" ) ;
}

//***
void_t async::set_ready( void_t ) 
{
    natus::concurrent::lock_guard_t lk( _frame_mtx ) ;
    _ready = true ;
}
