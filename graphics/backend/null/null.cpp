

#include "null.h"

using namespace natus::graphics ;

//***
null_backend::null_backend( void_t ) {}
null_backend::null_backend( this_rref_t rhv ) : backend( ::std::move( rhv ) ){}
null_backend::~null_backend( void_t ) {}

//****
void_t null_backend::set_window_info( window_info_cref_t ) noexcept
{
}

//***
natus::graphics::result null_backend::configure( natus::graphics::geometry_configuration_res_t ) noexcept
{
    //static size_t number = 0 ;
    return natus::graphics::result::ok ;
}

//***
natus::graphics::result null_backend::configure( natus::graphics::render_configuration_res_t ) noexcept 
{
    //static size_t number = 0 ;
    return natus::graphics::result::ok ;
}

//***
natus::graphics::result null_backend::configure( natus::graphics::shader_configuration_res_t ) noexcept 
{
    //static size_t number = 0 ;
    return natus::graphics::result::ok ;
}

//***
natus::graphics::result null_backend::configure( natus::graphics::image_configuration_res_t ) noexcept 
{
    // static size_t number = 0 ;
    return natus::graphics::result::ok ;
}

//***
natus::graphics::result null_backend::connect( natus::graphics::render_configuration_res_t, natus::graphics::variable_set_res_t ) noexcept 
{
    return natus::graphics::result::ok ;
}

//***
natus::graphics::result null_backend::update( natus::graphics::geometry_configuration_res_t ) noexcept 
{
    return natus::graphics::result::ok ;
}

//***
natus::graphics::result null_backend::render( natus::graphics::render_configuration_res_t, natus::graphics::backend::render_detail_cref_t ) noexcept 
{
    return natus::graphics::result::ok ;
}

//***
void_t null_backend::render_begin( void_t ) noexcept 
{
}

//***
void_t null_backend::render_end( void_t ) noexcept 
{
}
