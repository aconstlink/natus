
#pragma once

#include "../typedefs.h"
#include "../components/button.hpp"
#include "../components/scroll.hpp"
#include "../components/point.hpp"
#include "../device.hpp"

namespace natus
{
    namespace device
    {
        namespace layouts
        {
            class three_mouse
            {
                natus_this_typedefs( three_mouse ) ;
                natus_res_typedefs( natus::device::device< this_t >, this_device ) ;

                friend this_device_t ;

            public:

                enum class input_component
                {
                    left_button,
                    right_button,
                    middle_button,
                    scroll_wheel,
                    local_coords,
                    global_coords,
                    num_components
                };

                enum class output_component
                {
                    num_components
                };

            private:

                static void_t init_components( this_t::this_device_ref_t dev ) 
                {
                    // inputs
                    dev.add_input_component<natus::device::components::button_t>() ;
                    dev.add_input_component<natus::device::components::button_t>() ;
                    dev.add_input_component<natus::device::components::button_t>() ;
                    dev.add_input_component<natus::device::components::scroll_t>() ;
                    dev.add_input_component<natus::device::components::point_t>() ;
                    dev.add_input_component<natus::device::components::point_t>() ;
                  
                    // outputs
                }

            private:

                this_t::this_device_res_t _dev ;

            public:

                three_mouse( this_t::this_device_res_t dev ) : _dev( ::std::move( dev ) )
                {
                    if( !_dev.is_valid() )
                    {
                        _dev = this_device_t() ;
                    }
                }
                three_mouse( this_cref_t rhv ) : _dev( rhv._dev ) {}
                three_mouse( this_rref_t rhv ) : _dev( ::std::move( rhv._dev ) ) {}
                ~three_mouse( void_t ) {}

            public:

                enum class button
                {
                    none, left, right, middle, num_buttons
                };

                static natus::ntd::string_cref_t to_string( button const b ) noexcept
                {
                    static natus::ntd::string __names[] = { "none", "left", "right", "middle", "invalid" } ;
                    return __names[ size_t( b ) >= size_t( button::num_buttons ) ? size_t( button::num_buttons ) : size_t( b ) ] ;
                }

                bool_t get_scroll( float_t & v ) noexcept
                {
                    auto* comp = _dev->get_component<natus::device::components::scroll_t>(
                        this_t::input_component::scroll_wheel ) ;

                    v = comp->value() ;

                    return comp->has_changed() ;
                }

                natus::math::vec2f_t get_local( void_t ) noexcept
                {
                    auto* comp = _dev->get_component<natus::device::components::point_t>(
                        this_t::input_component::local_coords ) ;

                    return natus::math::vec2f_t( comp->value() ) ;
                }

                natus::math::vec2f_t get_global( void_t ) noexcept
                {
                    auto* comp = _dev->get_component<natus::device::components::point_t>(
                        this_t::input_component::global_coords ) ;

                    return natus::math::vec2f_t( comp->value() ) ;
                }

            public:

                 natus::device::components::button_ptr_t get_component( this_t::button const b ) noexcept
                {
                    natus::device::components::button_ptr_t ret = nullptr ;

                    if( b == this_t::button::left )
                    {
                        ret = _dev->get_component<natus::device::components::button_t>(
                            this_t::input_component::left_button ) ;
                    }
                    else if( b == this_t::button::middle )
                    {
                        ret = _dev->get_component<natus::device::components::button_t>(
                            this_t::input_component::middle_button ) ;
                    }
                    else if( b == this_t::button::right )
                    {
                        ret = _dev->get_component<natus::device::components::button_t>(
                            this_t::input_component::right_button ) ;
                    }

                    return ret ;
                }

                natus::device::components::scroll_ptr_t get_scroll_component( void_t ) noexcept
                {
                    return _dev->get_component<natus::device::components::scroll_t>(
                        this_t::input_component::scroll_wheel ) ;
                }

                natus::device::components::point_ptr_t get_local_component( void_t ) noexcept
                {
                    return _dev->get_component<natus::device::components::point_t>(
                        this_t::input_component::local_coords ) ;
                }

                natus::device::components::point_ptr_t get_global_component( void_t ) noexcept
                {
                    return _dev->get_component<natus::device::components::point_t>(
                        this_t::input_component::global_coords ) ;
                }

            public:

                bool_t is_pressed( this_t::button const b ) noexcept
                {
                    bool_t res = false ;

                    switch( b )
                    {
                    case this_t::button::left:
                    {
                        auto* comp = _dev->get_component<natus::device::components::button_t>( this_t::input_component::left_button ) ;

                        res = comp->state() == natus::device::components::button_state::pressed ;
                    }

                    break ;
                    case this_t::button::right:
                    {
                        auto* comp = _dev->get_component<natus::device::components::button_t>( this_t::input_component::right_button ) ;

                        res = comp->state() == natus::device::components::button_state::pressed ;
                        break ;
                    }
                    case this_t::button::middle:
                    {
                        auto* comp = _dev->get_component<natus::device::components::button_t>( this_t::input_component::middle_button ) ;

                        res = comp->state() == natus::device::components::button_state::pressed ;
                        break ;
                    }
                    default: break ;
                    }

                    return res ;
                }

                bool_t is_pressing( this_t::button const b ) noexcept
                {
                    bool_t res = false ;

                    switch( b )
                    {
                    case this_t::button::left:
                    {
                        auto* comp = _dev->get_component<natus::device::components::button_t>( this_t::input_component::left_button ) ;

                        res = comp->state() == natus::device::components::button_state::pressing ;

                        break ;
                    }
                    case this_t::button::right:
                    {
                        auto* comp = _dev->get_component<natus::device::components::button_t>( this_t::input_component::right_button ) ;

                        res = comp->state() == natus::device::components::button_state::pressing ;
                        break ;
                    }
                    case this_t::button::middle:
                    {
                        auto* comp = _dev->get_component<natus::device::components::button_t>( this_t::input_component::middle_button ) ;

                        res = comp->state() == natus::device::components::button_state::pressing ;
                        break ;
                    }
                    default:break ;
                    }

                    return res ;
                }

                bool_t is_released( this_t::button const b ) noexcept
                {
                    bool_t res = false ;

                    switch( b )
                    {
                    case this_t::button::left:
                    {
                        auto* comp = _dev->get_component<natus::device::components::button_t>( this_t::input_component::left_button ) ;

                        res = comp->state() == natus::device::components::button_state::released ;

                        break ;
                    }
                    case this_t::button::right:
                    {
                        auto* comp = _dev->get_component<natus::device::components::button_t>( this_t::input_component::right_button ) ;

                        res = comp->state() == natus::device::components::button_state::released ;
                        break ;
                    }
                    case this_t::button::middle:
                    {
                        auto* comp = _dev->get_component<natus::device::components::button_t>( this_t::input_component::middle_button ) ;

                        res = comp->state() == natus::device::components::button_state::released ;
                        break ;
                    }
                    default: break ;
                    }

                    return res ;
                }
            };
            natus_typedef( three_mouse ) ;
        }
        natus_res_typedefs( natus::device::device< natus::device::layouts::three_mouse_t>, three_device ) ;
    }
}
