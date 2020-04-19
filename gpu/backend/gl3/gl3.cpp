

#include "gl3.h"

#include <natus/ogl/gl/gl.h>
#include <natus/memory/global.h>
#include <natus/std/vector.hpp>

using namespace natus::gpu ;

namespace this_file
{
    struct render_config
    {
        bool_t valid = false ;
    };
}

struct gl3_backend::pimpl
{
    typedef natus::std::vector< this_file::render_config > rconfigs_t ;
    rconfigs_t rconfigs ;

    pimpl( void_t ) 
    {}

    size_t construct_rconfig( void_t )  
    {
        size_t i = 0 ;
        for( i; i<rconfigs.size(); ++i )
        {
            if( natus::core::is_not( rconfigs[i].valid ) )
            {
                break ;
            }
        }

        if( i == rconfigs.size() ) {
            rconfigs.resize( i + 1 ) ;
        }
        return i ;
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
natus::gpu::id_t gl3_backend::prepare( natus::gpu::render_configurations_in_t rc ) noexcept
{
    return this_t::prepare( natus::gpu::id_t(), rc ) ;
}

//****
natus::gpu::id_t gl3_backend::prepare( id_rref_t id, natus::gpu::render_configurations_in_t ) noexcept 
{
    if( id.is_not_valid() )
    {
        id = natus::gpu::id_t( this_t::get_bid(),
            _pimpl->construct_rconfig() ) ;
    }

    if( id.is_not_bid( this_t::get_bid() ) )
    {
        natus::log::global_t::error( natus_log_fn( "invalid id" ) ) ;
        return ::std::move( id ) ;
    }

    size_t const oid = id.get_oid() ;

    natus::log::global_t::status( natus_log_fn("prepare") ) ;

    return ::std::move( id ) ;
}

//****
natus::gpu::id_t gl3_backend::render( id_rref_t id ) noexcept 
{ 
    natus::log::global_t::status( natus_log_fn("render") ) ;

    return ::std::move( id ) ;
}
