
#include "xinput_module.h"

#include <natus/math/vector/vector2.hpp>

#include <windows.h>
#include <Xinput.h>

#include <array>

using namespace natus::device ;
using namespace natus::device::win32 ;

// 
// For class definition see far below
//


//
// 1. Xinput Helper
//
class xinput_module::xinput_device
{
    natus_this_typedefs( xinput_device ) ;

private:

    /// the xinput device id.
    DWORD _id = DWORD( -1 ) ;

    /// the current state
    XINPUT_STATE _state ;


public:

    xinput_device( void_t )
    {
        memset( &_state, 0, sizeof( XINPUT_STATE ) ) ;
    }

    xinput_device( this_cref_t ) = delete ;
    xinput_device( this_rref_t rhv )
    {
        _id = rhv._id ;
        rhv._id = DWORD( -1 ) ;

        memcpy( &_state, &rhv._state, sizeof( XINPUT_STATE ) ) ;
    }
    xinput_device( DWORD id )
    {
        _id = id ;
        memset( &_state, 0, sizeof( XINPUT_STATE ) ) ;
    }



public:

    DWORD get_id( void_t )
    {
        return _id ;
    }

    /// check if a pressed event occurred.
    bool_t is_pressed( XINPUT_STATE const& new_state, DWORD button ) const
    {
        bool_t const old_button = bool_t( ( _state.Gamepad.wButtons & button ) != 0 ) ;
        bool_t const new_button = bool_t( ( new_state.Gamepad.wButtons & button ) != 0 ) ;

        return !old_button && new_button ;
    }

    /// check if a pressing event occurred.
    bool_t is_pressing( XINPUT_STATE const& new_state, DWORD button ) const
    {
        bool_t const old_button = bool_t( ( _state.Gamepad.wButtons & button ) != 0 ) ;
        bool_t const new_button = bool_t( ( new_state.Gamepad.wButtons & button ) != 0 ) ;

        return old_button && new_button ;
    }

    /// check if a released event occurred.
    bool_t is_released( XINPUT_STATE const& new_state, DWORD button ) const
    {
        bool_t const old_button = bool_t( ( _state.Gamepad.wButtons & button ) != 0 ) ;
        bool_t const new_button = bool_t( ( new_state.Gamepad.wButtons & button ) != 0 ) ;

        return old_button && !new_button ;
    }

    /// check the state of the left trigger
    natus::device::components::button_state check_left_trigger(
        XINPUT_STATE const& new_state, uint16_t& intensity_out ) const
    {
        bool_t const old_press = bool_t( _state.Gamepad.bLeftTrigger != 0 ) ;
        bool_t const new_press = bool_t( new_state.Gamepad.bLeftTrigger != 0 ) ;

        intensity_out = uint16_t( new_state.Gamepad.bLeftTrigger ) ;

        if( new_press ) return natus::device::components::button_state::pressed ;
        if( old_press && !new_press ) return natus::device::components::button_state::released ;

        return natus::device::components::button_state::none ;
    }

    /// check the state of the right trigger
    natus::device::components::button_state check_right_trigger(
        XINPUT_STATE const& new_state, uint16_t& intensity_out ) const
    {
        bool_t const old_press = bool_t( _state.Gamepad.bRightTrigger != 0 ) ;
        bool_t const new_press = bool_t( new_state.Gamepad.bRightTrigger != 0 ) ;

        intensity_out = uint16_t( new_state.Gamepad.bRightTrigger ) ;

        if( new_press ) return natus::device::components::button_state::pressed ;
        if( old_press && !new_press ) return natus::device::components::button_state::released ;
        return natus::device::components::button_state::none ;
    }


    natus::device::components::stick_state check_left_stick(
        XINPUT_STATE const& new_state, natus::math::vec2f_t& nnc_out ) const
    {
        natus::math::vec2b_t const old_tilt = natus::math::vec2b_t(
            _state.Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE || _state.Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE,
            _state.Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE || _state.Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ) ;
        natus::math::vec2b_t const new_tilt = natus::math::vec2b_t(
            new_state.Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE || new_state.Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE,
            new_state.Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE || new_state.Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ) ;

        // according to ms doc, max range is in [0, 1<<16]
        // using range [0,1<<15] to get [-1,1] value instead of [-0.5,0.5] values.
        natus::math::vec2f_t const inv_width =
            natus::math::vec2f_t( 1.0f / float_t( ( 1 << 15 ) - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ) ) ;

        natus::math::vector2< SHORT > const tmp = natus::math::vector2< SHORT >(
            new_state.Gamepad.sThumbLX, new_state.Gamepad.sThumbLY ).
            dead_zone( XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ) ;

        SHORT X = tmp.x() ;
        SHORT Y = tmp.y() ;

        X -= X > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ? XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE : 0 ;
        X += X < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ? XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE : 0 ;

        Y -= Y > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ? XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE : 0 ;
        Y += Y < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ? XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE : 0 ;

        nnc_out = natus::math::vec2f_t( float_t( X ), float_t( Y ) ) * inv_width ;

        if( !old_tilt.any() && new_tilt.any() ) return natus::device::components::stick_state::tilted ;
        if( old_tilt.any() && new_tilt.any() ) return natus::device::components::stick_state::tilted ;
        if( old_tilt.any() && !new_tilt.any() ) return natus::device::components::stick_state::untilted ;

        return natus::device::components::stick_state::none ;
    }

