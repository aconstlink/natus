//------------------------------------------------------------
// snakeoil (c) Alexis Constantin Link
// Distributed under the MIT license
//------------------------------------------------------------
#include "xlib_window_handle.h"

using namespace so_app ;
using namespace so_app::so_xlib ;

//***********************************************************************
xlib_window_handle::xlib_window_handle( void_t ) 
{

}

//***********************************************************************
xlib_window_handle::xlib_window_handle( Display * display, Window handle ) 
{
    _display = display ;
    _handle = handle ;
}

//***********************************************************************
xlib_window_handle::xlib_window_handle( this_rref_t rhv ) 
{
    _display = rhv._display ;
    rhv._display = NULL ;
    
    _handle = rhv._handle ;
    rhv._handle = Window() ;
}

//***********************************************************************
xlib_window_handle::~xlib_window_handle( void_t ) 
{
}

//***********************************************************************
xlib_window_handle::this_ref_t xlib_window_handle::operator = ( this_rref_t rhv ) 
{
    _display = rhv._display ;
    rhv._display = NULL ;
    
    _handle = rhv._handle ;
    rhv._handle = Window() ;
    
    return *this ;
}

//***********************************************************************
Window xlib_window_handle::set_handle( Window hwnd ) 
{
    Window tmp = _handle ;
    _handle = hwnd ;
    return tmp ;
}

//***********************************************************************
Window xlib_window_handle::get_handle( void_t ) 
{
    return _handle ;
}

//***********************************************************************
Display* xlib_window_handle::get_display( void_t ) 
{
    return _display ;
}

//***********************************************************************
bool_t xlib_window_handle::is_valid( void_t ) 
{
    return _handle != Window() ;
}

