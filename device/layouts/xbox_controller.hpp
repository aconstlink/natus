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
            class xbox_controller
            {
                natus_this_typedefs( xbox_controller ) ;
                natus_soil_typedefs( natus::device::device< this_t >, this_device ) ;

                friend this_device_t ;

            public:

                enum class input_component
                {
                    button_back,            // XINPUT_GAMEPAD_BACK
                    button_start,           // XINPUT_GAMEPAD_START
                    button_a,               // XINPUT_GAMEPAD_A
                    button_b,               // XINPUT_GAMEPAD_B
                    button_x,               // XINPUT_GAMEPAD_X
                    button_y,               // XINPUT_GAMEPAD_Y
                    button_thumb_left,      // XINPUT_GAMEPAD_LEFT_THUMB
                    button_thumb_right,     // XINPUT_GAMEPAD_RIGHT_THUMB
                    button_shoulder_left,   // XINPUT_GAMEPAD_LEFT_SHOULDER
                    button_shoulder_right,  // XINPUT_GAMEPAD_RIGHT_SHOULDER

                    button_dpad_up,         // XINPUT_GAMEPAD_DPAD_UP
                    button_dpad_down,       // XINPUT_GAMEPAD_DPAD_DOWN
                    button_dpad_left,       // XINPUT_GAMEPAD_DPAD_LEFT
                    button_dpad_right,      // XINPUT_GAMEPAD_DPAD_RIGHT

                    stick_left,
                    stick_right,

                    trigger_left,
                    trigger_right,

                    num_components
                };

                enum class output_component
                {
                    motor_left,
                    motor_right,
                    num_components
                };

            private:

                static void_t init_components( this_t::this_device_ref_t dev )
                {
                    // inputs

                    // buttons
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

                    // sticks
                    dev.add_input_component<natus::device::components::stick_t>() ;
                    dev.add_input_component<natus::device::components::stick_t>() ;

                    // triggers
                    dev.add_input_component<natus::device::components::button_t>() ;
                    dev.add_input_component<natus::device::components::button_t>() ;

                    // outputs
                    dev.add_output_component<natus::device::components::motor_t>() ;
                    dev.add_output_component<natus::device::components::motor_t>() ;
                }

            public: 

                natus::device::components::button_ptr_t comp_button( input_component const ic ) const noexcept
                {
                    return _dev->get_component<natus::device::components::button_t>( ic ) ;
                }

            private:

                this_t::this_device_res_t _dev ;

            public:

                xbox_controller( this_t::this_device_res_t dev ) : _dev( ::std::move( dev ) )
                {
                    if( !_dev.is_valid() )
                    {
                        _dev = this_device_t() ;
                    }
                }
                xbox_controller( this_cref_t rhv ) : _dev( rhv._dev ) {}
                xbox_controller( this_rref_t rhv ) : _dev( ::std::move( rhv._dev ) ) {}
                ~xbox_controller( void_t ) {}

            public:

                enum class button
                {
                    none, back, start, a, b, x, y
                };
                
                static natus::std::string_t to_string( button const b ) noexcept
                {
                    static const natus::std::string_t __names[] = { "none", "back", "start", "a", "b", "x", "y", "invalid" } ;
                    return size_t( b ) >= size_t( button::y ) ? __names[ size_t( button::y ) ] : __names[ size_t( b ) ] ;
                }

                enum class thumb
                {
                    none, left, right
                };

                static natus::std::string_t to_string( thumb const b ) noexcept
                {
                    static const natus::std::string_t __names[] = { "none", "left", "right", "invalid" } ;
                    return size_t( b ) >= size_t( thumb::right ) ? __names[ size_t( thumb::right ) ] : __names[ size_t( b ) ] ;
                }

                enum class shoulder
                {
                    none, left, right
                };
                static natus::std::string_t to_string( shoulder const b ) noexcept
                {
                    static const natus::std::string_t __names[] = { "none", "left", "right", "invalid" } ;
                    return size_t( b ) >= size_t( shoulder::right ) ? __names[ size_t( shoulder::right ) ] : __names[ size_t( b ) ] ;
                }

                enum class stick
                {
                    none, left, right 
                };
                static natus::std::string_t to_string( stick const b ) noexcept
                {
                    static const natus::std::string_t __names[] = { "none", "left", "right", "invalid" } ;
                    return size_t( b ) >= size_t( stick::right ) ? __names[ size_t( stick::right ) ] : __names[ size_t( b ) ] ;
                }

                enum class trigger
                {
                    none, left, right
                };
                static natus::std::string_t to_string( trigger const b ) noexcept
                {
                    static const natus::std::string_t __names[] = { "none", "left", "right", "invalid" } ;
                    return size_t( b ) >= size_t( trigger::right ) ? __names[ size_t( trigger::right ) ] : __names[ size_t( b ) ] ;
                }

                enum class dpad
                {
                    none, left, right, up, down
                };
                static natus::std::string_t to_string( dpad const b ) noexcept
                {
                    static const natus::std::string_t __names[] = { "none", "left", "right", "up", "down", "invalid" } ;
                    return size_t( b ) >= size_t( dpad::down ) ? __names[ size_t( dpad::down ) ] : __names[ size_t( b ) ] ;
                }

                enum class motor
                {
                    none, left, right
                };
                static natus::std::string_t to_string( motor const b ) noexcept
                {
                    static const natus::std::string_t __names[] = { "none", "left", "right", "invalid" } ;
                    return size_t( b ) >= size_t( motor::right ) ? __names[ size_t( motor::right ) ] : __names[ size_t( b ) ] ;
                }

            public: // is functions


                bool_t is( this_t::button const b, natus::device::components::button_state const bs ) const noexcept
                {
                    return this_t::get_component(b)->state() == bs ;
                }

                bool_t is( this_t::thumb const b, natus::device::components::button_state const bs ) const noexcept
                {
                    return this_t::get_component(b)->state() == bs ;
                }

                bool_t is( this_t::shoulder const b, natus::device::components::button_state const bs ) const noexcept
                {
                    return this_t::get_component(b)->state() == bs ;
                }

                bool_t is( this_t::dpad const b, natus::device::components::button_state const bs ) const noexcept
                {
                    return this_t::get_component(b)->state() == bs ;
                }

                bool_t is( this_t::trigger const b, natus::device::components::button_state const bs, float_out_t v ) const noexcept
                {
                    auto* comp = this_t::get_component( b ) ;

                    v = comp->value() ;
                    return comp->state() == bs ;
                }


                bool_t is( this_t::motor const b, natus::device::components::motor_state const bs, float_out_t v ) const noexcept
                {
                    auto* comp = this_t::get_component( b ) ;

                    v = comp->value() ;
                    return comp->state() == bs ;
                }

            public: // stick

                natus::math::vec2f_t get_stick( this_t::stick const s ) const noexcept
                {
                    return this_t::get_component( s )->value() ;
                }

            public: // get component

                natus::device::components::button_ptr_t get_component( this_t::button const b ) const noexcept
                {
                    this_t::input_component ic = this_t::input_component::num_components ;

                    switch( b )
                    {
                    case this_t::button::back: ic = this_t::input_component::button_back ; break ;
                    case this_t::button::start: ic = this_t::input_component::button_start ; break ;
                    case this_t::button::a: ic = this_t::input_component::button_a ; break ;
                    case this_t::button::b: ic = this_t::input_component::button_b ; break ;
                    case this_t::button::x: ic = this_t::input_component::button_x ; break ;
                    case this_t::button::y: ic = this_t::input_component::button_y ; break ;
                    default: break ;
                    }

                    if( ic == this_t::input_component::num_components ) return nullptr ;

                    return this_t::comp_button( ic );
                }

                natus::device::components::button_ptr_t get_component( this_t::thumb const b ) const noexcept
                {
                    this_t::input_component ic = this_t::input_component::num_components ;

                    switch( b )
                    {
                    case this_t::thumb::left: ic = this_t::input_component::button_thumb_left ; break ;
                    case this_t::thumb::right: ic = this_t::input_component::button_thumb_right ; break ;
                    default: break ;
                    }

                    if( ic == this_t::input_component::num_components ) return nullptr ;

                    return this_t::comp_button( ic ) ;
                }

                natus::device::components::button_ptr_t get_component( this_t::shoulder const b ) const noexcept
                {
                    this_t::input_component ic = this_t::input_component::num_components ;

                    switch( b )
                    {
                    case this_t::shoulder::left: ic = this_t::input_component::button_shoulder_left ; break ;
                    case this_t::shoulder::right: ic = this_t::input_component::button_shoulder_right ; break ;
                    default: break ;
                    }

                    if( ic == this_t::input_component::num_components ) return nullptr ;

                    return this_t::comp_button( ic ) ;
                }

                natus::device::components::button_ptr_t get_component( this_t::dpad const b ) const noexcept
                {
                    this_t::input_component ic = this_t::input_component::num_components ;

                    switch( b )
                    {
                    case this_t::dpad::left: ic = this_t::input_component::button_shoulder_left ; break ;
                    case this_t::dpad::right: ic = this_t::input_component::button_shoulder_right ; break ;
                    case this_t::dpad::up: ic = this_t::input_component::button_dpad_up ; break ;
                    case this_t::dpad::down: ic = this_t::input_component::button_dpad_down ; break ;
                    default: break ;
                    }

                    if( ic == this_t::input_component::num_components ) return nullptr ;

                    return this_t::comp_button( ic ) ;
                }

                natus::device::components::button_ptr_t get_component( this_t::trigger const b ) const noexcept
                {
                    this_t::input_component ic = this_t::input_component::num_components ;

                    switch( b )
                    {
                    case this_t::trigger::left: ic = this_t::input_component::trigger_left ; break ;
                    case this_t::trigger::right: ic = this_t::input_component::trigger_right ; break ;
                    default: break ;
                    }

                    if( ic == this_t::input_component::num_components ) return nullptr ;

                    return this_t::comp_button( ic ) ;
                }

                natus::device::components::stick_ptr_t get_component( this_t::stick const s ) const noexcept
                {
                    this_t::input_component ic = this_t::input_component::num_components ;

                    switch( s )
                    {
                    case this_t::stick::left: ic = this_t::input_component::stick_left ; break ;
                    case this_t::stick::right: ic = this_t::input_component::stick_right ; break ;
                    default: nullptr ;
                    }

                    return _dev->get_component<natus::device::components::stick_t>( ic ) ;
                }

                natus::device::components::motor_ptr_t get_component( this_t::motor const s ) const noexcept
                {
                    this_t::output_component ic = this_t::output_component::num_components ;

                    switch( s )
                    {
                    case this_t::motor::left: ic = this_t::output_component::motor_left ; break ;
                    case this_t::motor::right: ic = this_t::output_component::motor_right ; break ;
                    default: nullptr ;
                    }

                    return _dev->get_component<natus::device::components::motor_t>( ic ) ;
                }

            } ;
            natus_typedef( xbox_controller ) ;
        }
        natus_soil_typedefs( natus::device::device< natus::device::layouts::xbox_controller_t>, xbc_device ) ;
    }
}