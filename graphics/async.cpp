
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
    _runtimes[0] = std::move( rhv._runtimes[0] ) ;
    _runtimes[1] = std::move( rhv._runtimes[1] ) ;
    _configures[0] = std::move( rhv._configures[0] ) ;
    _configures[1] = std::move( rhv._configures[1] ) ;
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
        _configures[_configures_id].push_back( [=] ( natus::graphics::backend_ptr_t be ) mutable 
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
        _configures[_configures_id].push_back( [=] ( natus::graphics::backend_ptr_t be ) mutable
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
        _configures[_configures_id].push_back( [=] ( natus::graphics::backend_ptr_t be ) mutable
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
        _configures[_configures_id].push_back( [=] ( natus::graphics::backend_ptr_t be ) mutable
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
        _configures[_configures_id].push_back( [=] ( natus::graphics::backend_ptr_t be ) mutable
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
        _configures[_configures_id].push_back( [=] ( natus::graphics::backend_ptr_t be ) mutable
        {
            auto const ires = be->configure( std::move( s ) ) ;
            if( res.is_valid() ) *res = ires ;
        } ) ;
    }

    return *this ;
}

async::this_ref_t async::configure( natus::graphics::array_object_res_t obj, 
    natus::graphics::result_res_t res ) noexcept 
{
    {
        natus::concurrent::lock_guard_t lk( _configures_mtx ) ;
        _configures[_configures_id].push_back( [=] ( natus::graphics::backend_ptr_t be ) mutable
        {
            auto const ires = be->configure( std::move( obj ) ) ;
            if( res.is_valid() ) *res = ires ;
        } ) ;
    }

    return *this ;
}

async::this_ref_t async::release( natus::graphics::geometry_object_res_t obj, natus::graphics::result_res_t res ) noexcept 
{
    {
        natus::concurrent::lock_guard_t lk( _configures_mtx ) ;
        _configures[_configures_id].push_back( [=] ( natus::graphics::backend_ptr_t be ) mutable
        {
            auto const ires = be->release( std::move( obj ) ) ;
            if( res.is_valid() ) *res = ires ;
        } ) ;
    }

    return *this ;
}

async::this_ref_t async::release( natus::graphics::render_object_res_t obj, natus::graphics::result_res_t res ) noexcept
{
    {
        natus::concurrent::lock_guard_t lk( _configures_mtx ) ;
        _configures[_configures_id].push_back( [=] ( natus::graphics::backend_ptr_t be ) mutable
        {
            auto const ires = be->release( std::move( obj ) ) ;
            if( res.is_valid() ) *res = ires ;
        } ) ;
    }

    return *this ;
}

async::this_ref_t async::release( natus::graphics::shader_object_res_t obj, natus::graphics::result_res_t res ) noexcept
{
    {
        natus::concurrent::lock_guard_t lk( _configures_mtx ) ;
        _configures[_configures_id].push_back( [=] ( natus::graphics::backend_ptr_t be ) mutable
        {
            auto const ires = be->release( std::move( obj ) ) ;
            if( res.is_valid() ) *res = ires ;
        } ) ;
    }

    return *this ;
}

async::this_ref_t async::release( natus::graphics::image_object_res_t obj, natus::graphics::result_res_t res ) noexcept
{
    {
        natus::concurrent::lock_guard_t lk( _configures_mtx ) ;
        _configures[_configures_id].push_back( [=] ( natus::graphics::backend_ptr_t be ) mutable
        {
            auto const ires = be->release( std::move( obj ) ) ;
            if( res.is_valid() ) *res = ires ;
        } ) ;
    }

    return *this ;
}

async::this_ref_t async::release( natus::graphics::framebuffer_object_res_t obj, natus::graphics::result_res_t res ) noexcept
{
    {
        natus::concurrent::lock_guard_t lk( _configures_mtx ) ;
        _configures[_configures_id].push_back( [=] ( natus::graphics::backend_ptr_t be ) mutable
        {
            auto const ires = be->release( std::move( obj ) ) ;
            if( res.is_valid() ) *res = ires ;
        } ) ;
    }

    return *this ;
}

async::this_ref_t async::release( natus::graphics::state_object_res_t obj, natus::graphics::result_res_t res ) noexcept
{
    {
        natus::concurrent::lock_guard_t lk( _configures_mtx ) ;
        _configures[_configures_id].push_back( [=] ( natus::graphics::backend_ptr_t be ) mutable
        {
            auto const ires = be->release( std::move( obj ) ) ;
            if( res.is_valid() ) *res = ires ;
        } ) ;
    }

    return *this ;
}

async::this_ref_t async::release( natus::graphics::array_object_res_t obj, natus::graphics::result_res_t res ) noexcept
{
    {
        natus::concurrent::lock_guard_t lk( _configures_mtx ) ;
        _configures[_configures_id].push_back( [=] ( natus::graphics::backend_ptr_t be ) mutable
        {
            auto const ires = be->release( std::move( obj ) ) ;
            if( res.is_valid() ) *res = ires ;
        } ) ;
    }

    return *this ;
}

async::this_ref_t async::update( natus::graphics::geometry_object_res_t gs, 
    natus::graphics::result_res_t res ) noexcept 
{
    {
        natus::concurrent::lock_guard_t lk( _configures_mtx ) ;

        _configures[_configures_id].push_back( [=] ( natus::graphics::backend_ptr_t be ) mutable
        {
            auto const ires = be->update( std::move( gs ) ) ;
            if( res.is_valid() ) *res = ires ;
        } ) ;
    }

    return *this ;
}

//****
async::this_ref_t async::update( natus::graphics::array_object_res_t obj, 
    natus::graphics::result_res_t res ) noexcept 
{
    {
        natus::concurrent::lock_guard_t lk( _configures_mtx ) ;

        _configures[_configures_id].push_back( [=] ( natus::graphics::backend_ptr_t be ) mutable
        {
            auto const ires = be->update( std::move( obj ) ) ;
            if( res.is_valid() ) *res = ires ;
        } ) ;
    }

    return *this ;
}

//****
async::this_ref_t async::update( natus::graphics::image_object_res_t obj, 
    natus::graphics::result_res_t res ) noexcept 
{
    {
        natus::concurrent::lock_guard_t lk( _configures_mtx ) ;

        _configures[_configures_id].push_back( [=] ( natus::graphics::backend_ptr_t be ) mutable
        {
            auto const ires = be->update( std::move( obj ) ) ;
            if( res.is_valid() ) *res = ires ;
        } ) ;
    }

    return *this ;
}

//****
async::this_ref_t async::use( natus::graphics::framebuffer_object_res_t fb,
    natus::graphics::result_res_t res ) noexcept 
{
    {
        natus::concurrent::lock_guard_t lk( _runtimes_mtx ) ;

        _runtimes[_runtimes_id].push_back( [=] ( natus::graphics::backend_ptr_t be ) mutable
        {
            auto const ires = be->use( std::move( fb ) ) ;
            if( res.is_valid() ) *res = ires ;
        } ) ;
    }
    return *this ;
}

async::this_ref_t async::unuse( natus::graphics::backend::unuse_type const t, natus::graphics::result_res_t res ) noexcept
{
    {
        natus::concurrent::lock_guard_t lk( _runtimes_mtx ) ;

        _runtimes[_runtimes_id].push_back( [=] ( natus::graphics::backend_ptr_t be ) mutable
        {
            auto const ires = be->unuse( t ) ;
            if( res.is_valid() ) *res = ires ;
        } ) ;
    }
    return *this ;
}

//****
async::this_ref_t async::push( natus::graphics::state_object_res_t s, size_t const sid, bool_t const push, 
    natus::graphics::result_res_t res ) noexcept 
{
    natus::concurrent::lock_guard_t lk( _runtimes_mtx ) ;

    _runtimes[_runtimes_id].push_back( [=] ( natus::graphics::backend_ptr_t be ) mutable
    {
        auto const ires = be->push( std::move( s ), sid, push ) ;
        if( res.is_valid() ) *res = ires ;
    } ) ;
    return *this ;
}
//****
async::this_ref_t async::pop( natus::graphics::backend::pop_type const t, natus::graphics::result_res_t res ) noexcept
{
    natus::concurrent::lock_guard_t lk( _runtimes_mtx ) ;

    _runtimes[_runtimes_id].push_back( [=] ( natus::graphics::backend_ptr_t be ) mutable
    {
        auto const ires = be->pop( t ) ;
        if( res.is_valid() ) *res = ires ;
    } ) ;

    return *this ;
}

//****
async::this_ref_t async::render( natus::graphics::render_object_res_t obj, natus::graphics::backend::render_detail_cref_t detail,
    natus::graphics::result_res_t res ) noexcept
{
    {
        natus::concurrent::lock_guard_t lk( _configures_mtx ) ;

        _configures[_configures_id].push_back( [=] ( natus::graphics::backend_ptr_t be ) mutable
        {
            auto const ires = be->update( obj, detail.varset ) ;
            if( res.is_valid() ) *res = ires ;
        } ) ;
    }

    {
        natus::concurrent::lock_guard_t lk( _runtimes_mtx ) ;

        _runtimes[_runtimes_id].push_back( [=] ( natus::graphics::backend_ptr_t be ) mutable
        { 
            auto const ires = be->render( std::move( obj ), detail ) ;
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
        {
            natus::concurrent::lock_guard_t lk( _configures_mtx ) ;
            _configures_id = ++_configures_id % 2 ;
        }

        size_t const use_id = (_configures_id + 1) % 2 ;
        this_t::commands_t & coms = _configures[use_id] ;

        auto* bptr = _backend.get_sptr().get() ;
        for( auto& rtz : coms )
        {
            rtz( bptr ) ;
        }
        coms.clear() ;
    }

    // runtime functions
    {
        {
            natus::concurrent::lock_guard_t lk( _runtimes_mtx ) ;
            _runtimes_id = ++_runtimes_id % 2 ;
        }

        {
            natus::concurrent::lock_guard_t lk( _frame_mtx ) ;
            _num_enter = 0 ;
        }
        
        {
            auto const use_id = (_runtimes_id+1)%2 ;
            auto & coms = _runtimes[ use_id ] ;

            _backend->render_begin() ;

            auto* bptr = _backend.get_sptr().get() ;
            for( auto& rtz : coms )
            {
                rtz( bptr ) ;
            }

            _backend->render_end() ;

            coms.clear() ;
        }
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
