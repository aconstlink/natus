

#include "d3d11.h"
//#include "gl3_convert.h"

#include "../../buffer/vertex_buffer.hpp"
#include "../../buffer/index_buffer.hpp"

#include <natus/memory/global.h>
#include <natus/memory/guards/malloc_guard.hpp>
#include <natus/ntd/vector.hpp>
#include <natus/ntd/string/split.hpp>

#include <d3d11.h>

using namespace natus::graphics ;

namespace this_file
{
}

struct d3d11_backend::pimpl
{
    natus::graphics::backend_type const bt = natus::graphics::backend_type::d3d11 ;
    natus::graphics::d3d11_context_ptr_t _ctx ;

    pimpl( natus::graphics::d3d11_context_ptr_t ctx )
    {
        _ctx = ctx ;
    }

    void_t begin_frame( void_t )
    {
        #if 0
        glClearColor( 0.4f, 0.1f, 0.1f, 1.0f ) ;
        natus::ogl::error::check_and_log( natus_log_fn( "glClearColor" ) ) ;

        glClear( GL_COLOR_BUFFER_BIT ) ;
        natus::ogl::error::check_and_log( natus_log_fn( "glClear" ) ) ;

        // reset render states
        this_t::set_render_states( *natus::graphics::backend_t::default_render_states() ) ;

        glViewport( 0, 0, vp_width, vp_height ) ;
        natus::ogl::error::check_and_log( natus_log_fn( "glViewport" ) ) ;
        #endif
    }

    void_t end_frame( void_t )
    {
        #if 0
        glFlush() ;
        glFinish() ;
        #endif
    }
} ;

//************************************************************************************************
//
//
//************************************************************************************************

//****
d3d11_backend::d3d11_backend( natus::graphics::d3d11_context_ptr_t ctx ) noexcept : 
    backend( natus::graphics::backend_type::gl3 )
{
    _pimpl = natus::memory::global_t::alloc( pimpl( ctx ), 
        natus_log_fn("d3d11_backend::pimpl") ) ;

    _context = ctx ;
}

//****
d3d11_backend::d3d11_backend( this_rref_t rhv ) noexcept : backend( ::std::move( rhv ) )
{
    natus_move_member_ptr( _pimpl, rhv ) ;
    natus_move_member_ptr( _context, rhv ) ;
}

//****
d3d11_backend::~d3d11_backend( void_t ) 
{
    natus::memory::global_t::dealloc( _pimpl ) ;
}

//****
void_t d3d11_backend::set_window_info( window_info_cref_t wi ) noexcept 
{
    {
        //bool_t change = false ;
        if( wi.width != 0 )
        {
            //_pimpl->vp_width = GLsizei( wi.width ) ;
            
        }
        if( wi.height != 0 )
        {
            //_pimpl->vp_height = GLsizei( wi.height ) ;
            
        }
    }
}

//****
natus::graphics::result d3d11_backend::configure( natus::graphics::geometry_object_res_t gconf ) noexcept 
{
    natus::graphics::id_res_t id = gconf->get_id() ;

    #if 0
    if( id->is_not_valid( this_t::get_bid() ) )
    {
        id = natus::graphics::id_t( this_t::get_bid(),
            _pimpl->construct_geo( gconf->name(), *(gconf.get_sptr()) ) ) ;
    }

    {
        auto const res = _pimpl->update( id->get_oid( this_t::get_bid() ), gconf ) ;
        if( natus::core::is_not( res ) )
        {
            return natus::graphics::result::failed ;
        }
    }
    #endif

    return natus::graphics::result::ok ;
}

//****
natus::graphics::result d3d11_backend::configure( natus::graphics::render_object_res_t config ) noexcept 
{
    natus::graphics::id_res_t id = config->get_id() ;

    #if 0
    {
        id = natus::graphics::id_t( this_t::get_bid(),
            _pimpl->construct_render_config( id->get_oid( this_t::get_bid() ), config->name(), *config ) ) ;
    }

    size_t const oid = id->get_oid( this_t::get_bid() ) ;

    {
        auto const res = _pimpl->update( oid, *config ) ;
        if( natus::core::is_not( res ) )
        {
            return natus::graphics::result::failed ;
        }
    }
    #endif

    return natus::graphics::result::ok ;
}

