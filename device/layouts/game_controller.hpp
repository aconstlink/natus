
#pragma once

#include "../typedefs.h"
#include "../components/button.hpp"
#include "../components/stick.hpp"
#include "../components/motor.hpp"

#include "../device.hpp"

namespace natus
{
    namespace device
    {
        namespace layouts
        {
            // logical device layout
            class game_controller
            {
                natus_this_typedefs( game_controller ) ;
                natus_res_typedefs( natus::device::device< this_t >, this_device ) ;

                friend this_device_t ;

            private:

                this_t::this_device_res_t _dev ;

            public:

                enum class input_component
                {
                    // 1d 
                    jump,
                    crouch,
                    shoot,
                    interact,

                    pause,
                    menu,
                    home,

                    action_a,
                    action_b,
                    action_c,
                    action_d,
                    action_e,
                    action_f,
                    action_g,
                    action_h,
                    action_i,

                    // 2d
                    movement,
                    aim,

                    num_components
                };

                enum class output_component
                {
                    vibration_a,
                    vibration_b,
                    num_components
                };

            private:

                static void_t init_components( this_t::this_device_ref_t dev )
                {
                    // inputs

                    // buttons
                    dev.add_input_component<natus::device::components::button_t>() ; // jump
                    dev.add_input_component<natus::device::components::button_t>() ;
                    dev.add_input_component<natus::device::components::button_t>() ;
                    dev.add_input_component<natus::device::components::button_t>() ;
                    dev.add_input_component<natus::device::components::button_t>() ;
                    dev.add_input_component<natus::device::components::button_t>() ;
                    dev.add_input_component<natus::device::components::button_t>() ;
                    dev.add_input_component<natus::device::components::button_t>() ;
                    dev.add_input_component<natus::device::components::button_t>() ;
                    dev.add_input_component<natus::device::components::button_t>() ;
                    dev.add_input_component<natus::device::components::button_t>() ;
                    dev.add_input_component<natus::device::components::button_t>() ;
                    dev.add_input_component<natus::device::components::button_t>() ;
                    dev.add_input_component<natus::device::components::button_t>() ;
                    dev.add_input_component<natus::device::components::button_t>() ;
                    dev.add_input_component<natus::device::components::button_t>() ; // action_i

                    // sticks
                    dev.add_input_component<natus::device::components::stick_t>() ; // movement
                    dev.add_input_component<natus::device::components::stick_t>() ; // aim
                   

                    // outputs
                    dev.add_output_component<natus::device::components::motor_t>() ; // vibration_a
                    dev.add_output_component<natus::device::components::motor_t>() ; // vibration_b
                }

            private:

                natus::device::components::button_ptr_t comp_button( this_t::input_component const ic ) const noexcept
                {
                    return _dev->get_component<natus::device::components::button_t>( ic ) ;
                }

            public:

                game_controller( this_t::this_device_res_t dev ) : _dev( ::std::move( dev ) )
                {
                    if( !_dev.is_valid() )
                    {
                        _dev = this_device_t() ;
                    }
                }
                game_controller( this_cref_t rhv ) : _dev( rhv._dev ) {}
                game_controller( this_rref_t rhv ) : _dev( ::std::move( rhv._dev ) ) {}
                ~game_controller( void_t ) {}

            public:

                enum class button
                {
                    jump,
                    crouch,
                    shoot,
                    interact,

                    pause,
                    menu,
                    home,

                    action_a,
                    action_b,
                    action_c,
                    action_d,
                    action_e,
                    action_f,
                    action_g,
                    action_h,
                    action_i,

                    num_buttons
                };

                static natus::ntd::string_t to_string( this_t::button const b ) noexcept
                {
                    static const natus::ntd::string_t __names[] = { "jump", "crouch", "shoot", "interact", "pause", 
                        "menu", "home", "action_a", "action_b", "action_c", "action_d", "action_e", "action_f", "action_g",
                        "action_h", "action_i", "invalid" } ;
                    return size_t( b ) >= size_t( button::num_buttons ) ? __names[ size_t( button::num_buttons ) ] : __names[ size_t( b ) ] ;
                }

                enum class directional
                {
                    movement, aim, num_dirs
                };
                static natus::ntd::string_t to_string( this_t::directional const b ) noexcept
                {
                    static const natus::ntd::string_t __names[] = { "movement", "aim", "invalid" } ;
                    return size_t( b ) >= size_t( directional::num_dirs ) ? __names[ size_t( directional::num_dirs ) ] : __names[ size_t( b ) ] ;
                }

