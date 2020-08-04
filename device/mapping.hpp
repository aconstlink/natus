
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
        enum class mapping_detail
        {
            one_to_one,
            negative_x,
            positive_x,
            negative_y,
            positive_y,

            num_details
        };

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
            bool_t b_to_a( comp_t*, comp_t*, natus::device::mapping_detail const, mapping_funk_t& ) 
            {
                return false ;
            }

            template<>
            bool_t b_to_a<comp_stick_t>( comp_t* ic_a, comp_t* ic_b, 
                natus::device::mapping_detail const det, mapping_funk_t& funk_out )
            {
                using a_t = comp_stick_t ;

                // stick to stick
                {
                    using b_t = comp_stick_t ;
                    b_t* comp = dynamic_cast< b_t* >( ic_b ) ;
                    if( natus::core::is_not_nullptr( comp ) )
                    {
                        funk_out = [&] ( comp_t* a_, comp_t const* b_ )
                        {
                            a_t* a = reinterpret_cast< a_t* >( a_ ) ;
                            b_t const* b = reinterpret_cast< b_t const* >( b_ ) ;
                            if( b->state() != natus::device::components::stick_state::none ) 
                                *a = *b ;
                        } ;
                        return true ;
                    }
                }
                // point to stick
                {
                    using b_t = comp_point_t ;
                    b_t* comp = dynamic_cast< b_t* >( ic_b ) ;
                    if( natus::core::is_not_nullptr( comp ) )
                    {
                        funk_out = [&] ( comp_t* a_, comp_t const* b_ )
                        {
                            a_t* a = reinterpret_cast< a_t* >( a_ ) ;
                            b_t const* b = reinterpret_cast< b_t const* >( b_ ) ;

                            // add the relative stick to the absolute point
                            if( b->has_changed() )
                            {
                                natus::device::components::stick_state ss = natus::device::components::stick_state::none ;

                                switch( b->state() )
                                {
                                case natus::device::components::move_state::moved: 
                                    ss = natus::device::components::stick_state::tilted ; break ;
                                case natus::device::components::move_state::moving:
                                    ss = natus::device::components::stick_state::tilting ; break ;
                                case natus::device::components::move_state::unmoved:
                                    ss = natus::device::components::stick_state::untilted ; break ;
                                }

                                *a = b->rel() ;
                                *a = ss ;
                            }
                        } ;
                        return true ;
                    }
                }
                // ascii to stick
                {
                    using b_t = comp_ascii_t ;
                    b_t* comp = dynamic_cast< b_t* >( ic_b ) ;
                    if( natus::core::is_not_nullptr( comp ) )
                    {
                        funk_out = [=] ( comp_t* a_, comp_t const* b_ )
                        {
                            a_t* a = reinterpret_cast< a_t* >( a_ ) ;
                            b_t const* b = reinterpret_cast< b_t const* >( b_ ) ;
                            
                            if( b->state() != natus::device::components::key_state::none )
                            {
                                natus::device::components::stick_state ss = natus::device::components::stick_state::none ;
                                switch( b->state() ) 
                                {
                                case natus::device::components::key_state::pressed: 
                                    ss = natus::device::components::stick_state::tilted; break ;
                                case natus::device::components::key_state::pressing: 
                                    ss = natus::device::components::stick_state::tilting; break ;
                                case natus::device::components::key_state::released: 
                                    ss = natus::device::components::stick_state::untilted; break ;
                                default: break ;
                                }
                                
                                natus::math::vec2f_t v0( a->value() ) ;
                                float_t const v1 = b->value() ;

                                switch( det )
                                {                                
                                case natus::device::mapping_detail::negative_x: v0.x( -v1 ) ; break ;
                                case natus::device::mapping_detail::positive_x: v0.x( +v1 ) ; break ;
                                case natus::device::mapping_detail::negative_y: v0.y( -v1 ) ; break ;
                                case natus::device::mapping_detail::positive_y: v0.y( +v1 ) ; break ;
                                case natus::device::mapping_detail::one_to_one: // not possible
                                default: 
                                    natus::log::global_t::warning( "ascii -> stick mapping not possible. use mapping_detail." ) ;
                                    break ;
                                }

                                *a = v0 ;
                                *a = ss  ;
                            }
                        } ;
                        return true ;
                    }
                }

                natus::log::global_t::warning( "[X-to-stick] - no appropriate device component mapping found/possible" ) ;

                return false ;
            }

            template<>
            bool_t b_to_a<comp_point_t>( comp_t* ic_a, comp_t* ic_b, 
                natus::device::mapping_detail const, mapping_funk_t& funk_out )
            {
                using a_t = comp_point_t ;

                // point to point
                {
                    using b_t = comp_point_t ;
                    b_t* comp = dynamic_cast< b_t* >( ic_b ) ;
                    if( natus::core::is_not_nullptr( comp ) )
                    {
                        funk_out = [&] ( comp_t* a_, comp_t const* b_ )
                        {
                            a_t* a = reinterpret_cast< a_t* >( a_ ) ;
                            b_t const* b = reinterpret_cast< b_t const* >( b_ ) ;
                            if( b->has_changed() ) *a = *b ;
                        } ;
                        return true ;
                    }
                }
                // stick to point
                {
                    using b_t = comp_stick_t ;
                    b_t* comp = dynamic_cast< b_t* >( ic_b ) ;
                    if( natus::core::is_not_nullptr( comp ) )
                    {
                        funk_out = [&] ( comp_t* a_, comp_t const* b_ )
                        {
                            a_t* a = reinterpret_cast< a_t* >( a_ ) ;
                            b_t const* b = reinterpret_cast< b_t const* >( b_ ) ;

                            // add the relative stick to the absolute point
                            if( b->state() != natus::device::components::stick_state::none )
                            {
                                *a = a->value() + b->value() ;
                            }
                        } ;
                        return true ;
                    }
                }

                natus::log::global_t::warning( "[X-to-point] - no appropriate device component mapping found/possible" ) ;

                return false ;
            }

            template<>
            bool_t b_to_a<comp_ascii_t>( comp_t* ic_a, comp_t* ic_b, 
                natus::device::mapping_detail const, mapping_funk_t& funk_out )
            {
                using a_t = comp_ascii_t ;

                // ascii to ascii
                {
                    using b_t = comp_ascii_t ;
                    b_t* comp = dynamic_cast< b_t* >( ic_b ) ;
                    if( natus::core::is_not_nullptr( comp ) )
                    {
                        funk_out = [&] ( comp_t* a_, comp_t const* b_ )
                        {
                            a_t* a = reinterpret_cast< a_t* >( a_ ) ;
                            b_t const* b = reinterpret_cast< b_t const* >( b_ ) ;
                            if( b->state() != natus::device::components::key_state::none )
                                *a = *b ;
                        } ;
                        return true ;
                    }
                }
                // button to ascii
                {
                    using b_t = comp_button_t ;
                    b_t* comp = dynamic_cast< b_t* >( ic_b ) ;
                    if( natus::core::is_not_nullptr( comp ) )
                    {
                        funk_out = [&] ( comp_t* a_, comp_t const* b_ )
                        {
                            a_t* a = reinterpret_cast< a_t* >( a_ ) ;
                            b_t const* b = reinterpret_cast< b_t const* >( b_ ) ;

                            natus::device::components::key_state bs = components::key_state::none ;

                            switch( b->state() )
                            {
                            case natus::device::components::button_state::pressed: bs = components::key_state::pressed ; break ;
                            case natus::device::components::button_state::pressing: bs = components::key_state::pressing ; break ;
                            case natus::device::components::button_state::released: bs = components::key_state::released ; break ;
                            default: break ;
                            }

                            if( bs != components::key_state::none )
                            {
                                *a = bs ;
                                *a = b->value() ;
                            }
                        } ;
                        return true ;
                    }
                }

                natus::log::global_t::warning( "[X-to-ascii] - no appropriate device component mapping found/possible" ) ;

                return false ;
            }

            template<>
            static bool_t b_to_a<comp_button_t>( comp_t* ic_a, comp_t* ic_b, 
                natus::device::mapping_detail const, mapping_funk_t& funk_out )
            {
                using a_t = comp_button_t ;

                // button to button
                {
                    using b_t = comp_button_t ;
                    b_t* comp = dynamic_cast< b_t* >( ic_b ) ;
                    if( natus::core::is_not_nullptr( comp ) )
                    {
                        funk_out = [&] ( comp_t* a_, comp_t const* b_ )
                        {
                            a_t* a = reinterpret_cast< a_t* >( a_ ) ;
                            b_t const* b = reinterpret_cast< b_t const* >( b_ ) ;
                            if( b->state() != natus::device::components::button_state::none )
                                *a = *b ;
                            
                        } ;
                        return true ;
                    }
                }
                // ascii key to button
                {
                    using b_t = comp_ascii_t ;
                    b_t* comp = dynamic_cast< b_t* >( ic_b ) ;
                    if( natus::core::is_not_nullptr( comp ) )
                    {
                        funk_out = [&] ( comp_t* a_, comp_t const* b_ )
                        {
                            a_t* a = reinterpret_cast< a_t* >( a_ ) ;
                            b_t const* b = reinterpret_cast< b_t const* >( b_ ) ;

                            natus::device::components::button_state bs = components::button_state::none ;

                            switch( b->state() )
                            {
                            case natus::device::components::key_state::pressed: bs = components::button_state::pressed ; break ;
                            case natus::device::components::key_state::pressing: bs = components::button_state::pressing ; break ;
                            case natus::device::components::key_state::released: bs = components::button_state::released ; break ;
                            default: break ;
                            }

                            if( bs != components::button_state::none )
                            {
                                *a = bs ;
                                *a = b->value() ;
                            }
                        } ;
                        return true ;
                    }
                }

                natus::log::global_t::warning( "[X-to-button] - no appropriate device component mapping found/possible" ) ;

                return false ;
            }

            template<>
            static bool_t b_to_a<void_t>( comp_t* ic_a, comp_t* ic_b, 
                natus::device::mapping_detail const det, mapping_funk_t& funk_out )
            {
                // inputs
                if( natus::core::is_not_nullptr( dynamic_cast< comp_button_t* >( ic_a ) ) )
                {
                    return b_to_a<comp_button_t>( ic_a, ic_b, det, funk_out ) ;
                }
                if( natus::core::is_not_nullptr( dynamic_cast< comp_ascii_t* >( ic_a ) ) )
                {
                    return b_to_a<comp_ascii_t>( ic_a, ic_b, det, funk_out ) ;
                }
                if( natus::core::is_not_nullptr( dynamic_cast< comp_point_t* >( ic_a ) ) )
                {
                    return b_to_a<comp_point_t>( ic_a, ic_b, det, funk_out ) ;
                }
                if( natus::core::is_not_nullptr( dynamic_cast< comp_scroll_t* >( ic_a ) ) )
                {
                    return b_to_a<comp_scroll_t>( ic_a, ic_b, det, funk_out ) ;
                }
                if( natus::core::is_not_nullptr( dynamic_cast< comp_stick_t* >( ic_a ) ) )
                {
                    return b_to_a<comp_stick_t>( ic_a, ic_b, det, funk_out ) ;
                }

                // outputs
                if( natus::core::is_not_nullptr( dynamic_cast< comp_motor_t* >( ic_a ) ) )
                {
                    return b_to_a<comp_motor_t>( ic_a, ic_b, det, funk_out ) ;
                }

                return false ;
            }
        }

        class imapping 
        { 
        private:

            natus::std::string_t _name ;

        public:

            imapping( natus::std::string_cref_t name ) : _name(name){}
            imapping( imapping && rhv ) : _name( ::std::move(rhv._name) ){}
            imapping( imapping const & rhv ) : _name( rhv._name ) {}
            virtual ~imapping( void_t ){}
            
            natus::std::string_cref_t name( void_t ) const noexcept { return _name ; }

        protected:

            void_t set_name( natus::std::string_cref_t name ) noexcept { _name = name ; }
            natus::std::string_t move_name( void_t ) noexcept { return ::std::move( _name ) ;  }

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

            mapping( natus::std::string_cref_t name, device_a_res_t to, device_b_res_t from ) : 
                imapping( name ), _a( to ), _b( from ) {}
            mapping( this_cref_t rhv ) : imapping(rhv), _a( rhv._a), _b(rhv._b), 
                _inputs(rhv._inputs), _outputs(rhv._outputs) {}
            mapping( this_rref_t rhv ) : imapping( ::std::move( rhv ) ) 
            {
                _a = ::std::move( rhv._a ) ;
                _b = ::std::move( rhv._b ) ;
                _inputs = ::std::move( rhv._inputs ) ;
                _outputs = ::std::move( rhv._outputs ) ;
            }

            this_ref_t operator = ( this_cref_t rhv ) noexcept
            {
                this_t::set_name( rhv.name() ) ;

                _a = rhv._a ;
                _b = rhv._b ;
                _inputs = rhv._inputs ;
                _outputs = rhv._outputs ;
                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv ) noexcept
            {
                this_t::set_name( rhv.move_name() ) ;

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
                typename device_b_t::layout_t::input_component const b,
                natus::device::mapping_detail const det = mapping_detail::one_to_one ) noexcept
            {
                auto * ic_a = _a->get_component( a ) ;
                auto * ic_b = _b->get_component( b ) ;

                detail::mapping_funk_t funk ;
                if( detail::b_to_a<void_t>( ic_a, ic_b, det, funk ) )
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