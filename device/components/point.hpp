
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
                
                natus::math::vec2f_t _old ;
                natus::math::vec2f_t _value ;
                bool_t _changed = false ;

            public:

                point( void_t )
                {
                }

                point( this_cref_t rhv ) noexcept : input_component( rhv )
                {
                    _old = rhv._old ;
                    _value = rhv._value ;
                    _changed = rhv._changed ;
                }

                point( this_rref_t rhv ) noexcept : input_component( ::std::move( rhv ) )
                {
                    _old = rhv._old ;
                    _value = rhv._value ;
                    _changed = rhv._changed ;
                }

                virtual ~point( void_t ) noexcept {}

            public:

                this_ref_t operator = ( natus::math::vec2f_cref_t v ) noexcept
                {
                    _value = v ;
                    _changed = true ;
                    return *this ;
                }

                this_ref_t operator = ( this_cref_t rhv ) noexcept 
                {
                    _value = rhv._value ;
                    _changed = rhv._changed ;

                    return *this ;
                }

                natus::math::vec2f_t rel( void_t ) const noexcept { return _value - _old ; }
                natus::math::vec2f_cref_t value ( void_t ) const noexcept { return _value ; }
                bool_t has_changed( void_t ) const noexcept { return _changed ; }

                virtual void_t update( void_t ) noexcept final
                {
                    _old = _value ;
                    _changed = false ;
                }
            };
            natus_typedef( point ) ;
        }
    }
}