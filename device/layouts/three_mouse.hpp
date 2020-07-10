
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
                natus_soil_typedefs( natus::device::device< this_t >, this_device ) ;

                friend class this_device_t ;

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

            public:

                enum class button
                {
                    none, left, right, middle, num_buttons
                };

                static natus::std::string_cref_t to_string( button const b ) noexcept
                {
                    static natus::std::string __names[] = { "none", "left", "right", "middle", "invalid" } ;
                    return __names[ size_t( b ) >= size_t( button::num_buttons ) ? size_t( button::num_buttons ) : size_t( b ) ] ;
                }

                static bool_t get_scroll( this_device_res_t dev, float_t & v ) noexcept
                {
                    auto* comp = dev->get_component<natus::device::components::scroll_t>(
                        this_t::input_component::scroll_wheel ) ;

                    v = comp->value() ;

                    return comp->has_changed() ;
                }

                static natus::math::vec2f_t get_local( this_device_res_t dev ) noexcept
                {
                    auto* comp = dev->get_component<natus::device::components::point_t>(
                        this_t::input_component::local_coords ) ;

                    return natus::math::vec2f_t( comp->value() ) ;
                }

                static natus::math::vec2f_t get_global( this_device_res_t dev ) noexcept
                {
                    auto* comp = dev->get_component<natus::device::components::point_t>(
                        this_t::input_component::global_coords ) ;

                    return natus::math::vec2f_t( comp->value() ) ;
                }

            public:

                static natus::device::components::button_ptr_t get_component( 
                    this_device_res_t dev, this_t::button const b ) noexcept
                {
                    natus::device::components::button_ptr_t ret = nullptr ;

                    if( b == this_t::button::left )
                    {
                        ret = dev->get_component<natus::device::components::button_t>(
                            this_t::input_component::left_button ) ;
                    }
                    else if( b == this_t::button::middle )
                    {
                        ret = dev->get_component<natus::device::components::button_t>(
                            this_t::input_component::middle_button ) ;
                    }
                    else if( b == this_t::button::right )
                    {
                        ret = dev->get_component<natus::device::components::button_t>(
                            this_t::input_component::right_button ) ;
                    }

                    return ret ;
                }

                static natus::device::components::scroll_ptr_t get_scroll_component( this_device_res_t dev ) noexcept
                {
                    return dev->get_component<natus::device::components::scroll_t>(
                        this_t::input_component::scroll_wheel ) ;
                }

                static natus::device::components::point_ptr_t get_local_component( this_device_res_t dev ) noexcept
                {
                    return dev->get_component<natus::device::components::point_t>(
                        this_t::input_component::local_coords ) ;
                }

                static natus::device::components::point_ptr_t get_global_component( this_device_res_t dev ) noexcept
                {
                    return dev->get_component<natus::device::components::point_t>(
                        this_t::input_component::global_coords ) ;
                }

            public:

                static bool_t is_pressed( this_device_res_t dev, this_t::button const b ) noexcept
                {
                    bool_t res = false ;
                    if( !dev.is_valid() ) return res ;

                    switch( b )
                    {
                    case this_t::button::left:
                    {
                        auto* comp = dev->get_component<natus::device::components::button_t>( this_t::input_component::left_button ) ;

                        res = comp->state() == natus::device::components::button_state::pressed ;
                    }

                    break ;
                    case this_t::button::right:
                    {
                        auto* comp = dev->get_component<natus::device::components::button_t>( this_t::input_component::right_button ) ;

                        res = comp->state() == natus::device::components::button_state::pressed ;
                        break ;
                    }
                    case this_t::button::middle:
                    {
                        auto* comp = dev->get_component<natus::device::components::button_t>( this_t::input_component::middle_button ) ;

                        res = comp->state() == natus::device::components::button_state::pressed ;
                        break ;
                    }
                    }

                    return res ;
                }

                static bool_t is_pressing( this_device_res_t dev, this_t::button const b ) noexcept
                {
                    bool_t res = false ;
                    if( !dev.is_valid() ) return res ;

                    switch( b )
                    {
                    case this_t::button::left:
                    {
                        auto* comp = dev->get_component<natus::device::components::button_t>( this_t::input_component::left_button ) ;

                        res = comp->state() == natus::device::components::button_state::pressing ;

                        break ;
                    }
                    case this_t::button::right:
                    {
                        auto* comp = dev->get_component<natus::device::components::button_t>( this_t::input_component::right_button ) ;

                        res = comp->state() == natus::device::components::button_state::pressing ;
                        break ;
                    }
                    case this_t::button::middle:
                    {
                        auto* comp = dev->get_component<natus::device::components::button_t>( this_t::input_component::middle_button ) ;

                        res = comp->state() == natus::device::components::button_state::pressing ;
                        break ;
                    }
                    }

                    return res ;
                }

                static bool_t is_released( this_device_res_t dev, this_t::button const b ) noexcept
                {
                    bool_t res = false ;
                    if( !dev.is_valid() ) return res ;

                    switch( b )
                    {
                    case this_t::button::left:
                    {
                        auto* comp = dev->get_component<natus::device::components::button_t>( this_t::input_component::left_button ) ;

                        res = comp->state() == natus::device::components::button_state::released ;

                        break ;
                    }
                    case this_t::button::right:
                    {
                        auto* comp = dev->get_component<natus::device::components::button_t>( this_t::input_component::right_button ) ;

                        res = comp->state() == natus::device::components::button_state::released ;
                        break ;
                    }
                    case this_t::button::middle:
                    {
                        auto* comp = dev->get_component<natus::device::components::button_t>( this_t::input_component::middle_button ) ;

                        res = comp->state() == natus::device::components::button_state::released ;
                        break ;
                    }
                    }

                    return res ;
                }
            };
            natus_typedef( three_mouse ) ;
        }
        natus_soil_typedefs( natus::device::device< natus::device::layouts::three_mouse_t>, three_device ) ;
    }
}