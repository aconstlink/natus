
#pragma once

#include "../component.hpp"

namespace natus
{
    namespace device
    {
        namespace components
        {
            enum class key_state
            {
                none,
                pressed,
                pressing,
                released
            };

            static bool_t is_valid( key_state ks ) { return ks != key_state::none ; }
            static bool_t is_invalid( key_state ks ) { return ks == key_state::none ; }

            template< typename T >
            class key : public input_component
            {
                natus_this_typedefs( key ) ;
                natus_typedefs( T, key_type ) ;

            private:

                key_type_t _key_type ;
                natus::device::components::key_state _s ;
                float_t _value ;

            public:

                key( void_t )
                {
                }

                key( key_type_t kt ) noexcept :  _key_type( kt )
                {
                }

                key( this_cref_t rhv ) noexcept : input_component( rhv )
                {
                    _s = rhv._s ;
                    _value = rhv._value ;
                    _key_type = rhv._key_type ;
                }

                key( this_rref_t rhv ) noexcept : input_component( ::std::move(rhv) )
                {
                    _s = rhv._s ; rhv._s = natus::device::components::key_state::none ;
                    _key_type = rhv._key_type ;
                    _value = rhv._value ;
                }

                virtual ~key( void_t ) noexcept {}

            public:

                this_ref_t operator = ( float_t const v ) noexcept
                {
                    _value = v ;
                    return *this ;
                }

                this_ref_t operator = ( key_state const bs ) noexcept
                {
                    _s = bs ;
                    return *this ;
                }

                this_ref_t operator = ( this_t::key_type_t const t ) noexcept
                {
                    _key_type = k ;
                    return *this ;
                }

                float_t value ( void_t ) const noexcept { return _value ; }
                natus::device::components::key_state state( void_t ) const noexcept { return _s ; }
                this_t::key_type_t key_type( void_t ) const noexcept { return _key_type ; }
            };
        }
    }
}