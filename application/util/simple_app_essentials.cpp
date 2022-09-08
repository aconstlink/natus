#include "simple_app_essentials.h"

#include <natus/property/property_sheet.hpp>

#include <natus/format/global.h>
#include <natus/format/nsl/nsl_module.h>
#include <natus/format/future_items.hpp>

using namespace natus::application ;
using namespace natus::application::util ;

//****************************************************************
simple_app_essentials::simple_app_essentials( void_t ) noexcept 
{
}

//****************************************************************
simple_app_essentials::simple_app_essentials( this_rref_t rhv ) noexcept 
{
    *this = std::move( rhv ) ;
}

//****************************************************************
simple_app_essentials::~simple_app_essentials( void_t ) noexcept 
{
}

//****************************************************************
simple_app_essentials::this_ref_t simple_app_essentials::operator = ( this_rref_t rhv ) noexcept 
{
    _camera_0 = std::move( rhv._camera_0 ) ;
    _graphics = std::move( rhv._graphics ) ;
    _db = std::move( rhv._db ) ;
    _tr = std::move( rhv._tr ) ;
    _pr = std::move( rhv._pr ) ;
    _lr3 = std::move( rhv._lr3 ) ;
    _pr3 = std::move( rhv._pr3 ) ;
    _window_dims = std::move( rhv._window_dims ) ;

    return *this ;
}

//****************************************************************
void_t simple_app_essentials::init_font( void_t ) noexcept 
{
    // import fonts and create text render
    {
        natus::property::property_sheet_res_t ps = natus::property::property_sheet_t() ;

        {
            natus::font::code_points_t pts ;
            for( uint32_t i = 33; i <= 126; ++i ) pts.emplace_back( i ) ;
            for( uint32_t i : {uint32_t( 0x00003041 )} ) pts.emplace_back( i ) ;
            ps->set_value< natus::font::code_points_t >( "code_points", pts ) ;
        }

        {
            natus::ntd::vector< natus::io::location_t > locations = 
            {
                natus::io::location_t("fonts.mitimasu.ttf"),
                //natus::io::location_t("")
            } ;
            ps->set_value( "additional_locations", locations ) ;
        }

        {
            ps->set_value<size_t>( "atlas_width", 128 ) ;
            ps->set_value<size_t>( "atlas_height", 512 ) ;
            ps->set_value<size_t>( "point_size", 90 ) ;
        }

        natus::format::module_registry_res_t mod_reg = natus::format::global_t::registry() ;
        auto fitem = mod_reg->import_from( natus::io::location_t( "fonts.LCD_Solid.ttf" ), _db, ps ) ;
        natus::format::glyph_atlas_item_res_t ii = fitem.get() ;
        if( ii.is_valid() )
        {
            _tr = natus::gfx::text_render_2d_res_t( natus::gfx::text_render_2d_t( "my_text_render", _graphics ) ) ;
                    
            _tr->init( std::move( *ii->obj ) ) ;
            _has_font = true ;
        }
    }
}

//****************************************************************
void_t simple_app_essentials::init_graphics( natus::ntd::string_cref_t name, natus::graphics::async_views_t graphics ) noexcept 
{
    _graphics = graphics ;

    // prepare primitive
    {
        _pr = natus::gfx::primitive_render_2d_res_t( natus::gfx::primitive_render_2d_t() ) ;
        _pr->init( name + ".prim_render", _graphics ) ;
    }

    // prepare primitive
    {
        _pr3 = natus::gfx::primitive_render_3d_res_t( natus::gfx::primitive_render_3d_t() ) ;
        _pr3->init( name + ".prim_render_3d", _graphics ) ;
    }

    // prepare line render
    {
        _lr3 = natus::gfx::line_render_3d_res_t( natus::gfx::line_render_3d_t() ) ;
        _lr3->init( name + ".line_render", _graphics ) ;
    }

    //
    // why setting camera position two times?
    //
    {
        _camera_0.look_at( natus::math::vec3f_t( 0.0f, 100.0f, -1000.0f ),
                natus::math::vec3f_t( 0.0f, 1.0f, 0.0f ), natus::math::vec3f_t( 0.0f, 100.0f, 0.0f )) ;
    }

    _camera_0.set_transformation( natus::math::m3d::trafof_t( 1.0f, 
        natus::math::vec3f_t(0.0f,0.0f,0.0f), 
        natus::math::vec3f_t(0.0f,0.0f,-1000.0f) ) ) ;
}

//****************************************************************
void_t simple_app_essentials::init_data( natus::io::path_cref_t base, natus::io::path_cref_t rel, natus::io::path_cref_t name ) noexcept 
{
    _db = natus::io::database_t( base, rel, name ) ;
}

//****************************************************************
void_t simple_app_essentials::init_device( void_t ) noexcept 
{
    natus::device::global_t::system()->search( [&] ( natus::device::idevice_res_t dev_in )
    {
        if( natus::device::three_device_res_t::castable( dev_in ) )
        {
            _dev_mouse = dev_in ;
        }
        else if( natus::device::ascii_device_res_t::castable( dev_in ) )
        {
            _dev_ascii = dev_in ;
        }
    } ) ;

    if( !_dev_mouse.is_valid() )
    {
        natus::log::global_t::status( "no three mosue found" ) ;
    }

    if( !_dev_ascii.is_valid() )
    {
        natus::log::global_t::status( "no ascii keyboard found" ) ;
    }
}

