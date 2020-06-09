
#include "xlib_application.h"
#include "xlib_window.h"

#include <natus/log/global.h>

#include <algorithm>
#include <X11/Xlib.h>

using namespace natus::application ;
using namespace natus::application::xlib ;

//*****************************************************************
window::window( void_t ) 
{}

//*****************************************************************
window::window( window_info const & info ) 
{
    this_t::create_window( info ) ;
}

//*****************************************************************
window::window( Display * display, Window wnd ) 
{
    this_t::create_window( display, wnd ) ;
}

//****************************************************************
window::window( this_rref_t rhv ) : platform_window( ::std::move( rhv ) )
{
    _display = rhv._display ;
    rhv._display = nullptr ;

    _handle = rhv._handle ;
    rhv._handle = 0 ;

    this_t::store_this_ptr_in_atom( 
        _display, _handle ) ;
}

//***************************************************************
window::~window( void_t ) 
{
    this_t::destroy_window() ;
}

//***************************************************************
Window window::get_handle( void_t ) 
{
    return _handle ;
}

//***************************************************************
Display * window::get_display( void_t ) 
{
    return _display ;
}

//***************************************************************
void_t window::create_window( window_info const & wi ) 
{
    auto const status = XInitThreads() ;
    natus::log::global_t::warning( status == 0, 
           natus_log_fn("XInitThreads") ) ;

    window_info wil = wi ;

    Display * display = natus::application::xlib::xlib_application::get_display() ;

    Window root = DefaultRootWindow( display ) ;

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
    
    if( natus::log::global_t::error( wnd == BadAlloc, 
            "[window::create_window] : XCreateSimpleWindow - BadAlloc" ) ){
        return ;
    }
    else if( natus::log::global_t::error( wnd == BadValue, 
            "[window::create_window] : XCreateSimpleWindow - BadValue" ) ){
        return ;
    }

    this_t::create_window( display, wnd ) ;
    
    if( wi.show )
    {
        XMapWindow( display, wnd );
        XFlush( display ) ;
    }
}

//***************************************************************
void_t window::create_window( Display * display, Window wnd ) 
{
    XSelectInput( display, wnd, 
                  ExposureMask | KeyPressMask | KeyReleaseMask | 
                  ButtonPressMask | ButtonReleaseMask | 
                  StructureNotifyMask //| ResizeRedirectMask 
                  ) ;

    // prepare per window data
    this_t::store_this_ptr_in_atom( display, wnd ) ;

    /// setup client message for closing a window
    {
        Atom del = XInternAtom( display, "WM_DELETE_WINDOW", false ) ;
        XSetWMProtocols( display, wnd, &del, 1 ) ;
    }

    _handle = wnd ;
    _display = display ;
}

//**************************************************************
void_t window::store_this_ptr_in_atom( Display * display, Window wnd ) 
{
    Atom a = XInternAtom( display, "wnd_ptr_value", false ) ;
    Atom at = XInternAtom( display, "wnd_ptr_type", false ) ; 
    int format_in = 8 ; // 8bit
    int const len_in = sizeof(this_ptr_t) ;

    // store the data in the per window memory
    {
        this_ptr_t data = this ;
        XChangeProperty( display, wnd, a, at, 
          format_in, PropModeReplace, (uchar_ptr_t)&data, len_in ) ;
    }

    // test the stored data
    {
        Atom at_ret ;
        int format_out ;
        unsigned long len_out, bytes_after ;

        uchar_ptr_t stored_data ;
        XGetWindowProperty( display, wnd, a, 0, 
           sizeof(void_ptr_t), false, at, &at_ret, 
           &format_out, &len_out, &bytes_after, 
           (uchar_ptr_t*)&stored_data ) ;

        this_ptr_t test_ptr = this_ptr_t(*(this_ptr_t*)stored_data) ;

        natus_assert( len_out == len_in ) ;
        natus_assert( format_out == format_in ) ;
        natus_assert( test_ptr == this ) ;
    }
}

//*****************************************************************
void_t window::destroy_window( void_t ) 
{
    //if( _display != NULL && _handle != NULL )
        //XDestroyWindow( _display, _handle ) ;
}

//****************************************************************
void_t window::xevent_callback( XEvent const & event ) 
{
    natus::log::global_t::status( "[window::xevent_callback]" ) ;

    switch( event.type )
    {
    case Expose:
        XClearWindow( event.xany.display, event.xany.window ) ;
        natus::log::global_t::status("expose") ;
        break ;

    case ButtonRelease:
        XClearWindow( event.xany.display, event.xany.window ) ;
        //run = false ;
        //natus::log::global_t::status("buttonrelease") ;
        break ;

    case VisibilityNotify:
        //natus::log::global_t::status("visnotify") ;
        break ;

    case ConfigureNotify:{
        XConfigureEvent xce = event.xconfigure ;
        natus::application::resize_message const rm {
                0,0,
                (size_t)xce.width, (size_t)xce.height
            } ;

            this_t::foreach_listener( [&]( natus::application::iwindow_message_listener_res_t lst ){ lst->on_resize( rm ) ; } ) ;
        break ;}

    case ResizeRequest:
        {
            natus::log::global_t::status("window resize") ;
            XWindowAttributes attr ;
            XGetWindowAttributes( event.xany.display, 
                event.xany.window, &attr ) ;

            XResizeRequestEvent const & rse = (XResizeRequestEvent const &) event ;
            natus::application::resize_message const rm {
                attr.x, attr.y, 
                (size_t)rse.width, (size_t)rse.height
            } ;

            this_t::foreach_listener( [&]( natus::application::iwindow_message_listener_res_t lst ){ lst->on_resize( rm ) ; } ) ;
            XClearArea( event.xany.display, event.xany.window, 0,0,0,0, true ) ;
        }
        break ;
    }
}

//***
void_t window::show_window(  window_info const & wi ) 
{
    if( wi.show ) 
    {
        XMapWindow( _display, _handle ) ;
    }
    else 
    {
        XUnmapWindow( _display, _handle ) ;
    }
}

//***************************************************************
void_t send_toggle( natus::application::toggle_window_in_t ) 
{
}