                enum class vibration
                {
                    a, b, num_vibs
                };
                static natus::ntd::string_t to_string( this_t::vibration const b ) noexcept
                {
                    static const natus::ntd::string_t __names[] = { "a", "b", "invalid" } ;
                    return size_t( b ) >= size_t( vibration::num_vibs ) ? __names[ size_t( vibration::num_vibs ) ] : __names[ size_t( b ) ] ;
                }

            public: // is

                bool_t is( this_t::button const b, natus::device::components::button_state const bs, float_out_t value ) const noexcept
                {
                    natus::device::components::button_ptr_t btn = nullptr ;

                    switch( b )
                    {
                    case this_t::button::jump: btn = this_t::comp_button( this_t::input_component::jump ) ; break ;
                    case this_t::button::crouch: btn = this_t::comp_button( this_t::input_component::crouch ) ; break ;
                    case this_t::button::shoot: btn = this_t::comp_button( this_t::input_component::shoot ) ; break ;
                    case this_t::button::interact: btn = this_t::comp_button( this_t::input_component::interact ) ; break ;
                    case this_t::button::pause: btn = this_t::comp_button( this_t::input_component::pause ) ; break ;
                    case this_t::button::menu: btn = this_t::comp_button( this_t::input_component::menu ) ; break ;
                    case this_t::button::home: btn = this_t::comp_button( this_t::input_component::home ) ; break ;
                    case this_t::button::action_a: btn = this_t::comp_button( this_t::input_component::action_a ) ; break ;
                    case this_t::button::action_b: btn = this_t::comp_button( this_t::input_component::action_b ) ; break ;
                    case this_t::button::action_c: btn = this_t::comp_button( this_t::input_component::action_c ) ; break ;
                    case this_t::button::action_d: btn = this_t::comp_button( this_t::input_component::action_d ) ; break ;
                    case this_t::button::action_e: btn = this_t::comp_button( this_t::input_component::action_e ) ; break ;
                    case this_t::button::action_f: btn = this_t::comp_button( this_t::input_component::action_f ) ; break ;
                    case this_t::button::action_g: btn = this_t::comp_button( this_t::input_component::action_g ) ; break ;
                    case this_t::button::action_h: btn = this_t::comp_button( this_t::input_component::action_h ) ; break ;
                    case this_t::button::action_i: btn = this_t::comp_button( this_t::input_component::action_i ) ; break ;
                    default: break ;
                    }
                    if( natus::core::is_nullptr( btn ) ) return false ;
                    if( btn->state() != bs ) return false ;
                     
                    value = btn->value() ;

                    return true ;
                }

                bool_t is( this_t::directional const d, natus::device::components::stick_state const bs, natus::math::vec2f_out_t value ) const noexcept
                {
                    using comp_t = natus::device::components::stick_t ;
                    comp_t * comp = nullptr ;

                    switch( d )
                    {
                    case this_t::directional::movement: comp = _dev->get_component<comp_t>( this_t::input_component::movement ) ; break ;
                    case this_t::directional::aim: comp = _dev->get_component<comp_t>( this_t::input_component::aim ) ; break ;
                    
                    default: break ;
                    }
                    if( natus::core::is_nullptr( comp ) ) return false ;
                    if( comp->state() != bs ) return false ;

                    value = comp->value() ;

                    return true ;
                }

            public: // vibration

                bool_t is( this_t::vibration const b, natus::device::components::motor_state const bs, float_out_t value ) const noexcept
                {
                    using comp_t = natus::device::components::motor_t ;
                    comp_t* comp = nullptr ;

                    switch( b )
                    {
                    case this_t::vibration::a: comp = _dev->get_component<comp_t>( this_t::output_component::vibration_a ) ; break ;
                    case this_t::vibration::b: comp = _dev->get_component<comp_t>( this_t::output_component::vibration_b ) ; break ;

                    default: break ;
                    }
                    if( natus::core::is_nullptr( comp ) ) return false ;
                    if( comp->state() != bs ) return false ;

                    value = comp->value() ;

                    return true ;
                }

                bool_t set( this_t::vibration const v, float_t const value ) noexcept
                {
                    using comp_t = natus::device::components::motor_t ;
                    comp_t* comp = nullptr ;

                    switch( v )
                    {
                    case this_t::vibration::a: comp = _dev->get_component<comp_t>( this_t::output_component::vibration_a ) ; break ;
                    case this_t::vibration::b: comp = _dev->get_component<comp_t>( this_t::output_component::vibration_b ) ; break ;

                    default: break ;
                    }
                    if( natus::core::is_nullptr( comp ) ) return false ;
                    *comp = value ;

                    return true ;
                }
            };
            natus_res_typedef( game_controller ) ;
        }
        natus_res_typedefs( natus::device::device< natus::device::layouts::game_controller_t>, game_device ) ;
    }
}