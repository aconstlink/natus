

#include "vdev_module.h"

#include "../mapping.hpp"

#include <natus/device/layouts/xbox_controller.hpp>
#include <natus/device/layouts/game_controller.hpp>
#include <natus/device/layouts/ascii_keyboard.hpp>
#include <natus/device/layouts/three_mouse.hpp>


using namespace natus::device ;

//***
vdev_module::vdev_module( void_t ) 
{
    for( size_t i=0; i<4; ++i )
    {
        this_t::data d ;
        d.dev = natus::device::game_device_t( "logical game controller " + ::std::to_string(i) ) ;

        _games.emplace_back( d ) ;
    }
}

//***
vdev_module::vdev_module( this_rref_t rhv ) 
{
    _games = ::std::move( rhv._games ) ;
}

vdev_module::~vdev_module( void_t )
{
}

//***
vdev_module::this_ref_t vdev_module::operator = ( this_rref_t rhv )
{
    _games = ::std::move( rhv._games ) ;
    return *this ;
}

//***
void_t vdev_module::search( natus::device::imodule::search_funk_t funk )
{
    for( auto & g : _games )
    {
        funk( g.dev ) ;
    }
}

//***
void_t vdev_module::update( void_t )
{
    for( auto & g : _games )
    {
        g.dev->update() ;
    }
}

void_t vdev_module::release( void_t ) noexcept 
{
    _games.clear() ;
}

//***
void_t vdev_module::search( this_t::mapping_searach_ft )
{
}

//***
void_t vdev_module::check_devices( natus::device::imodule_res_t mod ) 
{
    this_t::init_controller_1( mod ) ;
}

