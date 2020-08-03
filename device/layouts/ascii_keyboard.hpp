
#pragma once

#include "../typedefs.h"

#include "../components/key.hpp"

namespace natus
{
    namespace device
    {
        namespace layouts
        {
            class ascii_keyboard
            {
                natus_this_typedefs( ascii_keyboard ) ;
                natus_soil_typedefs( natus::device::device< this_t >, this_device ) ;

                friend this_device_t ;

            public:

                enum class ascii_key
                {
                    none,
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
                natus_typedefs( natus::device::components::key< ascii_key >, ascii_component ) ;

                static natus::std::string_cref_t to_string( ascii_key const k ) noexcept
                {
                    static natus::std::string_t __keys[] = { "none", "escape", "f1", "f2", "f3", "f4", "f5", "f6", "f7",
                        "f8", "f9", "f10", "f11", "f12", "f13", "f14", "print", "pause", "a", "b", "c", "d", "e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z","k_0","k_1","k_2","k_3","k_4","k_5","k_6","k_7","k_8","k_9","num_0","num_1","num_2","num_3","num_4","num_5","num_6","num_7","num_8","num_9","num_komma","num_sub","num_add","num_mult","num_div","num_caps","num_return","shift_left","shift_right","ctrl_left","ctrl_right","alt_left","alt_right","k_return","back_space","space","tab","tab_lock","sharp","plus","minus","komma","point","insert","k_delete","pos1","sk_end","one_up","one_down","arrow_left","arrow_right","arrow_up","arrow_down","lower_than","context","win_win_left","win_win_right","mac_control", "invalid"} ;

                    size_t const i = size_t( k ) >= size_t( ascii_key::num_keys ) ? size_t( ascii_key::num_keys ) : size_t( k );
                    return __keys[ i ] ;
                }

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

                

            public:

                enum class input_component
                {
                    num_components = size_t( this_t::ascii_key::num_keys )
                };

                enum class output_component
                {
                    num_components
                };

                static input_component ascii_key_to_input_component( this_t::ascii_key const k )
                {
                    return input_component( size_t( k ) ) ;
                }

            private:

                static void_t init_components( this_t::this_device_ref_t dev )
                {
                    using key_t = natus::device::components::key<this_t::ascii_key> ;

                    // inputs
                    for( size_t i=0; i<size_t(input_component::num_components); ++i )
                    {
                        auto const key = key_t( this_t::ascii_key( i ) ) ;
                        dev.add_input_component( key ) ;
                    }
                    
                    // outputs
                }

            private:

                this_t::this_device_res_t _dev ;

            public:

                ascii_keyboard( this_t::this_device_res_t dev ) : _dev( ::std::move( dev ) )
                {
                    if( !_dev.is_valid() )
                    {
                        _dev = this_device_t() ;
                    }
                }
                ascii_keyboard( this_cref_t rhv ) : _dev( rhv._dev ) {}
                ascii_keyboard( this_rref_t rhv ) : _dev( ::std::move( rhv._dev) ){}
                ~ascii_keyboard( void_t ) {}

            private:

                this_t::ascii_component_ptr_t get_component( this_t::ascii_key const k ) noexcept
                {
                    this_t::input_component const i = this_t::input_component( size_t( k ) ) ;
                    return _dev->get_component< this_t::ascii_component_t >( i ) ;
                }

                this_t::ascii_component_cptr_t get_component( this_t::ascii_key const k ) const noexcept
                {
                    this_t::input_component const i = this_t::input_component( size_t( k ) ) ;
                    return _dev->get_component< this_t::ascii_component_t >( i ) ;
                }

            public:

                this_ref_t set_state( this_t::ascii_key const k, natus::device::components::key_state const ks ) noexcept
                {
                    *this_t::get_component(k) = ks ;
                    return *this ;
                }

                natus::device::components::key_state get_state( this_t::ascii_key const k ) const noexcept
                {
                    return this_t::get_component(k)->state() ;
                }
            };
            natus_typedef( ascii_keyboard ) ;
        }
        natus_soil_typedefs( natus::device::device< natus::device::layouts::ascii_keyboard>, ascii_device ) ;
        natus_soil_typedefs( natus::device::device< natus::device::layouts::ascii_keyboard>, ascii_keyboard_device ) ;
    }
}
