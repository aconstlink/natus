//------------------------------------------------------------
// snakeoil (c) Alexis Constantin Link
// Distributed under the MIT license
//------------------------------------------------------------
#include "xlib_window.h"
#include "../../log.h"

#include "../../application/xlib/xlib_application.h"

#include "../iwindow_message_listener.h"

#include <algorithm>
#include <X11/Xlib.h>

using namespace so_app ;
using namespace so_app::so_xlib ;

//***********************************************************************
xlib_window::xlib_window( window_info const & info ) 
{
    this_t::create_window( info ) ;
}

//***********************************************************************
xlib_window::xlib_window( Display * display, Window wnd ) 
{
    this_t::create_window( display, wnd ) ;
}

//***********************************************************************
xlib_window::xlib_window( this_rref_t rhv ) 
{
    _handle = std::move( rhv._handle ) ;
    
    _msg_listeners = std::move( rhv._msg_listeners ) ;
    _name = std::move( rhv._name ) ;
    
    this_t::store_this_ptr_in_atom( 
        _handle.get_display(), _handle.get_handle() ) ;
}

//***********************************************************************
xlib_window::~xlib_window( void_t ) 
{
    this_t::destroy_window() ;
}

//***********************************************************************
xlib_window::this_ptr_t xlib_window::create( this_rref_t rhv ) 
{
    return this_t::create( std::move(rhv), std::string("[xlib_window::create]") ) ;
}

//***********************************************************************
xlib_window::this_ptr_t xlib_window::create( this_rref_t rhv, std::string const & msg ) 
{
    return so_app::memory::alloc( std::move(rhv), msg ) ;
}

//***********************************************************************
void_t xlib_window::destroy( this_ptr_t ptr ) 
{
    so_app::memory::dealloc( ptr ) ;
}

//***********************************************************************
so_app::result xlib_window::subscribe( iwindow_message_listener_ptr_t lptr ) 
{
    auto iter = std::find( _msg_listeners.begin(), _msg_listeners.end(), lptr ) ;
    if( iter != _msg_listeners.end() ) return so_app::ok ;

    _msg_listeners.push_back( lptr ) ;

    // every newly registered listener needs the current state.
    // so, send all possible events to the listener.
    {
        {
            XWindowAttributes attr ;
            XGetWindowAttributes( _handle.get_display(), _handle.get_handle(), &attr ) ;

            so_app::resize_message rm {
                attr.x, attr.y, 
                attr.width, attr.height
            } ;
            this_t::send_resize( rm ) ;
        }
        
        this_t::send_screen_dpi( &_handle ) ;
        this_t::send_screen_size( &_handle ) ;
    }

    return so_app::ok ;
}

//***********************************************************************
so_app::result xlib_window::unsubscribe( iwindow_message_listener_ptr_t lptr ) 
{
    auto iter = std::find( _msg_listeners.begin(), _msg_listeners.end(), lptr ) ;
    if( iter == _msg_listeners.end() ) return so_app::invalid_argument ;

    _msg_listeners.erase( iter ) ;

    return so_app::ok ;
}

//***********************************************************************
so_app::result xlib_window::destroy( void_t ) 
{
    so_app::memory::dealloc( this ) ;
    return so_app::ok ;
}

//***********************************************************************
iwindow_handle_ptr_t xlib_window::get_handle( void_t ) 
{
    return &_handle ;
}

//***********************************************************************
std::string const & xlib_window::get_name( void_t ) 
{
    return _name ;
}

//***********************************************************************
void_t xlib_window::create_window( window_info const & wi ) 
{
    window_info wil = wi ;

    Display * display = so_xlib::xlib_application::get_display() ;

    Window root = DefaultRootWindow( display ) ;
       
    _name = wil.window_name ;

    int start_x = wi.x ; 
    int start_y = wi.y ; 
    int width = wi.w ; 
    int height = wi.h ; 

    //ShowCursor( wil.show_cursor ) ;

    if( wil.fullscreen )
    {
        
    }
    else
    {
        
    }

    Window wnd = XCreateSimpleWindow( 
            display, root, 
            start_x, start_y, width, height, 1, 
            XBlackPixel(display,0), 
            XWhitePixel(display,0) ) ;
    
    if( so_app::log::error( wnd == BadAlloc, 
            "[xlib_window::create_window] : XCreateSimpleWindow - BadAlloc" ) ){
        return ;
    }
    else if( so_app::log::error( wnd == BadValue, 
            "[xlib_window::create_window] : XCreateSimpleWindow - BadValue" ) ){
        return ;
    }

    this_t::create_window( display, wnd ) ;
    
    if( wi.show )
    {
        XMapWindow( display, wnd );
        XFlush( display ) ;
    }
}

