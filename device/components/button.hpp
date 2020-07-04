
#pragma once

#include "../component.hpp"

namespace natus
{
    namespace device
    {
        namespace components
        {
            enum class button_state
            {
                none,
                pressed,
                pressing,
                released
            };

            class button : public input_component
            {
                natus_this_typedefs( button ) ;

            private:

                natus::device::components::button_state _bs ;
                float_t _value = 0.0f ;

            public:

                button( void_t ) 
                {
                }

                button( natus::std::string_cref_t name, size_t const id ) noexcept : input_component( name, id )
                {
                }

                button( this_cref_t rhv ) noexcept : input_component( rhv )
                {
                    _bs = rhv._bs ;
                    _value = rhv._value ;
                }

                button( this_rref_t rhv ) noexcept : input_component( ::std::move(rhv) )
                {
                    _bs = rhv._bs ; rhv._bs = natus::device::components::button_state::none ;
                    _value = rhv._value ; 
                }

                virtual ~button( void_t ) noexcept {}

            public:

                this_ref_t operator = ( float_t const v ) noexcept
                {
                    _value = v ;
                    return *this ;
                }

                this_ref_t operator = ( button_state const bs ) noexcept
                {
                    _bs = bs ;
                    return *this ;
                }

                float_t value ( void_t ) const noexcept { return _value ; }
            };
            natus_typedef( button ) ;
        }
    }
}