    natus::device::components::stick_state check_left_stick(
        XINPUT_STATE const& new_state, natus::math::vector2<int16_t>& val_out ) const
    {
        natus::math::vector2<int16_t> old_xy(
            _state.Gamepad.sThumbLX, _state.Gamepad.sThumbLY ) ;

        natus::math::vector2<int16_t> new_xy(
            new_state.Gamepad.sThumbLX, new_state.Gamepad.sThumbLY ) ;

        natus::math::vector2<int16_t> const dead_zone_xy(
            XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ) ;

        natus::math::vector2<int16_t> const new_xy_dif = new_xy.clamped( dead_zone_xy.negated(), dead_zone_xy ) ;
        natus::math::vector2<int16_t> const old_xy_dif = old_xy.clamped( dead_zone_xy.negated(), dead_zone_xy ) ;

        old_xy = old_xy + old_xy_dif.negated() ;
        new_xy = new_xy + new_xy_dif.negated() ;

        val_out = new_xy ;

        natus::math::vec2b_t const old_tilt = old_xy.absed().greater_than( natus::math::vec2i16_t( 0 ) ) ;
        natus::math::vec2b_t const new_tilt = new_xy.absed().greater_than( natus::math::vec2i16_t( 0 ) ) ;

        if( new_tilt.any() ) return natus::device::components::stick_state::tilted ;
        if( old_tilt.any() && !new_tilt.any() ) return natus::device::components::stick_state::untilted ;

        return natus::device::components::stick_state::none ;
    }

    natus::device::components::stick_state check_right_stick(
        XINPUT_STATE const& new_state, natus::math::vec2f_t& nnc_out ) const
    {
        natus::math::vec2b_t const old_tilt = natus::math::vec2b_t(
            _state.Gamepad.sThumbRX > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE || _state.Gamepad.sThumbRX < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE,
            _state.Gamepad.sThumbRY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE || _state.Gamepad.sThumbRY < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ) ;
        natus::math::vec2b_t const new_tilt = natus::math::vec2b_t(
            new_state.Gamepad.sThumbRX > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE || new_state.Gamepad.sThumbRX < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE,
            new_state.Gamepad.sThumbRY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE || new_state.Gamepad.sThumbRY < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ) ;

        // according to ms doc, max range is in [0, 1<<16]
        // using range [0,1<<15] to get [-1,1] value instead of [-0.5,0.5] values.
        natus::math::vec2f_t const inv_width =
            natus::math::vec2f_t( 1.0f / float_t( ( 1 << 15 ) - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ) ) ;

        natus::math::vector2< SHORT > const tmp = natus::math::vector2< SHORT >(
            new_state.Gamepad.sThumbRX, new_state.Gamepad.sThumbRY ).dead_zone( XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ) ;

        SHORT X = tmp.x() ;
        SHORT Y = tmp.y() ;

        X -= X > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ? XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE : 0 ;
        X += X < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ? XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE : 0 ;

        Y -= Y > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ? XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE : 0 ;
        Y += Y < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ? XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE : 0 ;

        nnc_out = natus::math::vec2f_t( float_t( X ), float_t( Y ) ) * inv_width ;

        if( !old_tilt.any() && new_tilt.any() ) return natus::device::components::stick_state::tilted ;
        if( old_tilt.any() && new_tilt.any() ) return natus::device::components::stick_state::tilted ;
        if( old_tilt.any() && !new_tilt.any() ) return natus::device::components::stick_state::untilted ;
        return natus::device::components::stick_state::none ;
    }

