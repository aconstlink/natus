

#include "null.h"

using namespace natus::gpu ;

//***
null_backend::null_backend( void_t ) {}
null_backend::null_backend( this_rref_t rhv ) : backend( ::std::move( rhv ) ){}
null_backend::~null_backend( void_t ) {}

//****
void_t null_backend::set_window_info( window_info_cref_t ) noexcept
{
}

//***
natus::gpu::id_t null_backend::configure( id_rref_t id, natus::gpu::geometry_configuration_res_t ) noexcept
{
    static size_t number = 0 ;

    natus::gpu::id_t ret = ::std::move( id ) ;
    if( ret.is_not_valid() )
        ret = natus::gpu::id_t( this_t::get_bid(), ++number ) ;

    return ::std::move( ret ) ;
}

//***
natus::gpu::result null_backend::configure( natus::gpu::render_configuration_res_t ) noexcept 
{
    static size_t number = 0 ;

    return natus::gpu::result::ok ;
}

//***
natus::gpu::id_t null_backend::configure( id_rref_t id, natus::gpu::shader_configuration_res_t ) noexcept 
{
    static size_t number = 0 ;

    natus::gpu::id_t ret = ::std::move( id ) ;
    if( ret.is_not_valid() )
        ret = natus::gpu::id_t( this_t::get_bid(), ++number ) ;

    return ::std::move( ret ) ;
}

//***
natus::gpu::id_t null_backend::connect( id_rref_t id, natus::gpu::variable_set_res_t ) noexcept 
{
    return ::std::move( id ) ;
}

//***
natus::gpu::id_t null_backend::update( id_rref_t id, natus::gpu::geometry_configuration_res_t ) noexcept 
{
    return ::std::move( id ) ;
}

//***
natus::gpu::result null_backend::render( natus::gpu::render_configuration_res_t, natus::gpu::backend::render_detail_cref_t ) noexcept 
{
    return natus::gpu::result::ok ;
}

//***
void_t null_backend::render_begin( void_t ) noexcept 
{
}

//***
void_t null_backend::render_end( void_t ) noexcept 
{
}