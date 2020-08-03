
#pragma once

#include "device.hpp"

#include "components/button.hpp"
#include "components/key.hpp"
#include "components/knob.hpp"
#include "components/light.hpp"
#include "components/motor.hpp"
#include "components/point.hpp"
#include "components/scroll.hpp"
#include "components/slider.hpp"
#include "components/stick.hpp"
#include "components/touch.hpp"

#include "layouts/ascii_keyboard.hpp"

namespace natus
{
    namespace device
    {
        namespace detail
        {
            using comp_t = natus::device::icomponent ;
            using comp_button_t = natus::device::components::button_t ;
            using comp_ascii_t = natus::device::layouts::ascii_keyboard_t::ascii_component_t;
            //using comp_knob_t = natus::device::components::knob_t ;
            using comp_point_t = natus::device::components::point_t ;
            using comp_scroll_t = natus::device::components::scroll_t ;
            //using comp_slider_t = natus::device::components::slider_t ;
            using comp_stick_t = natus::device::components::stick_t ;
            //using comp_touch_t = natus::device::components::touch_t ;


            //using comp_led_t = natus::device::components::led_t ;
            //using comp_light_t = natus::device::components::light_t ;
            using comp_motor_t = natus::device::components::motor_t ;

            typedef ::std::function< void_t ( comp_t*, comp_t const* ) > mapping_funk_t ;

            template< typename T >
            bool_t a_to_b( comp_t* ic_a, comp_t* ic_b, mapping_funk_t& funk_out ) ;

            template<>
            bool_t a_to_b<void_t>( comp_t* ic_a, comp_t* ic_b, mapping_funk_t& funk_out )
            {
                // inputs
                if( natus::core::is_not_nullptr( dynamic_cast< comp_button_t* >( ic_a ) ) )
                {
                    return a_to_b<comp_button_t>( ic_a, ic_b, funk_out ) ;
                }
                if( natus::core::is_not_nullptr( dynamic_cast< comp_ascii_t* >( ic_a ) ) )
                {
                    return a_to_b<comp_ascii_t>( ic_a, ic_b, funk_out ) ;
                }
                if( natus::core::is_not_nullptr( dynamic_cast< comp_point_t* >( ic_a ) ) )
                {
                    return a_to_b<comp_point_t>( ic_a, ic_b, funk_out ) ;
                }
                if( natus::core::is_not_nullptr( dynamic_cast< comp_scroll_t* >( ic_a ) ) )
                {
                    return a_to_b<comp_scroll_t>( ic_a, ic_b, funk_out ) ;
                }
                if( natus::core::is_not_nullptr( dynamic_cast< comp_stick_t* >( ic_a ) ) )
                {
                    return a_to_b<comp_stick_t>( ic_a, ic_b, funk_out ) ;
                }

                // outputs
                if( natus::core::is_not_nullptr( dynamic_cast< comp_motor_t* >( ic_a ) ) )
                {
                    return a_to_b<comp_motor_t>( ic_a, ic_b, funk_out ) ;
                }

                return false ;
            }

            template<typename T>
            bool_t a_to_b<comp_button_t>( comp_t* ic_a, comp_t* ic_b, mapping_funk_t& funk_out )
            {
                {
                    using other_t = comp_button_t ;
                    other_t* comp = dynamic_cast< other_t* >( ic_b ) ;
                    if( natus::core::is_not_nullptr( comp ) )
                    {
                        funk_out = [&] ( comp_t* a_, comp_t const * b_ )
                        {
                            comp_button_t* a = reinterpret_cast< comp_button_t* >( a_ ) ;
                            other_t const * b = reinterpret_cast< other_t const* >( b_ ) ;
                            *a = *b ;
                        } ;
                        return true ;
                    }
                }
                {
                    using other_t = comp_ascii_t ;
                    other_t* comp = dynamic_cast< other_t* >( ic_b ) ;
                    if( natus::core::is_not_nullptr( comp ) )
                    {
                        funk_out = [&] ( comp_t* a_, comp_t const* b_ )
                        {
                            comp_button_t* a = reinterpret_cast< comp_button_t* >( a_ ) ;
                            other_t const* b = reinterpret_cast< other_t const* >( b_ ) ;
                            
                            natus::device::components::button_state bs = components::button_state::none ;

                            switch( b->state() )
                            {
                            case natus::device::components::key_state::pressed: bs = components::button_state::pressed ; break ;
                            case natus::device::components::key_state::pressing: bs = components::button_state::pressing ; break ;
                            case natus::device::components::key_state::released: bs = components::button_state::released ; break ;
                            default: break ;
                            }

                            *a = bs ;
                            *a = b->value() ;
                        } ;
                        return true ;
                    }
                }

                natus::log::global_t::warning( natus_log_fn( "no appropriate device component mapping found" ) ) ;

                return false ;
            }
        }