//***
void_t vdev_module::init_controller_1( natus::device::imodule_res_t mod )
{
    this_t::data & g = _games[ 0 ] ;

    {
        natus::device::xbc_device_res_t xbc_dev ;
        natus::ntd::string_t const name = "[system] : xbox" ;

        bool_t have_xbox = false ;
        g.dev->search( [&] ( natus::device::imapping_res_t r )
        {
            if( r->name() == name ) have_xbox = true ;
        } ) ;

        mod->search( [&] ( natus::device::idevice_res_t dev_in )
        {
            if( natus::device::xbc_device_res_t::castable( dev_in ) )
            {
                if( xbc_dev.is_valid() ) return ;

                xbc_dev = dev_in ;
            }
        } ) ;

        // do mappings for xbox
        if( natus::core::is_not( have_xbox ) && xbc_dev.is_valid() )
        {
            using a_t = natus::device::game_device_t ;
            using b_t = natus::device::xbc_device_t ;

            using ica_t = a_t::layout_t::input_component ;
            using icb_t = b_t::layout_t::input_component ;

            using mapping_t = natus::device::mapping< a_t, b_t > ;
            mapping_t m( name, g.dev, xbc_dev ) ;

            {
                auto const res = m.insert( ica_t::jump, icb_t::button_a ) ;
                natus::log::global_t::warning( natus::core::is_not( res ), "can not do mapping." ) ;
            }
            {
                auto const res = m.insert( ica_t::shoot, icb_t::button_b ) ;
                natus::log::global_t::warning( natus::core::is_not( res ), "can not do mapping." ) ;
            }
            {
                auto const res = m.insert( ica_t::action_a, icb_t::button_x ) ;
                natus::log::global_t::warning( natus::core::is_not( res ), "can not do mapping." ) ;
            }
            {
                auto const res = m.insert( ica_t::action_b, icb_t::button_y ) ;
                natus::log::global_t::warning( natus::core::is_not( res ), "can not do mapping." ) ;
            }
            {
                auto const res = m.insert( ica_t::aim, icb_t::stick_right ) ;
                natus::log::global_t::warning( natus::core::is_not( res ), "can not do mapping." ) ;
            }
            {
                auto const res = m.insert( ica_t::movement, icb_t::stick_left ) ;
                natus::log::global_t::warning( natus::core::is_not( res ), "can not do mapping." ) ;
            }

            g.dev->add_mapping( natus::memory::res_t<mapping_t>( m ) ) ;
        }
    }

    {
        natus::device::ascii_device_res_t ascii_dev ;
        natus::ntd::string_t const name = "[system] : ascii" ;

        bool_t have_ascii = false ;
        g.dev->search( [&] ( natus::device::imapping_res_t r )
        {
            if( r->name() == name ) have_ascii = true ;
        } ) ;

        mod->search( [&] ( natus::device::idevice_res_t dev_in )
        {
            if( natus::device::ascii_device_res_t::castable( dev_in ) )
            {
                if( ascii_dev.is_valid() ) return ;

                ascii_dev = dev_in ;
            }
        } ) ;

        // do mappings for ascii
        if( natus::core::is_not( have_ascii ) && ascii_dev.is_valid() )
        {
            using a_t = natus::device::game_device_t ;
            using b_t = natus::device::ascii_device_t ;

            using ica_t = a_t::layout_t::input_component ;
            //using icb_t = b_t::layout_t::input_component ;

            using mapping_t = natus::device::mapping< a_t, b_t > ;
            mapping_t m( name, g.dev, ascii_dev ) ;

            {
                auto const res = m.insert( ica_t::jump,
                    b_t::layout_t::ascii_key_to_input_component( b_t::layout_t::ascii_key::space ) ) ;
                natus::log::global_t::warning( natus::core::is_not( res ), "can not do mapping." ) ;
            }

            {
                auto const res = m.insert( ica_t::movement,
                    b_t::layout_t::ascii_key_to_input_component( b_t::layout_t::ascii_key::a ),
                    natus::device::mapping_detail::negative_x ) ;
                natus::log::global_t::warning( natus::core::is_not( res ), "can not do mapping." ) ;
            }

            {
                auto const res = m.insert( ica_t::movement,
                    b_t::layout_t::ascii_key_to_input_component( b_t::layout_t::ascii_key::d ),
                    natus::device::mapping_detail::positive_x ) ;
                natus::log::global_t::warning( natus::core::is_not( res ), "can not do mapping." ) ;
            }

            {
                auto const res = m.insert( ica_t::movement,
                    b_t::layout_t::ascii_key_to_input_component( b_t::layout_t::ascii_key::w ),
                    natus::device::mapping_detail::positive_y ) ;
                natus::log::global_t::warning( natus::core::is_not( res ), "can not do mapping." ) ;
            }

            {
                auto const res = m.insert( ica_t::movement,
                    b_t::layout_t::ascii_key_to_input_component( b_t::layout_t::ascii_key::s ),
                    natus::device::mapping_detail::negative_y ) ;
                natus::log::global_t::warning( natus::core::is_not( res ), "can not do mapping." ) ;
            }


            g.dev->add_mapping( natus::memory::res_t<mapping_t>( m ) ) ;
        }
    }

    // mouse
    {
        natus::device::three_device_res_t three_dev ;
        natus::ntd::string_t const name = "[system] : mouse" ;

        bool_t have_mouse = false ;
        g.dev->search( [&] ( natus::device::imapping_res_t r )
        {
            if( r->name() == name ) have_mouse = true ;
        } ) ;

        mod->search( [&] ( natus::device::idevice_res_t dev_in )
        {
            if( natus::device::three_device_res_t::castable( dev_in ) )
            {
                if( three_dev.is_valid() ) return ;

                three_dev = dev_in ;
            }
        } ) ;

        // do mappings for mouse
        if( natus::core::is_not( have_mouse ) && three_dev.is_valid() )
        {
            using a_t = natus::device::game_device_t ;
            using b_t = natus::device::three_device_t ;

            using ica_t = a_t::layout_t::input_component ;
            using icb_t = b_t::layout_t::input_component ;

            using mapping_t = natus::device::mapping< a_t, b_t > ;
            mapping_t m( name, g.dev, three_dev ) ;

            {
                auto const res = m.insert( ica_t::aim, icb_t::local_coords ) ;
                natus::log::global_t::warning( natus::core::is_not( res ), "can not do mapping." ) ;
            }

            g.dev->add_mapping( natus::memory::res_t<mapping_t>( m ) ) ;
        }
    }
}

//***
