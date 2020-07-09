#include "win32_application.h"

#include <natus/device/global.h>
#include <natus/log/global.h>

#include <windows.h>

using namespace natus::application ;
using namespace natus::application::win32 ;

//***********************************************************************
win32_application::win32_application( void_t ) 
{
    _rawinput = natus::device::win32::rawinput_module_t() ;
    natus::device::global_t::system()->add_module( _rawinput ) ;
}

//***********************************************************************
win32_application::win32_application( this_rref_t rhv ) : base_t( ::std::move( rhv ) )
{
    _rawinput = ::std::move( rhv._rawinput ) ;
}

//***********************************************************************
win32_application::win32_application( natus::application::app_res_t app ) : base_t( app )
{
    _rawinput = natus::device::win32::rawinput_module_t() ;
    natus::device::global_t::system()->add_module( _rawinput ) ;
}

//***********************************************************************
win32_application::~win32_application( void_t )
{
    // _rawinput_module_ptr will be auto deleted
}

//***********************************************************************
win32_application::this_ptr_t win32_application::create( void_t ) 
{
    return this_t::create( this_t() ) ;
}

//***********************************************************************
win32_application::this_ptr_t win32_application::create( this_rref_t rhv )
{
    return natus::application::memory::alloc( ::std::move( rhv ), 
        "[win32_application::create]" ) ;
}

//***********************************************************************
void_t win32_application::destroy( this_ptr_t ptr ) 
{
    natus::application::memory::dealloc( ptr ) ;
}

//***********************************************************************
natus::application::result win32_application::on_exec( void_t )
{
    MSG msg ;
    while( GetMessage( &msg, 0,0,0) )
    {
        TranslateMessage( &msg ) ;
        DispatchMessage( &msg ) ;

        // will be send if a win32_window is closed.
        if( msg.message == WM_USER )
        {
            break ;
        }

        _rawinput->handle_input_event( msg.hwnd, msg.message,
            msg.wParam, msg.lParam ) ;
    } 

    return result::ok ;
}
