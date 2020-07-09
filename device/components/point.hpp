
#pragma once

#include "../component.hpp"

#include <natus/math/vector/vector2.hpp>

namespace natus
{
    namespace device
    {
        namespace components
        {
            class point : public input_component
            {
                natus_this_typedefs( point ) ;

            private:
                
                natus::math::vec2f_t _value ;

            public:

                point( void_t )
                {
                }

                point( this_cref_t rhv ) noexcept : input_component( rhv )
                {
                    _value = rhv._value ;
                }

                point( this_rref_t rhv ) noexcept : input_component( ::std::move( rhv ) )
                {
                    _value = rhv._value ;
                }

                virtual ~point( void_t ) noexcept {}

            public:

                this_ref_t operator = ( natus::math::vec2f_cref_t v ) noexcept
                {
                    _value = v ;
                    return *this ;
                }

                natus::math::vec2f_cref_t value ( void_t ) const noexcept { return _value ; }
            };
            natus_typedef( point ) ;
        }
    }
}