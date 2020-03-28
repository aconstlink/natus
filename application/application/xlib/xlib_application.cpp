//------------------------------------------------------------
// snakeoil (c) Alexis Constantin Link
// Distributed under the MIT license
//------------------------------------------------------------
#include "xlib_application.h"

#include "../../window/glx/glx_window.h"

#include "../../system.h"
#include "../../log.h"

#include <X11/Xlib.h>
#include <X11/keysym.h>

using namespace so_app ;
using namespace so_app::so_xlib ;

Display * xlib_application::_display = NULL ;
size_t xlib_application::_display_use_count = 0 ;

//***********************************************************************
void_t xlib_application::connect_display( void_t ) 
{
    if( so_app::log::warning( _display != NULL, 
            "[xlib_application::connect_display] : Display already initialized." ) )
    {
        return ;
    }
    
    _display = XOpenDisplay( NULL ) ;

    if( so_app::log::error( _display==NULL, "[xlib_application] : XOpenDisplay" ) ){
        exit(0) ;
    }
    
    _display_use_count = 1 ;
}

//***********************************************************************
void_t xlib_application::disconnect_display( void_t ) 
{
    --_display_use_count ;
    if( _display_use_count == 0 )
    {
        XCloseDisplay( _display ) ;
        _display = NULL ;
    }
}

//***********************************************************************
Display * xlib_application::move_display( void_t ) 
{
    ++_display_use_count ;
    return _display ;
}

//***********************************************************************
xlib_application::xlib_application( void_t ) 
{
    int_t ret = XInitThreads() ;
    if( so_app::log::error( ret == 0, "[xlib_application] : XInitThreads" ) )    {
        exit(ret) ;
    }
    
    connect_display() ;
}

//***********************************************************************
Display * xlib_application::get_display( void_t ) 
{
    if( so_app::log::error( _display==NULL, 
            "[xlib_application::get_display] : No display Connection. An Xlib application object is required." ) )
    {
        exit(1) ;
    }
    
    return _display ;
}

//***********************************************************************
xlib_application::xlib_application( this_rref_t rhv ) : base_t( std::move( rhv ) )
{
    this_t::move_display() ;
}

//***********************************************************************
xlib_application::~xlib_application( void_t )
{
    this_t::disconnect_display() ;
}

//***********************************************************************
xlib_application::this_ptr_t xlib_application::create( void_t ) 
{
    return this_t::create( this_t() ) ;
}

//***********************************************************************
xlib_application::this_ptr_t xlib_application::create( this_rref_t rhv )
{
    return so_app::memory::alloc( std::move( rhv ), "[xlib_application::create]" ) ;
}

//***********************************************************************
void_t xlib_application::destroy( this_ptr_t ptr ) 
{
    so_app::memory::dealloc( ptr ) ;
}

//***********************************************************************
so_app::result xlib_application::destroy( void_t ) 
{
    this_t::destroy( this ) ;
    return so_app::ok ;
}

//***********************************************************************
so_app::result xlib_application::exec_derived( void_t )
{
    Display * display = this_t::get_display() ;
    
    XEvent event ;
    
    bool_t run = true ;
    while( run )
    {
        int_t num_events = 0 ;
        //bool_t pressed = false ;
        
        while( (num_events = XPending(display)) != 0 )
        {
            XNextEvent( display, &event ) ;
            
            {
                Atom a = XInternAtom( event.xany.display, "wnd_ptr_value", false ) ;
                Atom at = XInternAtom( event.xany.display, "wnd_ptr_type", false ) ; 
        
                Atom at_ret ;
                int format_out ;
                unsigned long len_out, bytes_after ;

                uchar_ptr_t stored_data ;
                XGetWindowProperty( event.xany.display, event.xany.window, a, 0, sizeof(void_ptr_t), false,
                        at, &at_ret, &format_out, &len_out, &bytes_after, (uchar_ptr_t*)&stored_data ) ;

                so_xlib::xlib_window_ptr_t wnd_ptr = so_xlib::xlib_window_ptr_t(*(this_ptr_t*)stored_data) ;
                wnd_ptr->xevent_callback( event ) ;
                
            }
            
            switch( event.type )
            {
            case Expose:
                
                break ;

            case ButtonRelease:
                //run = false ;
                break ;
                
            case DestroyNotify:
                break ;
                
            case UnmapNotify:
                break ;
                
            case ClientMessage:
                {
                    run = false ;
                }
                break ;
            }
        }
    }

    return so_app::ok ;
}



