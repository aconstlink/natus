
#pragma once

#include "../component.hpp"

namespace natus
{
    namespace device
    {
        namespace components
        {
            namespace keys
            {
                enum class ascii_key
                {
                    invalid,
                    escape,
                    f1,
                    f2,
                    f3,
                    f4,
                    f5,
                    f6,
                    f7,
                    f8,
                    f9,
                    f10,
                    f11,
                    f12,
                    f13,
                    f14,
                    print,
                    pause,
                    a,
                    b,
                    c,
                    d,
                    e,
                    f,
                    g,
                    h,
                    i,
                    j,
                    k,
                    l,
                    m,
                    n,
                    o,
                    p,
                    q,
                    r,
                    s,
                    t,
                    u,
                    v,
                    w,
                    x,
                    y,
                    z,
                    k_0,
                    k_1,
                    k_2,
                    k_3,
                    k_4,
                    k_5,
                    k_6,
                    k_7,
                    k_8,
                    k_9,
                    num_0,
                    num_1,
                    num_2,
                    num_3,
                    num_4,
                    num_5,
                    num_6,
                    num_7,
                    num_8,
                    num_9,
                    num_komma,
                    num_sub,
                    num_add,
                    num_mult,
                    num_div,
                    num_caps,
                    num_return,
                    shift_left,
                    shift_right,
                    ctrl_left,
                    ctrl_right,
                    alt_left,
                    alt_right,
                    k_return,
                    back_space,
                    space,
                    tab,
                    tab_lock,
                    sharp,
                    plus,
                    minus,
                    komma,
                    point,
                    insert,
                    k_delete,
                    pos1,
                    sk_end,
                    one_up,
                    one_down,
                    arrow_left,
                    arrow_right,
                    arrow_up,
                    arrow_down,
                    lower_than,
                    // os specific?
                    context,
                    // windows spec
                    win_win_left,
                    win_win_right,
                    // mac spec
                    mac_control,

                    // last key
                    num_keys
                };

                static bool_t convert_key_to_ascii_char( ascii_key const k, natus::core::types::char_ref_t c )
                {
                    if( k >= ascii_key::a && k <= ascii_key::z )
                    {
                        c = 'a' + char_t( k ) - char_t( ascii_key::a ) ;
                        return true ;
                    }
                    return false ;
                }

                static bool_t convert_key_to_ascii_number( ascii_key const k, char_ref_t c )
                {
                    if( k >= ascii_key::k_0 && k <= ascii_key::k_9 )
                    {
                        c = '0' + char_t( k ) - char_t( ascii_key::k_0 ) ;
                        return true ;
                    }
                    return false ;
                }

                static ascii_key convert_ascii_number_keys( size_t const delta )
                {
                    bool_t const cond = delta <= ( size_t( ascii_key::k_9 ) - size_t( ascii_key::k_0 ) ) ;
                    natus::log::global::error_and_exit( natus::core::is_not( cond ),
                        "[so_device::convert_ascii_number_keys]" ) ;

                    return ascii_key( size_t( ascii_key::k_0 ) + delta ) ;
                }

                static ascii_key convert_ascii_letter_keys( size_t const delta )
                {
                    bool_t const cond = delta <= ( size_t( ascii_key::z ) - size_t( ascii_key::a ) ) ;
                    natus::log::global::error_and_exit( natus::core::is_not( cond ),
                        "[so_device::convert_ascii_letter_keys]" ) ;

                    return ascii_key( size_t( ascii_key::a ) + delta ) ;
                }

                static ascii_key convert_ascii_function_keys( size_t const delta )
                {
                    bool_t const cond = delta <= ( size_t( ascii_key::f14 ) - size_t( ascii_key::f1 ) ) ;
                    natus::log::global::error_and_exit( natus::core::is_not( cond ),
                        "[so_device::convert_ascii_function_keys]" ) ;

                    return ascii_key( size_t( ascii_key::f1 ) + delta ) ;
                }

                static ascii_key convert_ascii_numpad_number_keys( size_t const delta )
                {
                    bool_t const cond = delta <= ( size_t( ascii_key::num_9 ) - size_t( ascii_key::num_0 ) ) ;
                    natus::log::global::error_and_exit( natus::core::is_not( cond ),
                        "[so_device::convert_ascii_numpad_number_keys]" ) ;

                    return ascii_key( size_t( ascii_key::num_0 ) + delta ) ;
                }
            }

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

                key( natus::std::string_cref_t name, size_t const id, key_type_t kt ) noexcept : input_component( name, id ), _key_type( kt )
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
            };
            natus_typedefs( key<keys::ascii_key>, ascii_key ) ;
        }
    }
}