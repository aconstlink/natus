
#include "primitive_render_3d.h"

using namespace natus::gfx ;

primitive_render_3d::primitive_render_3d( void_t ) 
{
   
}

primitive_render_3d::primitive_render_3d( this_rref_t rhv ) 
{
    _lr = std::move( rhv._lr ) ;
    _tr = std::move( rhv._tr ) ;
}
            
primitive_render_3d::~primitive_render_3d( void_t ) 
{
}

void_t primitive_render_3d::init( natus::ntd::string_cref_t name, natus::graphics::async_views_t asyncs ) noexcept 
{
    _name = name ;
    _asyncs = asyncs ;

    _lr = natus::gfx::line_render_3d_res_t( natus::gfx::line_render_3d_t() ) ;
    _lr->init( name + ".line_render", _asyncs ) ;

    _tr = natus::gfx::tri_render_3d_res_t( natus::gfx::tri_render_3d_t() ) ;
    _tr->init( name + ".tri_render", _asyncs ) ;
}

void_t primitive_render_3d::release( void_t ) noexcept 
{
    _lr->release() ;
    _tr->release() ;
}

void_t primitive_render_3d::draw_line( natus::math::vec3f_cref_t p0, natus::math::vec3f_cref_t p1, natus::math::vec4f_cref_t color ) noexcept
{
    _lr->draw( p0, p1, color ) ;
}

void_t primitive_render_3d::draw_tri( natus::math::vec3f_cref_t p0, natus::math::vec3f_cref_t p1, natus::math::vec3f_cref_t p2, natus::math::vec4f_cref_t color ) noexcept 
{
    _tr->draw( p0, p1, p2, color ) ;
}
#if 0
void_t primitive_render_3d::draw_rect(  natus::math::vec3f_cref_t p0, natus::math::vec3f_cref_t p1, natus::math::vec3f_cref_t p2, natus::math::vec3f_cref_t p3, natus::math::vec4f_cref_t color, 
    natus::math::vec4f_cref_t border_color ) noexcept 
{
    _tr->draw_rect( p0, p1, p2, p3, color ) ;
    _lr->draw_rect( p0, p1, p2, p3, border_color ) ;
}
#endif
void_t primitive_render_3d::draw_circle( natus::math::mat3f_cref_t o, natus::math::vec3f_cref_t p0, float_t const r, natus::math::vec4f_cref_t color, 
    natus::math::vec4f_cref_t border_color, size_t const s) noexcept 
{
    _tr->draw_circle( o, p0, r, color, s ) ;
    //_lr->draw_circle( s, p0, r, border_color ) ;
}


void_t primitive_render_3d::prepare_for_rendering( void_t ) noexcept 
{
    _lr->prepare_for_rendering() ;
    _tr->prepare_for_rendering() ;
}

void_t primitive_render_3d::render( void_t ) noexcept 
{
    _lr->render() ;
    _tr->render() ;
}

void_t primitive_render_3d::set_view_proj( natus::math::mat4f_cref_t view, natus::math::mat4f_cref_t proj ) noexcept 
{
    _tr->set_view_proj( view, proj ) ;
    _lr->set_view_proj( view, proj ) ;
}