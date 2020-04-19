

#include "null.h"

using namespace natus::gpu ;

//***
null_backend::null_backend( void_t ) {}
null_backend::null_backend( this_rref_t rhv ) : backend( ::std::move( rhv ) ){}
null_backend::~null_backend( void_t ) {}

//***
natus::gpu::id_t null_backend::prepare( natus::gpu::render_configuration_in_t rc ) noexcept
{
    return this_t::prepare( natus::gpu::id_t(), rc ) ;
}

//***
natus::gpu::id_t null_backend::prepare( id_rref_t id, natus::gpu::render_configuration_in_t ) noexcept 
{
    static size_t number = 0 ;

    natus::gpu::id_t ret = ::std::move( id ) ;
    if( ret.is_not_valid() ) 
        ret = natus::gpu::id_t( this_t::get_bid(), ++number ) ;

    return ::std::move( ret ) ;
}

//***
natus::gpu::id_t null_backend::render( id_rref_t id ) noexcept 
{
    return ::std::move( id ) ;
}