//***
natus::graphics::result d3d11_backend::configure( natus::graphics::shader_object_res_t config ) noexcept
{
    natus::graphics::id_res_t id = config->get_id() ;

    #if 0
    {
        id = natus::graphics::id_t( this_t::get_bid(),
            _pimpl->construct_shader_config( id->get_oid( this_t::get_bid() ), config->name(), *config ) ) ;
    }

    size_t const oid = id->get_oid( this_t::get_bid() ) ;

    {
        auto const res = _pimpl->update( oid, *config ) ;
        if( natus::core::is_not( res ) )
        {
            return natus::graphics::result::failed ;
        }
    }
    #endif
    return natus::graphics::result::ok ;
}

//***
natus::graphics::result d3d11_backend::configure( natus::graphics::image_object_res_t config ) noexcept 
{
    natus::graphics::id_res_t id = config->get_id() ;

    #if 0
    {
        id = natus::graphics::id_t( this_t::get_bid(), _pimpl->construct_image_config( 
            id->get_oid( this_t::get_bid() ), config->name(), *config ) ) ;
    }

    size_t const oid = id->get_oid( this_t::get_bid() ) ;

    {
        auto const res = _pimpl->update( oid, *config ) ;
        if( natus::core::is_not( res ) )
        {
            return natus::graphics::result::failed ;
        }
    }
    #endif
    return natus::graphics::result::ok ;
}

//***
natus::graphics::result d3d11_backend::connect( natus::graphics::render_object_res_t config, natus::graphics::variable_set_res_t vs ) noexcept
{
    natus::graphics::id_res_t id = config->get_id() ;

    #if 0
    if( id->is_not_valid( this_t::get_bid() ) )
    {
        natus::log::global_t::error( natus_log_fn( "invalid render configuration id" ) ) ;
        return natus::graphics::result::failed ;
    }

    size_t const oid = id->get_oid( this_t::get_bid() ) ;

    auto const res = _pimpl->connect( oid, vs ) ;
    natus::log::global_t::error( natus::graphics::is_not( res ), 
        natus_log_fn( "connect variable set" ) ) ;
   
    #endif

    return natus::graphics::result::ok ;
}

//****
natus::graphics::result d3d11_backend::update( natus::graphics::geometry_object_res_t config ) noexcept 
{
    natus::graphics::id_res_t id = config->get_id() ;

    #if 0
    if( id->is_not_valid( this_t::get_bid() ) )
    {
        natus::log::global_t::error( natus_log_fn( "invalid geometry configuration id" ) ) ;
        return natus::graphics::result::failed ;
    }

    size_t const oid = id->get_oid( this_t::get_bid() ) ;

    auto const res = _pimpl->update( oid, config ) ;
    natus::log::global_t::error( natus::graphics::is_not( res ),
        natus_log_fn( "update geometry" ) ) ;
    #endif

    return natus::graphics::result::ok ;
}

//****
natus::graphics::result d3d11_backend::render( natus::graphics::render_object_res_t config, natus::graphics::backend::render_detail_cref_t detail ) noexcept 
{ 
    natus::graphics::id_res_t id = config->get_id() ;

    //natus::log::global_t::status( natus_log_fn("render") ) ;
    #if 0
    if( id->is_not_bid( this_t::get_bid() ) || id->is_not_valid() )
    {
        natus::log::global_t::error( natus_log_fn( "invalid id" ) ) ;
        return natus::graphics::result::failed ;
    }

    if( detail.render_states.is_valid() )
    {
        _pimpl->set_render_states( *( detail.render_states ) ) ;
    }

    _pimpl->render( id->get_oid(), detail.varset, (GLsizei)detail.start, (GLsizei)detail.num_elems ) ;
    #endif
    return natus::graphics::result::ok ;
}

//****
void_t d3d11_backend::render_begin( void_t ) noexcept 
{
    _pimpl->begin_frame() ;
}

void_t d3d11_backend::render_end( void_t ) noexcept 
{
    _pimpl->end_frame() ;
}