//***********************************************************************
void_t xlib_window::create_window( Display * display, Window wnd ) 
{
    XSelectInput( display, wnd, ExposureMask | KeyPressMask | KeyReleaseMask |
            ButtonPressMask | ButtonReleaseMask | StructureNotifyMask | ResizeRedirectMask ) ;
    
    // prepare per window data
    this_t::store_this_ptr_in_atom( display, wnd ) ;
    
    /// setup client message for closing a window
    {
        Atom del = XInternAtom( display, "WM_DELETE_WINDOW", false ) ;
        XSetWMProtocols( display, wnd, &del, 1 ) ;
    }
        
    _handle = xlib_window_handle(display, wnd) ;
}

//***********************************************************************
void_t xlib_window::store_this_ptr_in_atom( Display * display, Window wnd ) 
{
    Atom a = XInternAtom( display, "wnd_ptr_value", false ) ;
    Atom at = XInternAtom( display, "wnd_ptr_type", false ) ; 
    int format_in = 8 ; // 8bit
    int const len_in = sizeof(this_ptr_t) ;

    // store the data in the per window memory
    {
        this_ptr_t data = this ;        
        XChangeProperty( display, wnd, a, at, format_in, PropModeReplace, 
                (uchar_ptr_t)&data, len_in ) ;
    }

    // test the stored data
    {
        Atom at_ret ;
        int format_out ;
        unsigned long len_out, bytes_after ;

        uchar_ptr_t stored_data ;
        XGetWindowProperty( display, wnd, a, 0, sizeof(void_ptr_t), false,
                at, &at_ret, &format_out, &len_out, &bytes_after, (uchar_ptr_t*)&stored_data ) ;

        this_ptr_t test_ptr = this_ptr_t(*(this_ptr_t*)stored_data) ;

        so_assert( len_out == len_in ) ;
        so_assert( format_out == format_in ) ;
        so_assert( test_ptr == this ) ;
    }
}

//***********************************************************************
void_t xlib_window::destroy_window( void_t ) 
{
    if( !_handle.is_valid() ) return ;

    XDestroyWindow( _handle.get_display(), _handle.get_handle() ) ;
}

//***********************************************************************
void_t xlib_window::xevent_callback( XEvent const & event ) 
{
    //so_app::log::status( "[xlib_window::xevent_callback]" ) ;
    
    switch( event.type )
    {
    case Expose:
        XClearWindow( event.xany.display, event.xany.window ) ;
        break ;

    case ButtonRelease:
        //run = false ;
        break ;
        
    case VisibilityNotify:
        
        break ;

    case ResizeRequest:
        {
            XWindowAttributes attr ;
            XGetWindowAttributes( _handle.get_display(), _handle.get_handle(), &attr ) ;

            XResizeRequestEvent const & rse = (XResizeRequestEvent const &) event ;
            so_app::resize_message const rm {
                attr.x, attr.y, 
                rse.width, rse.height
            } ;

            this_t::send_resize( rm ) ;
        }
        
        break ;
    }    
}

//***********************************************************************
void_t xlib_window::send_resize( so_app::resize_message const & rm ) 
{
    for( auto * lptr : _msg_listeners )
        lptr->on_resize( rm ) ;
}

//***********************************************************************
void_t xlib_window::send_screen_dpi( xlib_window_handle_ptr_t hwnd ) 
{
    // dots per inch
    // try compute with
    // DisplayHeightMM and DisplayHeight
    // DisplayWidthMM and DisplayWidth
    
    {
        uint_t dpix = 96 ;
        uint_t dpiy = 96 ;
        send_screen_dpi( hwnd, dpix, dpiy ) ;
    }
    
}

//***********************************************************************
void_t xlib_window::send_screen_dpi( xlib_window_handle_ptr_t, uint_t dpix, uint_t dpiy ) 
{
    so_app::screen_dpi_message const dpim { dpix, dpiy } ;

    for( auto * lptr : _msg_listeners )
        lptr->on_screen( dpim ) ;
}

//***********************************************************************
void_t xlib_window::send_screen_size( xlib_window_handle_ptr_t wnd_ptr ) 
{
    Display * dsp = wnd_ptr->get_display() ;
    Screen * def_screen = XDefaultScreenOfDisplay(dsp) ;
    
    uint_t const width = WidthOfScreen( def_screen ) ;
    uint_t const height = HeightOfScreen( def_screen ) ;
    
    send_screen_size( wnd_ptr, width, height ) ;
}

//***********************************************************************
void_t xlib_window::send_screen_size( xlib_window_handle_ptr_t, uint_t width, uint_t height ) 
{
    so_app::screen_size_message msg {
        width,height
    } ;

    for( auto * lptr : _msg_listeners )
        lptr->on_screen( msg ) ;
}