        class imapping 
        { 
        public: 
            virtual void_t update( void_t ) noexcept = 0; 
        };
        natus_typedef( imapping ) ;
        using imapping_res_t = natus::soil::res< imapping_t > ;


        // maps device a to device b
        // device a is user-controlled
        template< class device_a, class device_b > 
        class mapping : public imapping
        {
            typedef mapping< device_a, device_b > __this_t ;
            natus_this_typedefs( __this_t ) ;
            natus_soil_typedef( device_a ) ;
            natus_soil_typedef( device_b ) ;

        private:

            device_a_res_t _a ;
            device_b_res_t _b ;

            struct input_mapping
            {
                typename device_a_t::layout_t::input_component a ;
                typename device_b_t::layout_t::input_component b ;
                detail::mapping_funk_t funk ;
            };
            natus::std::vector< input_mapping > _inputs ;

            struct output_mapping
            {
                typename device_a_t::layout_t::output_component a ;
                typename device_b_t::layout_t::output_component b ;
                detail::mapping_funk_t funk ;
            };
            natus::std::vector< output_mapping > _outputs ;

        public:

            mapping( device_a_res_t a, device_b_res_t b ) : _a( a ), _b( b ) {}
            mapping( this_cref_t rhv ) : _a( rhv._a), _b(rhv._b), 
                _inputs(rhv._inputs), _outputs(rhv._outputs) {}
            mapping( this_rref_t rhv ) 
            {
                _a = ::std::move( rhv._a ) ;
                _b = ::std::move( rhv._b ) ;
                _inputs = ::std::move( rhv._inputs ) ;
                _outputs = ::std::move( rhv._outputs ) ;
            }

            this_ref_t operator = ( this_cref_t rhv ) noexcept
            {
                _a = rhv._a ;
                _b = rhv._b ;
                _inputs = rhv._inputs ;
                _outputs = rhv._outputs ;
                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv ) noexcept
            {
                _a = ::std::move( rhv._a ) ;
                _b = ::std::move( rhv._b ) ;
                _inputs = ::std::move( rhv._inputs ) ;
                _outputs = ::std::move( rhv._outputs ) ;
                return *this ;
            }
        
        public:

            // adds or exchanges an input mapping
            bool_t insert( 
                typename device_a_t::layout_t::input_component const a, 
                typename device_b_t::layout_t::input_component const b ) noexcept
            {
                auto * ic_a = _a->get_component( a ) ;
                auto * ic_b = _b->get_component( b ) ;

                detail::mapping_funk_t funk ;
                if( detail::a_to_b<void_t>( ic_a, ic_b, funk ) )
                {
                    _inputs.emplace_back( input_mapping{ a, b, funk } ) ;
                    return true ;
                }

                return false ;
            }

            virtual void_t update( void_t ) noexcept
            {
                for( auto const & m : _inputs ) 
                {
                    auto * ic_a = _a->get_component( m.a ) ;
                    auto const* ic_b = _b->get_component( m.b ) ;
                    m.funk( ic_a, ic_b ) ;
                }
            }
        };


        
    }
}