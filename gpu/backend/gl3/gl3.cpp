

#include "gl3.h"

#include <natus/ogl/gl/gl.h>
#include <natus/memory/global.h>

using namespace natus::gpu ;

struct gl3_backend::pimpl
{
    pimpl( void_t ) 
    {

    }
};

//****
gl3_backend::gl3_backend( void_t ) noexcept
{
    _pimpl = natus::memory::global_t::alloc( pimpl(), 
        natus_log_fn("gl3_backend::pimpl") ) ;
}

//****
gl3_backend::gl3_backend( this_rref_t rhv ) noexcept : backend( ::std::move( rhv ) )
{
    natus_move_member_ptr( _pimpl, rhv ) ;
}

//****
gl3_backend::~gl3_backend( void_t ) 
{
    if( natus::core::is_not_nullptr( _pimpl ) )
        natus::memory::global_t::dealloc( _pimpl ) ;
}

//****
natus::gpu::id_t gl3_backend::prepare( natus::gpu::render_configuration_in_t rc ) noexcept
{
    return this_t::prepare( natus::gpu::id_t(), rc ) ;
}

//****
natus::gpu::id_t gl3_backend::prepare( id_rref_t id, natus::gpu::render_configuration_in_t ) noexcept 
{
    if( id.is_not_valid() )
    {
        // create new id
    }
    return natus::gpu::id_t() ;
}

//****
natus::gpu::id_t gl3_backend::render( id_rref_t id ) noexcept 
{
    return natus::gpu::id_t() ;
}
