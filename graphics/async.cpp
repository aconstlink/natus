
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
    _runtimes = std::move( rhv._runtimes ) ;
    _configures = std::move( rhv._configures ) ;
    _backend = std::move( rhv._backend ) ;   
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
async::this_ref_t async::configure( natus::graphics::geometry_object_res_t gconfig, 
    natus::graphics::result_res_t res ) noexcept{
    
    {
        natus::concurrent::lock_guard_t lk( _configures_mtx ) ;
        _configures.emplace_back( [=] ( natus::graphics::backend_ptr_t be ) mutable 
        { 
            auto const ires = be->configure( std::move( gconfig ) ) ;
            if( res.is_valid() ) *res = ires ;
        } ) ;
    }
    return *this ;
}

//****
async::this_ref_t async::configure( natus::graphics::render_object_res_t rc, 
    natus::graphics::result_res_t res ) noexcept 
{
    {
        natus::concurrent::lock_guard_t lk( _configures_mtx ) ;
        _configures.emplace_back( [=] ( natus::graphics::backend_ptr_t be ) mutable
        {
            auto const ires = be->configure( std::move( rc ) ) ;
            if( res.is_valid() ) *res = ires ;
        } ) ;
    }
    
    return *this ;
}

//****
async::this_ref_t async::configure( natus::graphics::shader_object_res_t sc,
    natus::graphics::result_res_t res ) noexcept 
{
    {
        natus::concurrent::lock_guard_t lk( _configures_mtx ) ;
        _configures.emplace_back( [=] ( natus::graphics::backend_ptr_t be ) mutable
        {
            auto const ires = be->configure( std::move( sc ) ) ;
            if( res.is_valid() ) *res = ires ;
        } ) ;
    }

    return *this ;
}

//****
async::this_ref_t async::configure( natus::graphics::image_object_res_t sc,
    natus::graphics::result_res_t res ) noexcept 
{
    {
        natus::concurrent::lock_guard_t lk( _configures_mtx ) ;
        _configures.emplace_back( [=] ( natus::graphics::backend_ptr_t be ) mutable
        {
            auto const ires = be->configure( std::move( sc ) ) ;
            if( res.is_valid() ) *res = ires ;
        } ) ;
    }

    return *this ;
}

//****
async::this_ref_t async::configure( natus::graphics::framebuffer_object_res_t fb, 
    natus::graphics::result_res_t res ) noexcept 
{
    {
        natus::concurrent::lock_guard_t lk( _configures_mtx ) ;
        _configures.emplace_back( [=] ( natus::graphics::backend_ptr_t be ) mutable
        {
            auto const ires = be->configure( std::move( fb ) ) ;
            if( res.is_valid() ) *res = ires ;
        } ) ;
    }
    return *this ;
}

async::this_ref_t async::configure( natus::graphics::state_object_res_t s, 
    natus::graphics::result_res_t res ) noexcept 
{
    {
        natus::concurrent::lock_guard_t lk( _configures_mtx ) ;
        _configures.emplace_back( [=] ( natus::graphics::backend_ptr_t be ) mutable
        {
            auto const ires = be->configure( std::move( s ) ) ;
            if( res.is_valid() ) *res = ires ;
        } ) ;
    }

    return *this ;
}

async::this_ref_t async::update( natus::graphics::geometry_object_res_t gs, 
    natus::graphics::result_res_t res ) noexcept 
{
    {
        natus::concurrent::lock_guard_t lk( _runtimes_mtx ) ;

        _configures.push_back( [=] ( natus::graphics::backend_ptr_t be ) mutable
        {
            auto const ires = be->update( std::move( gs ) ) ;
            if( res.is_valid() ) *res = ires ;
        } ) ;
    }

    return *this ;
}

//****
async::this_ref_t async::use( natus::graphics::framebuffer_object_res_t fb, bool_t const clear_color,
    bool_t const clear_depth, bool_t const clear_stencil,
    natus::graphics::result_res_t res ) noexcept 
{
    {
        natus::concurrent::lock_guard_t lk( _runtimes_mtx ) ;

        _runtimes.push_back( [=] ( natus::graphics::backend_ptr_t be ) mutable
        {
            auto const ires = be->use( std::move( fb ), clear_color, clear_depth, clear_stencil ) ;
            if( res.is_valid() ) *res = ires ;
        } ) ;
    }
    return *this ;
}

//****
async::this_ref_t async::use( natus::graphics::state_object_res_t s, size_t const sid, bool_t const push, 
    natus::graphics::result_res_t res ) noexcept 
{
    natus::concurrent::lock_guard_t lk( _runtimes_mtx ) ;

    _runtimes.push_back( [=] ( natus::graphics::backend_ptr_t be ) mutable
    {
        auto const ires = be->use( std::move( s ), sid, push ) ;
        if( res.is_valid() ) *res = ires ;
    } ) ;
    return *this ;
}

//****
async::this_ref_t async::render( natus::graphics::render_object_res_t config, natus::graphics::backend::render_detail_cref_t detail,
    natus::graphics::result_res_t res ) noexcept
{
    {
        natus::concurrent::lock_guard_t lk( _runtimes_mtx ) ;

        _runtimes.push_back( [=] ( natus::graphics::backend_ptr_t be ) mutable
        { 
            auto const ires = be->render( std::move( config ), detail ) ;
            if( res.is_valid() ) *res = ires ;
        } ) ;
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

    // configures
    {
        this_t::commands_t coms ;
        {
            natus::concurrent::lock_guard_t lk( _configures_mtx ) ;
            coms = std::move( _configures ) ;
        }

        auto* bptr = _backend.get_sptr().get() ;
        for( auto& rtz : coms )
        {
            rtz( bptr ) ;
        }
    }

    // runtime functions
    {
        _backend->render_begin() ;

        this_t::commands_t coms ;
        {
            natus::concurrent::lock_guard_t lk( _runtimes_mtx ) ;
            coms = std::move( _runtimes ) ;
        }

        auto* bptr = _backend.get_sptr().get() ;
        for( auto& rtz : coms )
        {
            rtz( bptr ) ;
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