    natus::device::components::stick_state check_right_stick(
        XINPUT_STATE const& new_state, natus::math::vector2<int16_t>& val_out ) const
    {
        natus::math::vector2<int16_t> old_xy(
            _state.Gamepad.sThumbRX, _state.Gamepad.sThumbRY ) ;

        natus::math::vector2<int16_t> new_xy(
            new_state.Gamepad.sThumbRX, new_state.Gamepad.sThumbRY ) ;

        natus::math::vector2<int16_t> const dead_zone_xy(
            XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ) ;

        natus::math::vector2<int16_t> const new_xy_dif = new_xy.clamped( dead_zone_xy.negated(), dead_zone_xy ) ;
        natus::math::vector2<int16_t> const old_xy_dif = old_xy.clamped( dead_zone_xy.negated(), dead_zone_xy ) ;

        old_xy = old_xy + old_xy_dif.negated() ;
        new_xy = new_xy + new_xy_dif.negated() ;

        val_out = new_xy ;

        natus::math::vec2b_t const old_tilt = old_xy.absed().greater_than( natus::math::vec2i16_t( 0 ) ) ;
        natus::math::vec2b_t const new_tilt = new_xy.absed().greater_than( natus::math::vec2i16_t( 0 ) ) ;

        if( new_tilt.any() ) return natus::device::components::stick_state::tilted ;
        if( old_tilt.any() && !new_tilt.any() ) return natus::device::components::stick_state::untilted ;

        return natus::device::components::stick_state::none ;
    }

    /// exchange the old xinput state with the new one.
    void_t exchange_state( XINPUT_STATE const& new_state )
    {
        _state = new_state ;
    }
};

//
// 2. mappings
//

namespace this_file
{
    typedef natus::device::layouts::xbox_controller_t::input_component ic_t ;
    static ic_t map_button_xinput_to_component( DWORD const b )
    {
        ic_t ret = ic_t::num_components ;

        switch( b )
        {
        case XINPUT_GAMEPAD_BACK: ret = ic_t::button_back ;  break ;
        case XINPUT_GAMEPAD_START: ret = ic_t::button_start ;  break ;
        case XINPUT_GAMEPAD_A: ret = ic_t::button_a ;  break ;
        case XINPUT_GAMEPAD_B: ret = ic_t::button_b ;  break ;
        case XINPUT_GAMEPAD_X: ret = ic_t::button_x ;  break ;
        case XINPUT_GAMEPAD_Y: ret = ic_t::button_y ;  break ;
        case XINPUT_GAMEPAD_LEFT_THUMB: ret = ic_t::button_thumb_left ;  break ;
        case XINPUT_GAMEPAD_RIGHT_THUMB: ret = ic_t::button_thumb_right;  break ;
        case XINPUT_GAMEPAD_LEFT_SHOULDER: ret = ic_t::button_shoulder_left ;  break ;
        case XINPUT_GAMEPAD_RIGHT_SHOULDER: ret = ic_t::button_shoulder_right ;  break ;
        case XINPUT_GAMEPAD_DPAD_UP: ret = ic_t::button_dpad_up ;  break ;
        case XINPUT_GAMEPAD_DPAD_DOWN: ret = ic_t::button_dpad_down ;  break ;
        case XINPUT_GAMEPAD_DPAD_LEFT: ret = ic_t::button_dpad_left ;  break ;
        case XINPUT_GAMEPAD_DPAD_RIGHT: ret = ic_t::button_dpad_right ;  break ;
        default: break ;
        }

        return ret ;
    }

    typedef natus::device::layouts::xbox_controller_t::button button_t ;

    static button_t map_button_xinput_to_layout( DWORD const b )
    {
        button_t ret = button_t::none ;

        switch( b )
        {
        case XINPUT_GAMEPAD_BACK: ret = button_t::back ;  break ;
        case XINPUT_GAMEPAD_START: ret = button_t::start ;  break ;
        case XINPUT_GAMEPAD_A: ret = button_t::a ;  break ;
        case XINPUT_GAMEPAD_B: ret = button_t::b ;  break ;
        case XINPUT_GAMEPAD_X: ret = button_t::x ;  break ;
        case XINPUT_GAMEPAD_Y: ret = button_t::y ;  break ;
        default: break ;
        }

        return ret ;
    }

    typedef natus::device::layouts::xbox_controller_t::thumb thumb_t ;

    static thumb_t map_thumb_xinput_to_layout( DWORD const b )
    {
        thumb_t ret = thumb_t::none ;

        switch( b )
        {
        case XINPUT_GAMEPAD_LEFT_THUMB: ret = thumb_t::left ;  break ;
        case XINPUT_GAMEPAD_RIGHT_THUMB: ret = thumb_t::right ;  break ;
        default: break ;
        }

        return ret ;
    }

    typedef natus::device::layouts::xbox_controller_t::shoulder shoulder_t ;

    static shoulder_t map_shoulder_xinput_to_layout( DWORD const b )
    {
        shoulder_t ret = shoulder_t::none ;

        switch( b )
        {
        case XINPUT_GAMEPAD_LEFT_SHOULDER: ret = shoulder_t::left ;  break ;
        case XINPUT_GAMEPAD_RIGHT_SHOULDER: ret = shoulder_t::right ;  break ;
        default: break ;
        }

        return ret ;
    }

    typedef natus::device::layouts::xbox_controller_t::dpad dpad_t ;

