
#include "async.h"

using namespace natus::gpu ;

//****
async::async( void_t ) 
{}

//****
async::async( backend_rptr_t rptr ) :_backend( rptr ) 
{}

//****
async::async( this_rref_t rhv ) 
{
    _backend = ::std::move( rhv._backend ) ;
    _renders = ::std::move( rhv._renders ) ;
    _prepares = ::std::move( rhv._prepares ) ;
}

//****
async::~async( void_t ) 
{}

//****
natus::gpu::result async::prepare( natus::gpu::async_id_rptr_t aid, natus::gpu::render_configurations_in_t rc ) noexcept 
{
    _prepares.push_back( prepare_data( {aid, rc } ) ) ;
    return natus::gpu::result::ok ;
}

//****
natus::gpu::result async::render( natus::gpu::async_id_rptr_t aid ) noexcept 
{
    _renders.push_back( { aid } ) ;
    return natus::gpu::result::ok ;
}

//****
void_t async::update( void_t ) noexcept 
{
    for( auto & prc : _prepares )
    {
        natus::gpu::id_t id = _backend->prepare( prc.aid->id(), ::std::move( prc.config ) ) ;
        id = prc.aid->set( ::std::move( id ), natus::gpu::result::ok ) ;
        natus::log::global_t::error( id.is_valid(), natus_log_fn(
            "gpu resource need to be released." ) ) ;
    }

    _prepares.clear() ;

    for( auto & rnd : _renders )
    {
        natus::gpu::id_t id = _backend->render( rnd.aid->id() ) ;

        id = rnd.aid->set( ::std::move( id ), natus::gpu::result::ok ) ;
        natus::log::global_t::error( id.is_valid(), natus_log_fn(
            "gpu resource need to be released." ) ) ;
    }

    _renders.clear() ;
}