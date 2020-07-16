#include "xlib_module.h"

#include <natus/math/vector/vector2.hpp>

using namespace natus::device ;
using namespace natus::device::xlib ;

xlib_module::xlib_module( void_t ) 
{
    // init devices in xlib?
    natus::log::global_t::status("xlib module online" ) ;

    {
        _three_device = natus::device::three_device_t( "Xlib Three Button Mouse" ) ;
        _ascii_device = natus::device::ascii_device_t( "Xlib Ascii Keyboard" ) ;
    }
}

//***
xlib_module::xlib_module( this_rref_t rhv )
{
    _three_device = ::std::move( rhv._three_device ) ;
    _ascii_device = ::std::move( rhv._ascii_device ) ;
    _three_button_items = ::std::move( rhv._three_button_items ) ;
    _pointer_coords_global = ::std::move( rhv._pointer_coords_global ) ;
    _pointer_coords_local = ::std::move( rhv._pointer_coords_local ) ;
    _scroll_items = ::std::move( rhv._scroll_items ) ;
}

//***
xlib_module::this_ref_t xlib_module::operator = ( this_rref_t rhv ) 
{
    _three_device = ::std::move( rhv._three_device ) ;
    _ascii_device = ::std::move( rhv._ascii_device ) ;
    _three_button_items = ::std::move( rhv._three_button_items ) ;
    _pointer_coords_global = ::std::move( rhv._pointer_coords_global ) ;
    _pointer_coords_local = ::std::move( rhv._pointer_coords_local ) ;
    _scroll_items = ::std::move( rhv._scroll_items ) ;
    return *this ;
}

//***
xlib_module::~xlib_module( void_t )
{
}

//***
void_t xlib_module::search( natus::device::imodule::search_funk_t funk )
{
    funk( _three_device ) ;
    funk( _ascii_device ) ;
}

//***
void_t xlib_module::update( void_t )
{
    // check for plug and play
    // update all devices

    // 1. update components
    {
        _three_device->update_components() ;
        _ascii_device->update_components() ;
    }

    // mouse
    {
        natus::device::layouts::three_mouse_t mouse( _three_device ) ;

        natus::concurrent::lock_t lk( _buffer_mtx ) ;

        // insert new events
        {
            for( auto const& item : _three_button_items )
            {
                auto* btn = mouse.get_component( item.first ) ;

                *btn = item.second ;
            }

            if( _pointer_coords_global.size() > 0 )
            {
                auto* coord = mouse.get_global_component() ;

                *coord = _pointer_coords_global.back() ;
            }

            if( _pointer_coords_local.size() > 0 )
            {
                auto* coord = mouse.get_local_component() ;
                *coord = _pointer_coords_local.back() ;
            }

            if( _scroll_items.size() > 0 )
            {
                auto* scroll = mouse.get_scroll_component() ;
                
                if( _scroll_items.back() == 65416 ) *scroll = -1.0f ;
                else if( _scroll_items.back() == 120 ) *scroll = 1.0f ;
                else *scroll = 0.0f ;
            }
        }

        _three_button_items.clear() ;
        _pointer_coords_global.clear() ;
        _pointer_coords_local.clear() ;
        _scroll_items.clear() ;
    }

    // keybaord
    {
        natus::device::layouts::ascii_keyboard_t keyboard( _ascii_device ) ;

        natus::concurrent::lock_t lk( _buffer_mtx ) ;
        
        {
            for( auto const& item : _ascii_keyboard_keys )
            {
                keyboard.set_state( item.first, item.second ) ;
            }
            _ascii_keyboard_keys.clear() ;
        }
    }
}

//***
bool_t xlib_module::handle_input_event( XEvent const & event )
{
    switch( event.type )
    {
    case MotionNotify:
        //natus::log::global_t::status("MotionNotify") ;
        {
            XMotionEvent const & ev = event.xmotion ;
            //natus::log::global_t::status( ::std::to_string(ev.x) + " : " + ::std::to_string(ev.y) ) ;
            //natus::log::global_t::status( ::std::to_string(ev.x_root) + " : " + ::std::to_string(ev.y_root) ) ;

            XWindowAttributes wa ;
            XGetWindowAttributes( ev.display, ev.window, &wa ) ;


            // do global
            {
                natus::concurrent::lock_t lk( _buffer_mtx ) ;

                natus::math::vec2f_t const dim(
                     float_t( WidthOfScreen(wa.screen) ),
                     float_t( HeightOfScreen(wa.screen) ) ) ;

                natus::math::vec2f_t const v = natus::math::vec2f_t( 
                     float_t( ev.x_root ), float_t( dim.y() - ev.y_root ) ) / dim ;
                _pointer_coords_global.push_back( v ) ;
            }

            // do local
            {
                natus::concurrent::lock_t lk( _buffer_mtx ) ;

                natus::math::vec2f_t const dim(
                     float_t( wa.width ),
                     float_t( wa.height )) ;

                natus::math::vec2f_t const v = natus::math::vec2f_t( 
                    float_t( ev.x ), float_t( dim.y() - ev.y ) ) / dim ;
                _pointer_coords_local.push_back( v ) ;
            }

        }
        break;

    case ButtonPress:
        natus::log::global_t::status("ButtonPress") ;
        break ;

    case ButtonRelease:
        XClearWindow( event.xany.display, event.xany.window ) ;
        //run = false ;
        natus::log::global_t::status("ButtonRelease") ;
        break ;

    case KeyPress:
        natus::log::global_t::status("KeyPress") ;
        break ;

    case KeyRelease:
        natus::log::global_t::status("KeyRelease") ;
        break ;
    }

    return true ;
}
