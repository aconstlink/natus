
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
                released,
                num_keys
            };

            static natus::ntd::string_cref_t to_string( natus::device::components::key_state const ks ) noexcept
            {
                using ks_t = natus::device::components::key_state ;
                static natus::ntd::string_t __key_states[] = { "none", "pressed", "pressing", "released", "invalid" } ;
                size_t const i = size_t( ks ) >= size_t( ks_t::num_keys ) ? size_t( ks_t::num_keys ) : size_t( ks ) ;
                return __key_states[ i ] ;
            }

            static bool_t is_valid( key_state ks ) { return ks != key_state::none ; }
            static bool_t is_invalid( key_state ks ) { return ks == key_state::none ; }

            template< typename T >
            class key : public input_component
            {
                natus_this_typedefs( key ) ;
                natus_typedefs( T, key_type ) ;

            private:

                key_type_t _key_type ;
                natus::device::components::key_state _s = natus::device::components::key_state::none ;
                float_t _value = 0.0f ;

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

                this_ref_t operator = ( this_t::key_type_t const k ) noexcept
                {
                    _key_type = k ;
                    return *this ;
                }

                this_ref_t operator = ( this_cref_t const rhv ) noexcept
                {
                    _key_type = rhv._key_type ;
                    _value = rhv._value ;
                    _s = rhv._s ;
                    return *this ;
                }

                float_t value ( void_t ) const noexcept { return _value ; }
                natus::device::components::key_state state( void_t ) const noexcept { return _s ; }
                this_t::key_type_t key_type( void_t ) const noexcept { return _key_type ; }


                virtual void_t update( void_t ) noexcept final
                {
                    using ks_t = natus::device::components::key_state ;

                    if( _s == ks_t::pressed )
                    {
                        _s = ks_t::pressing ;
                    }
                    else if( _s == ks_t::released )
                    {
                        _s = ks_t::none ;
                    }
                }
            };
        }
    }
}
