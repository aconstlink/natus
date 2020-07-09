
#pragma once

#include "../component.hpp"

namespace natus
{
    namespace device
    {
        namespace components
        {
            // scroll component for vertical/horizontal scroll
            // relative value
            // value == 0.0f : no scroll
            // value > 0.0f : up/right scroll
            // value < 0.0f : down/left scroll
            class scroll : public input_component
            {
                natus_this_typedefs( scroll ) ;

            private:

                float_t _value = 0.0f ;

            public:

                scroll( void_t )
                {
                }

                scroll( this_cref_t rhv ) noexcept : input_component( rhv )
                {
                    _value = rhv._value ;
                }

                scroll( this_rref_t rhv ) noexcept : input_component( ::std::move( rhv ) )
                {
                    _value = rhv._value ;
                }

                virtual ~scroll( void_t ) noexcept {}

            public:

                this_ref_t operator = ( float_t const v ) noexcept
                {
                    _value = v ;
                    return *this ;
                }

                float_t value ( void_t ) const noexcept { return _value ; }
            };
            natus_typedef( scroll ) ;
        }
    }
}