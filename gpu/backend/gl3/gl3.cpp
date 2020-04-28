

#include "gl3.h"

#include "../../buffer/vertex_buffer.hpp"
#include "../../buffer/index_buffer.hpp"

#include <natus/ogl/gl/gl.h>
#include <natus/ogl/gl/error.hpp>
#include <natus/memory/global.h>
#include <natus/std/vector.hpp>

using namespace natus::gpu ;

namespace this_file
{
    struct render_config
    {
        bool_t valid = false ;
    };

    struct geo_config
    {
        GLuint vb_id = GLuint( -1 ) ;
        GLuint ib_id = GLuint( -1 ) ;
        
        void_ptr_t vb_mem = nullptr ;
        void_ptr_t ib_mem = nullptr ;
    };
}

struct gl3_backend::pimpl
{
    typedef natus::std::vector< this_file::render_config > rconfigs_t ;
    rconfigs_t rconfigs ;

    typedef natus::std::vector< this_file::geo_config > geo_configs_t ;
    geo_configs_t geo_configs ;

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

    size_t construct_geo( void_t ) 
    {
        size_t i = 0 ;
        for( i; i < geo_configs.size(); ++i )
        {
            if( natus::core::is_not( geo_configs[ i ].vb_id != GLuint(-1) ) )
            {
                break ;
            }
        }

        if( i == geo_configs.size() ) {
            geo_configs.resize( i + 1 ) ;
        }

        bool_t error = false ;

        // vertex buffer
        {
            GLuint id = GLuint( -1 ) ;
            natus::ogl::gl::glGenBuffers( 1, &id ) ;
            error = natus::ogl::error::check_and_log( 
                natus_log_fn( "Vertex Buffer creation" ) ) ;
            
            geo_configs[ i ].vb_id = id ;
        }

        // index buffer
        {
            GLuint id = GLuint( -1 ) ;
            natus::ogl::gl::glGenBuffers( 1, &id ) ;
            error = natus::ogl::error::check_and_log(
                natus_log_fn( "Index Buffer creation" ) ) ;
            
            geo_configs[ i ].ib_id = id ;
        }

        return i ;
    }

    bool_t configure_geo( size_t const id, natus::gpu::geometry_configuration_cref_t geo )
    {
        auto& config = geo_configs[ id ] ;

        // bind vertex buffer
        {
            natus::ogl::gl::glBindBuffer( GL_ARRAY_BUFFER, config.vb_id ) ;
            if( natus::ogl::error::check_and_log( natus_log_fn("glBindBuffer - vertex buffer") ) )
                return false ;
        }

        // allocate buffer memory
        // what about mapped memory?
        {
            GLuint const sib = GLuint( geo.vertex_buffer().get_sib() ) ;
            natus::ogl::gl::glBufferData( GL_ARRAY_BUFFER, sib, nullptr, GL_STATIC_DRAW ) ;
            if( natus::ogl::error::check_and_log( natus_log_fn( "glBufferData - vertex buffer" ) ) )
                return false ;
        }

        // bind index buffer
        {
            natus::ogl::gl::glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, config.ib_id ) ;
            if( natus::ogl::error::check_and_log( natus_log_fn( "glBindBuffer - index buffer" ) ) )
                return false ;
        }

        // allocate buffer memory
        // what about mapped memory?
        {
            GLuint const sib = GLuint( geo.index_buffer().get_sib() ) ;
            natus::ogl::gl::glBufferData( GL_ELEMENT_ARRAY_BUFFER, sib, nullptr, GL_STATIC_DRAW ) ;
            if( natus::ogl::error::check_and_log( natus_log_fn( "glBufferData - index buffer" ) ) )
                return false ;
        }

        return true ;
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
natus::gpu::id_t gl3_backend::configure( id_rref_t id,
    natus::gpu::geometry_configuration_in_t gconf ) noexcept 
{
    if( id.is_not_valid() )
    {
        id = natus::gpu::id_t( this_t::get_bid(),
            _pimpl->construct_geo() ) ;
    }

    if( id.is_not_bid( this_t::get_bid() ) )
    {
        natus::log::global_t::error( natus_log_fn( "false backend" ) ) ;
        return ::std::move( id ) ;
    }

    {
        auto const res = _pimpl->configure_geo( id.get_oid(), gconf ) ;
        if( natus::core::is_not( res ) )
        {
            return ::std::move( id ) ;
        }
    }

    return ::std::move( id ) ;
}

//****
natus::gpu::id_t gl3_backend::configure( natus::gpu::render_configurations_in_t rc ) noexcept
{
    return this_t::configure( natus::gpu::id_t(), rc ) ;
}

//****
natus::gpu::id_t gl3_backend::configure( id_rref_t id, natus::gpu::render_configurations_in_t ) noexcept 
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
