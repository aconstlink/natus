
#include "async.h"

using namespace natus::graphics ;

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
natus::graphics::result async::set_window_info( natus::graphics::backend_t::window_info_cref_t wi ) noexcept 
{
    natus::concurrent::lock_guard_t lk( _window_info_mtx ) ;
    _window_info_set = true ;
    _window_info = wi ;
    return natus::graphics::result::ok ;
}

//****
async::this_ref_t async::configure( natus::graphics::geometry_configuration_res_t gconfig, 
    natus::graphics::result_res_t res ) noexcept
{
    //auto const res = aid->swap( natus::graphics::async_result::in_transit ) ;
    //if( res != natus::graphics::async_result::in_transit )
    {
        natus::concurrent::lock_guard_t lk( _gconfigs_mtx ) ;
        _gconfigs.push_back( gconfig_data( { res, gconfig } ) ) ;
    }
    return *this ;
}

//****
async::this_ref_t async::configure( natus::graphics::render_configuration_res_t rc, 
    natus::graphics::result_res_t res ) noexcept 
{
    //auto const res = aid->swap( natus::graphics::async_result::in_transit ) ;
    //if( res != natus::graphics::async_result::in_transit ) 
    {
        natus::concurrent::lock_guard_t lk( _rconfigs_mtx ) ;
        _rconfigs.push_back( rconfig_data( { res, rc } ) ) ;
    }
    
    return *this ;
}

//****
async::this_ref_t async::configure( natus::graphics::shader_configuration_res_t sc,
    natus::graphics::result_res_t res ) noexcept 
{
    {
        natus::concurrent::lock_guard_t lk( _shader_configs_mtx ) ;
        _shader_configs.push_back( shader_config_data( { res, sc } ) ) ;
    }

    return *this ;
}

//****
async::this_ref_t async::configure( natus::graphics::image_configuration_res_t sc,
    natus::graphics::result_res_t res ) noexcept 
{
    {
        natus::concurrent::lock_guard_t lk( _image_configs_mtx ) ;
        _image_configs.push_back( image_config_data( { res, sc } ) ) ;
    }

    return *this ;
}

//****
async::this_ref_t async::connect( natus::graphics::render_configuration_res_t config, 
    natus::graphics::variable_set_res_t vs, natus::graphics::result_res_t res ) noexcept 
{
    {
        natus::concurrent::lock_guard_t lk( _connects_mtx ) ;
        _connects.push_back( connect_data( { res, vs, config } ) ) ;
    }
    return *this ;
}

async::this_ref_t async::update( natus::graphics::geometry_configuration_res_t gs, 
    natus::graphics::result_res_t res ) noexcept 
{
    {
        natus::concurrent::lock_guard_t lk( _gupdates_mtx ) ;
        _gupdates.push_back( gupdate_data( { res, gs } ) ) ;
    }
    return *this ;
}

//****
async::this_ref_t async::render( natus::graphics::render_configuration_res_t config, natus::graphics::backend::render_detail_cref_t detail,
    natus::graphics::result_res_t res ) noexcept
{
    //auto const res = aid->swap( natus::graphics::async_result::in_transit ) ;
    //if( res != natus::graphics::async_result::in_transit )
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
        natus::graphics::backend_t::window_info_t wi ;
        {
            natus::concurrent::lock_guard_t lk( _window_info_mtx ) ;
            wi = _window_info ;
            _window_info_set = false ;
        }
        _backend->set_window_info( wi ) ;
    }

    // image configs
    {
        this_t::image_configs_t preps ;
        {
            natus::concurrent::lock_guard_t lk( _image_configs_mtx ) ;
            preps = ::std::move( _image_configs ) ;
        }
        for( auto& prc : preps )
        {
            auto const res = _backend->configure( ::std::move( prc.config ) ) ;
            if( prc.res.is_valid() ) prc.res = res ;
        }
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
            auto const res = _backend->configure( ::std::move( prc.config ) ) ;
            if( prc.res.is_valid() ) prc.res = res ;
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
            auto const res = _backend->update( ::std::move( prc.config ) ) ;
            if( prc.res.is_valid() ) prc.res = res ;
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
            auto const res = _backend->configure( ::std::move( prc.config ) ) ;
            if( prc.res.is_valid() ) prc.res = res ;
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
            auto const res = _backend->connect( ::std::move( prc.config ), ::std::move( prc.vs ) ) ;
            if( prc.res.is_valid() ) prc.res = res ;
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