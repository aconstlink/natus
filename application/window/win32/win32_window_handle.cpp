
#include "win32_window_handle.h"

using namespace natus::application ;
using namespace natus::application::win32 ;

//***********************************************************************
win32_window_handle::win32_window_handle( void_t ) 
{

}

//***********************************************************************
win32_window_handle::win32_window_handle( HWND handle ) 
{
    _handle = handle ;
}

//***********************************************************************
win32_window_handle::win32_window_handle( this_rref_t rhv ) 
{
    _handle = rhv._handle ;
    rhv._handle = NULL ;
}

//***********************************************************************
win32_window_handle::~win32_window_handle( void_t ) 
{
    natus_assert( _handle == NULL ) ;
}

//***********************************************************************
win32_window_handle::this_ref_t win32_window_handle::operator = ( this_rref_t rhv ) 
{
    _handle = rhv._handle ;
    rhv._handle = NULL ;
    return *this ;
}

//***********************************************************************
HWND win32_window_handle::set_handle( HWND hwnd ) 
{
    HWND tmp = _handle ;
    _handle = hwnd ;
    return tmp ;
}

//***********************************************************************
HWND win32_window_handle::get_handle( void_t ) 
{
    return _handle ;
}

//***********************************************************************
bool_t win32_window_handle::is_valid( void_t ) 
{
    return _handle != NULL ;
}