    static dpad_t map_dpad_xinput_to_layout( DWORD const b )
    {
        dpad_t ret = dpad_t::none ;

        switch( b )
        {
        case XINPUT_GAMEPAD_DPAD_UP: ret = dpad_t::up ;  break ;
        case XINPUT_GAMEPAD_DPAD_DOWN: ret = dpad_t::down ;  break ;
        case XINPUT_GAMEPAD_DPAD_LEFT: ret = dpad_t::left ;  break ;
        case XINPUT_GAMEPAD_DPAD_RIGHT: ret = dpad_t::right ;  break ;
        default: break ;
        }

        return ret ;
    }
}

//***
xinput_module::xinput_module( void_t ) 
{
    #if defined( SNAKEOIL_OS_WIN8 )
    XInputEnable( true ) ;
    #endif

    this_t::init_gamepads() ;
}

//***
xinput_module::xinput_module( this_rref_t rhv ) 
{
    _devices = ::std::move( rhv._devices ) ;
}

//***
xinput_module::~xinput_module( void_t ) 
{
    #if defined( SNAKEOIL_OS_WIN8 )
    XInputEnable( false ) ;
    #endif

    for( auto item : _devices )
    {
        natus::memory::global_t::dealloc( item.xinput_ptr ) ;
    }
}

//***
xinput_module::this_ref_t xinput_module::operator = ( this_rref_t rhv ) 
{
    _devices = ::std::move( rhv._devices ) ;
    return *this ;
}

//***
void_t xinput_module::search( natus::device::imodule::search_funk_t ) 
{
}

//***
void_t xinput_module::update( void_t ) 
{
    for( auto& item : _devices )
    {
        natus::device::xbc_device_res_t & dev = item.dev ;
        xinput_device & helper = *item.xinput_ptr ;

        XINPUT_STATE state ;
        DWORD const res = XInputGetState( item.xinput_ptr->get_id(), &state ) ;
        if( natus::log::global_t::warning( res != ERROR_SUCCESS,
            natus_log_fn("XInputGetState") ) ) continue ;

        natus::device::layouts::xbox_controller_t ctrl( dev ) ;

        // buttons
        {
            static const ::std::array<DWORD, 6> buttons__ ( 
                {   XINPUT_GAMEPAD_BACK, XINPUT_GAMEPAD_START, XINPUT_GAMEPAD_A, 
                    XINPUT_GAMEPAD_B, XINPUT_GAMEPAD_X, XINPUT_GAMEPAD_Y } ) ;

            for( auto const & i : buttons__ )
            {
                auto const b = this_file::map_button_xinput_to_layout( i ) ;
                if( helper.is_pressed( state, i ) )
                {
                    *ctrl.get_component( b ) = natus::device::components::button_state::pressed ;
                }
                else if( helper.is_pressing( state, i ) )
                {
                    *ctrl.get_component( b ) = natus::device::components::button_state::pressing ;
                }
                else if( helper.is_released(state, i ) )
                {
                    *ctrl.get_component( b ) = natus::device::components::button_state::released ;
                }
            }
        }

        // triggers
        {}

        // stricks
        {}

        // motors
        {}



        //
        // must be done after the new state is consumed.
        // i.e. the gamepad received all new states
        //
        {
            helper.exchange_state( state ) ;
        }
    }
}

//***
void_t xinput_module::init_gamepads( void_t ) 
{
    // by XInput 1.4 spec. only 4 devices supported.
    for( DWORD i = 0; i < 4; ++i )
    {
        {
            XINPUT_CAPABILITIES caps ;
            DWORD res = XInputGetCapabilities( i, XINPUT_FLAG_GAMEPAD, &caps ) ;

            if( res == ERROR_DEVICE_NOT_CONNECTED ) continue ;

            if( natus::log::global_t::error( res != ERROR_SUCCESS,
                "[xinput_module::init_gamepads] : Error query controller caps." ) )
                continue ;

            if( natus::log::global_t::error( caps.Type != XINPUT_DEVTYPE_GAMEPAD,
                "[xinput_module::init_gamepads] : xinput device is not a gamepad." ) )
                continue ;

            natus::log::global_t::status( "[xinput_module::init_gamepads] : Device " +
                ::std::to_string( i ) + ": Gamepad found" ) ;
        }

        XINPUT_STATE state ;
        DWORD res = XInputGetState( i, &state ) ;
        if( res != ERROR_SUCCESS ) continue ;

        gamepad_data_t gd ;
        gd.xinput_ptr = natus::memory::global_t::alloc( xinput_device( i ),
            natus_log_fn("xinput_device") ) ;
        gd.xinput_ptr->exchange_state( state ) ;

        gd.dev = natus::device::xbc_device_t() ;

        _devices.push_back( gd ) ;
    }
}

