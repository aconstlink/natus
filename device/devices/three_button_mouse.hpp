

#pragma once

#include "../layouts/three.hpp"

#include <natus/math/vector/vector2.hpp>

namespace natus
{
    namespace device
    {
        class three_button_mouse
        {
            natus_this_typedefs( three_button_mouse ) ;

        public:

            enum class button
            {
                none, left, right, middle, num_buttons
            };

            static natus::std::string_cref_t to_string( button const b ) noexcept
            {
                static natus::std::string __names[] = { "none", "left", "right", "middle", "invalid" } ;                
                return __names[ size_t( b ) >= size_t( button::num_buttons ) ? size_t( button::num_buttons ) : size_t(b) ] ;
            }

        private:

            three_device_res_t _dev ;

        public:

            three_button_mouse( three_device_res_t dev ) : _dev( dev )
            {}

            three_button_mouse( this_cref_t rhv )
            {
                _dev = rhv._dev ;
            }

            three_button_mouse( this_rref_t rhv )
            {
                _dev = ::std::move( rhv._dev ) ;
            }
        
        public:

            float_t get_scroll( void_t ) const noexcept
            {
                // @todo
                return 0.0f ;
            }

            natus::math::vec2f_t get_coords( void_t ) const noexcept
            {
                // @todo
                return natus::math::vec2f_t() ;
            }

        public:

            natus::device::components::button_ptr_t get_component( this_t::button const b ) noexcept
            {
                natus::device::components::button_ptr_t ret = nullptr ;

                if( b == this_t::button::left )
                {
                    ret = _dev->layout().get_component<natus::device::components::button_t>(
                        natus::device::layouts::three::input_component::left_button ) ;
                }
                else if( b == this_t::button::middle )
                {
                    ret = _dev->layout().get_component<natus::device::components::button_t>(
                        natus::device::layouts::three::input_component::middle_button ) ;
                }
                else if( b == this_t::button::right )
                {
                    ret = _dev->layout().get_component<natus::device::components::button_t>(
                        natus::device::layouts::three::input_component::right_button ) ;
                }

                return ret ;
            }

            natus::device::components::scroll_ptr_t get_scroll_component( void_t ) noexcept
            {
                return _dev->layout().get_component<natus::device::components::scroll_t>(
                    natus::device::layouts::three::input_component::scroll_wheel ) ;
            }

            natus::device::components::point_ptr_t get_pointer_component( void_t ) noexcept
            {
                return _dev->layout().get_component<natus::device::components::point_t>(
                    natus::device::layouts::three::input_component::coord ) ;
            }

        public:

            bool_t is_pressed( this_t::button const b ) const noexcept
            {
                bool_t res = false ;
                if( !_dev.is_valid() ) return res ;

                switch( b )
                {
                case this_t::button::left:
                {
                    auto* comp = _dev->layout().get_component<natus::device::components::button_t>( natus::device::layouts::three::input_component::left_button ) ;

                    res = comp->state() == natus::device::components::button_state::pressed ;
                }
                    
                    break ;
                case this_t::button::right:
                {
                    auto* comp = _dev->layout().get_component<natus::device::components::button_t>( natus::device::layouts::three::input_component::right_button ) ;

                    res = comp->state() == natus::device::components::button_state::pressed ;
                    break ;
                }
                case this_t::button::middle:
                {
                    auto* comp = _dev->layout().get_component<natus::device::components::button_t>( natus::device::layouts::three::input_component::middle_button ) ;

                    res = comp->state() == natus::device::components::button_state::pressed ;
                    break ;
                }
                }

                return res ;
            }

            bool_t is_pressing( this_t::button const b ) const noexcept
            {
                bool_t res = false ;
                if( !_dev.is_valid() ) return res ;

                switch( b )
                {
                case this_t::button::left:
                {
                    auto* comp = _dev->layout().get_component<natus::device::components::button_t>( natus::device::layouts::three::input_component::left_button ) ;

                    res = comp->state() == natus::device::components::button_state::pressing ;

                    break ;
                }
                case this_t::button::right:
                {
                    auto* comp = _dev->layout().get_component<natus::device::components::button_t>( natus::device::layouts::three::input_component::right_button ) ;

                    res = comp->state() == natus::device::components::button_state::pressing ;
                    break ;
                }
                case this_t::button::middle:
                {
                    auto* comp = _dev->layout().get_component<natus::device::components::button_t>( natus::device::layouts::three::input_component::middle_button ) ;

                    res = comp->state() == natus::device::components::button_state::pressing ;
                    break ;
                }
                }

                return res ;
            }

            bool_t is_released( this_t::button const b ) const noexcept
            {
                bool_t res = false ;
                if( !_dev.is_valid() ) return res ;

                switch( b )
                {
                case this_t::button::left:
                {
                    auto* comp = _dev->layout().get_component<natus::device::components::button_t>( natus::device::layouts::three::input_component::left_button ) ;

                    res = comp->state() == natus::device::components::button_state::released ;

                    break ;
                }
                case this_t::button::right:
                {
                    auto* comp = _dev->layout().get_component<natus::device::components::button_t>( natus::device::layouts::three::input_component::right_button ) ;

                    res = comp->state() == natus::device::components::button_state::released ;
                    break ;
                }
                case this_t::button::middle:
                {
                    auto* comp = _dev->layout().get_component<natus::device::components::button_t>( natus::device::layouts::three::input_component::middle_button ) ;

                    res = comp->state() == natus::device::components::button_state::released ;
                    break ;
                }
                }

                return res ;
            }
        };
        natus_soil_typedef( three_button_mouse ) ;
    }
}