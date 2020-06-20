
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
    _num_enter = rhv._num_enter ;
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
async::this_ref_t async::configure( natus::gpu::render_configuration_res_t rc, natus::gpu::result_res_t res ) noexcept 
{
    //auto const res = aid->swap( natus::gpu::async_result::in_transit ) ;
    //if( res != natus::gpu::async_result::in_transit ) 
    {
        natus::concurrent::lock_guard_t lk( _rconfigs_mtx ) ;
        _rconfigs.push_back( rconfig_data( { res, rc } ) ) ;
    }
    
    return *this ;
}

//****
natus::gpu::result async::configure( natus::gpu::async_id_res_t aid, natus::gpu::shader_configuration_res_t sc ) noexcept 
{
    {
        natus::concurrent::lock_guard_t lk( _shader_configs_mtx ) ;
        _shader_configs.push_back( shader_config_data( { aid, sc } ) ) ;
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

natus::gpu::result async::update( natus::gpu::async_id_res_t aid, natus::gpu::geometry_configuration_res_t gs ) noexcept 
{
    {
        natus::concurrent::lock_guard_t lk( _gupdates_mtx ) ;
        _gupdates.push_back( gupdate_data( { aid, gs } ) ) ;
    }
    return natus::gpu::result::ok ;
}

//****
async::this_ref_t async::render( natus::gpu::render_configuration_res_t config, natus::gpu::backend::render_detail_cref_t detail,
    natus::gpu::result_res_t res ) noexcept
{
    //auto const res = aid->swap( natus::gpu::async_result::in_transit ) ;
    //if( res != natus::gpu::async_result::in_transit )
    {
        natus::concurrent::lock_guard_t lk( _renders_mtx ) ;
        _renders.push_back( { res, config, detail } ) ;
    }
    return *this ;
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

    // geometry update
    {
        this_t::gupdates_t preps ;
        {
            natus::concurrent::lock_guard_t lk( _gupdates_mtx ) ;
            preps = ::std::move( _gupdates ) ;
        }
        for( auto& prc : preps )
        {
            natus::gpu::id_t id = _backend->update( prc.aid->id(), ::std::move( prc.config ) ) ;
            id = prc.aid->set( ::std::move( id ), natus::gpu::async_result::ok ) ;
            natus::log::global_t::error( id.is_valid(), natus_log_fn(
                "gpu resource need to be released." ) ) ;
        }
    }

    // shader configs
    {
        this_t::shader_configs_t preps ;
        {
            natus::concurrent::lock_guard_t lk( _shader_configs_mtx ) ;
            preps = ::std::move( _shader_configs ) ;
        }
        for( auto& prc : preps )
        {
            natus::gpu::id_t id = _backend->configure( prc.aid->id(), ::std::move( prc.config ) ) ;
            id = prc.aid->set( ::std::move( id ), natus::gpu::async_result::ok ) ;
            natus::log::global_t::error( id.is_valid(), natus_log_fn(
                "gpu resource need to be released." ) ) ;
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
            auto const res = _backend->configure( ::std::move( prc.config ) ) ;
            if( prc.res.is_valid() ) prc.res = res ;
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
            auto const res = _backend->render( ::std::move( rnd.config ), rnd.detail ) ;
            if( rnd.res.is_valid() ) rnd.res = res ;
        }

        _backend->render_end() ;
    }

    {
        natus::concurrent::lock_guard_t lk( _frame_mtx ) ;
        _num_enter = 0 ;
    }
}

//***
bool_t async::enter_frame( void_t ) 
{
    natus::concurrent::lock_guard_t lk( _frame_mtx ) ;
    if( _num_enter > 0 ) return false ;
    return true ;
}

//***
void_t async::leave_frame( void_t ) 
{
    {
        natus::concurrent::lock_guard_t lk( _frame_mtx ) ;
        _num_enter = 1 ;
    }
    _frame_cv.notify_all() ;
}

//***
void_t async::wait_for_frame( void_t ) 
{
    natus::concurrent::ulock_t lk( _frame_mtx ) ;
    while( _num_enter == 0 ) _frame_cv.wait( lk ) ;
}