

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
natus::graphics::result null_backend::configure( natus::graphics::geometry_object_res_t ) noexcept
{
    //static size_t number = 0 ;
    return natus::graphics::result::ok ;
}

//***
natus::graphics::result null_backend::configure( natus::graphics::render_object_res_t ) noexcept 
{
    //static size_t number = 0 ;
    return natus::graphics::result::ok ;
}

//***
natus::graphics::result null_backend::configure( natus::graphics::shader_object_res_t ) noexcept 
{
    //static size_t number = 0 ;
    return natus::graphics::result::ok ;
}

//***
natus::graphics::result null_backend::configure( natus::graphics::image_object_res_t ) noexcept 
{
    // static size_t number = 0 ;
    return natus::graphics::result::ok ;
}

//***
natus::graphics::result null_backend::configure( natus::graphics::framebuffer_object_res_t ) noexcept 
{
    return natus::graphics::result::ok ;
}

natus::graphics::result null_backend::configure( natus::graphics::state_object_res_t ) noexcept 
{
    return natus::graphics::result::ok ;
}

natus::graphics::result null_backend::configure( natus::graphics::array_object_res_t ) noexcept 
{
    return natus::graphics::result::ok ;
}

//***
natus::graphics::result null_backend::connect( natus::graphics::render_object_res_t, natus::graphics::variable_set_res_t ) noexcept 
{
    return natus::graphics::result::ok ;
}

//***
natus::graphics::result null_backend::update( natus::graphics::geometry_object_res_t ) noexcept 
{
    return natus::graphics::result::ok ;
}

natus::graphics::result null_backend::update( natus::graphics::array_object_res_t ) noexcept
{
    return natus::graphics::result::ok ;
}

natus::graphics::result null_backend::update( natus::graphics::image_object_res_t ) noexcept
{
    return natus::graphics::result::ok ;
}

natus::graphics::result null_backend::update( natus::graphics::render_object_res_t, size_t const varset ) noexcept 
{
    return natus::graphics::result::ok ;
}

//****
natus::graphics::result null_backend::use( natus::graphics::framebuffer_object_res_t ) noexcept
{
    return natus::graphics::result::ok ;
}

//****
natus::graphics::result null_backend::unuse( natus::graphics::backend::unuse_type const ) noexcept
{
    return natus::graphics::result::ok ;
}

//****
natus::graphics::result null_backend::push( natus::graphics::state_object_res_t, size_t const, bool_t const ) noexcept 
{
    return natus::graphics::result::ok ;
}

//****
natus::graphics::result null_backend::pop( pop_type const ) noexcept 
{
    return natus::graphics::result::ok ;
}

//***
natus::graphics::result null_backend::render( natus::graphics::render_object_res_t, natus::graphics::backend::render_detail_cref_t ) noexcept 
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