//****************************************************************
void_t simple_app_essentials::on_event( natus::application::app::window_id_t const, natus::application::app::window_event_info_in_t wei ) noexcept 
{
    _window_dims = natus::math::vec2f_t( float_t(wei.w), float_t(wei.h) ) ;
    _camera_0.set_dims( float_t(wei.h), float_t(wei.w), _near, _far ) ;
    
}

//****************************************************************
void_t simple_app_essentials::on_device( natus::application::app::device_data_in_t ) noexcept 
{
    natus::device::layouts::ascii_keyboard_t ascii( _dev_ascii ) ;
    if( ascii.get_state( natus::device::layouts::ascii_keyboard_t::ascii_key::f8 ) ==
        natus::device::components::key_state::released )
    {
    }
    else if( ascii.get_state( natus::device::layouts::ascii_keyboard_t::ascii_key::f9 ) ==
        natus::device::components::key_state::released )
    {
    }
    else if( ascii.get_state( natus::device::layouts::ascii_keyboard_t::ascii_key::f2 ) ==
        natus::device::components::key_state::released )
    {
        _do_tool = !_do_tool ;
    }

    {
        natus::device::layouts::three_mouse_t mouse( _dev_mouse ) ;
        _cur_mouse = mouse.get_local() * natus::math::vec2f_t( 2.0f ) - natus::math::vec2f_t( 1.0f ) ;
        _cur_mouse = _cur_mouse * (_window_dims * natus::math::vec2f_t(0.5f) );
    }

    // rotate
    {
        natus::device::layouts::three_mouse_t mouse( _dev_mouse ) ;
        static natus::math::vec2f_t old = mouse.get_global() * natus::math::vec2f_t( 2.0f ) - natus::math::vec2f_t( 1.0f ) ;
        natus::math::vec2f_t const dif = (mouse.get_global()* natus::math::vec2f_t( 2.0f ) - natus::math::vec2f_t( 1.0f )) - old ;
        old = mouse.get_global() * natus::math::vec2f_t( 2.0f ) - natus::math::vec2f_t( 1.0f ) ;

        if( mouse.is_pressing(natus::device::layouts::three_mouse::button::right ) )
        {
            auto old2 = _camera_0.get_transformation() ;
            auto trafo = old2.rotate_by_angle_fr( natus::math::vec3f_t( -dif.y()*2.0f, dif.x()*2.0f, 0.0f ) ) ;
            _camera_0.set_transformation( trafo ) ;
        }
    }

    // translate
    {
        natus::math::vec3f_t translate ;

        if( ascii.get_state( natus::device::layouts::ascii_keyboard_t::ascii_key::a ) ==
            natus::device::components::key_state::pressing )
        {
            translate += natus::math::vec3f_t(-10.0f, 0.0f, 0.0f ) ;
        }

        if( ascii.get_state( natus::device::layouts::ascii_keyboard_t::ascii_key::s ) ==
            natus::device::components::key_state::pressing )
        {
            translate += natus::math::vec3f_t(0.0f, 0.0f, -10.0f) ;
        }

        if( ascii.get_state( natus::device::layouts::ascii_keyboard_t::ascii_key::d ) ==
            natus::device::components::key_state::pressing )
        {
            translate += natus::math::vec3f_t( 10.0f, 0.0f, 0.0f ) ;
        }

        if( ascii.get_state( natus::device::layouts::ascii_keyboard_t::ascii_key::w ) ==
            natus::device::components::key_state::pressing )
        {
            translate += natus::math::vec3f_t(0.0f, 0.0f, 10.0f ) ;
        }

        if( ascii.get_state( natus::device::layouts::ascii_keyboard_t::ascii_key::q ) ==
            natus::device::components::key_state::pressing )
        {
            translate += natus::math::vec3f_t(0.0f, -10.0f, 0.0f ) ;
        }

        if( ascii.get_state( natus::device::layouts::ascii_keyboard_t::ascii_key::e ) ==
            natus::device::components::key_state::pressing )
        {
            translate += natus::math::vec3f_t(0.0f, 10.0f, 0.0f ) ;
        }

        auto trafo = _camera_0.get_transformation() ;
        trafo.translate_fr( translate ) ;
        _camera_0.set_transformation( trafo ) ;
    }
}

//****************************************************************
void_t simple_app_essentials::on_graphics_begin( natus::application::app_t::render_data_in_t ) noexcept 
{
    _pr->set_view_proj( _camera_0.mat_view(), _camera_0.mat_proj() ) ;
    _lr3->set_view_proj( _camera_0.mat_view(), _camera_0.mat_proj() ) ;
    _pr3->set_view_proj( _camera_0.mat_view(), _camera_0.mat_proj() ) ;

    if( _has_font ) _tr->set_view_proj( _camera_0.mat_view(), _camera_0.mat_proj() ) ;
}

//****************************************************************
void_t simple_app_essentials::on_graphics_end( size_t const num_layers ) noexcept 
{
    // render all
    {
        if( _has_font ) _tr->prepare_for_rendering() ;

        _pr->prepare_for_rendering() ;
        _lr3->prepare_for_rendering() ;
        _pr3->prepare_for_rendering() ;

        _pr3->render() ;
        _lr3->render() ;

        for( size_t i=0; i<num_layers; ++i )
        {
            _pr->render( i ) ;
            if( _has_font ) _tr->render( i ) ;
        }
    }
}

//****************************************************************
void_t simple_app_essentials::on_tool( natus::application::app::tool_data_ref_t ) noexcept 
{
}

//****************************************************************