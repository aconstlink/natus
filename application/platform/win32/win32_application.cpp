#include "win32_application.h"

//#include <natus/device/global.h>
//#include <natus/device/system/idevice_system.h>
//#include <natus/device/api/win32/rawinput/rawinput_api.h>

#include <natus/log/global.h>

#include <windows.h>

using namespace natus::application ;
using namespace natus::application::win32 ;

//***********************************************************************
win32_application::win32_application( void_t ) 
{
    // @todo rawinput
    #if 0
    _rawinput_ptr = natus_device::natus_win32::rawinput_api::create(
        "[win32_application::win32_application] : rawinput_api" ) ;

    natus_device::global::device_system()->register_api( 
        reinterpret_cast<natus_device::iapi_ptr_t>(_rawinput_ptr) ) ;
    #endif
}

//***********************************************************************
win32_application::win32_application( this_rref_t rhv ) : base_t( ::std::move( rhv ) )
{
    //natus_move_member_ptr( _rawinput_ptr, rhv ) ;
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

        // @todo rawinput
        //_rawinput_ptr->handle_input_event( msg.hwnd, msg.message,
          //  msg.wParam, msg.lParam ) ;
    } 

    return result::ok ;
}
