
#pragma once

#include "../component.hpp"

namespace natus
{
    namespace device
    {
        namespace components
        {
            enum class motor_state
            {
                none,
                on,
                running,
                off,
                num_states
            };

            static natus::std::string_cref_t to_string( natus::device::components::motor_state const s ) noexcept
            {
                using bs_t = natus::device::components::motor_state ;
                static natus::std::string_t __states[] = { "none", "on", "running", "off", "invalid" } ;
                size_t const i = size_t( s ) >= size_t( bs_t::num_states ) ? size_t( bs_t::num_states ) : size_t( s ) ;
                return __states[ i ] ;
            }

            class motor : public output_component
            {
                natus_this_typedefs( motor ) ;

            private:

                natus::device::components::motor_state _s ;
                float_t _value = 0.0f ;

            public:

                motor( void_t )
                {
                }

                motor( this_cref_t rhv ) noexcept : output_component( rhv )
                {
                    _s = rhv._s ;
                    _value = rhv._value ;
                }

                motor( this_rref_t rhv ) noexcept : output_component( ::std::move( rhv ) )
                {
                    _s = rhv._s ; rhv._s = natus::device::components::motor_state::none ;
                    _value = rhv._value ;
                }

                virtual ~motor( void_t ) noexcept {}

            public:

                this_ref_t operator = ( float_t const v ) noexcept
                {
                    _value = v ;
                    return *this ;
                }

                this_ref_t operator = ( natus::device::components::motor_state const s ) noexcept
                {
                    _s = s ;
                    return *this ;
                }

                float_t value ( void_t ) const noexcept { return _value ; }
                motor_state state( void_t ) const noexcept { return _s ; }

                virtual void_t update( void_t ) noexcept final
                {
                    if( _s == natus::device::components::motor_state::on )
                    {
                        _s = natus::device::components::motor_state::running ;
                    }
                    else if( _s == natus::device::components::motor_state::off )
                    {
                        _s = natus::device::components::motor_state::none ;
                    }
                }
            };
            natus_typedef( motor ) ;
        }
    }
}
