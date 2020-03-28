
#include "win32_window.h"
#include "../iwindow_message_listener.h"

#include <natus/log/global.h>

#include <algorithm>
#include <windows.h>

using namespace natus::application ;
using namespace natus::application::win32 ;

//***********************************************************************
win32_window::win32_window( window_info const & info ) 
{
    this_t::create_window( info ) ;
}

//***********************************************************************
win32_window::win32_window( this_rref_t rhv ) 
{
    _handle = ::std::move( rhv._handle ) ;
    if( _handle.is_valid() )
    {
        SetWindowLongPtr( _handle.get_handle(), GWLP_USERDATA, (LONG_PTR)this ) ;
    }

    _msg_listeners = ::std::move( rhv._msg_listeners ) ;
    _name = ::std::move( rhv._name ) ;
    _is_fullscreen = rhv._is_fullscreen;
    _is_cursor = rhv._is_cursor ;
}

//***********************************************************************
win32_window::win32_window( win32_window_handle_rref_t handle )
{
    _handle = ::std::move( handle ) ;
    _name = "[Clone - snakeoil win32 window]" ;
}

//***********************************************************************
win32_window::~win32_window( void_t ) 
{
    this_t::destroy_window() ;
}

//***********************************************************************
win32_window::this_ptr_t win32_window::create( this_rref_t rhv ) 
{
    return this_t::create( ::std::move(rhv), ::std::string("[win32_window::create]") ) ;
}

//***********************************************************************
win32_window::this_ptr_t win32_window::create( this_rref_t rhv, natus::memory::purpose_cref_t msg )
{
    return natus::application::memory::alloc( ::std::move(rhv), msg ) ;
}

//***********************************************************************
void_t win32_window::destroy( this_ptr_t ptr ) 
{
    natus::application::memory::dealloc( ptr ) ;
}

//***********************************************************************
natus::application::result win32_window::subscribe( iwindow_message_listener_ptr_t lptr ) 
{
    auto iter = ::std::find( _msg_listeners.begin(), _msg_listeners.end(), lptr ) ;
    if( iter != _msg_listeners.end() ) return natus::application::result::ok ;

    _msg_listeners.push_back( lptr ) ;

    // every newly registered listener needs the current state.
    // so, send all possible events to the listener.
    {
        this_t::send_resize( _handle.get_handle() ) ;
        this_t::send_screen_dpi( _handle.get_handle() ) ;
        this_t::send_screen_size( _handle.get_handle() ) ;
    }

    return natus::application::result::ok ;
}

//***********************************************************************
natus::application::result win32_window::unsubscribe( iwindow_message_listener_ptr_t lptr ) 
{
    auto iter = ::std::find( _msg_listeners.begin(), _msg_listeners.end(), lptr ) ;
    if( iter == _msg_listeners.end() ) return natus::application::result::invalid_argument ;

    _msg_listeners.erase( iter ) ;

    return natus::application::result::ok ;
}

//***********************************************************************
natus::application::result win32_window::destroy( void_t ) 
{
    natus::application::memory::dealloc( this ) ;
    return natus::application::result::ok ;
}

//***********************************************************************
iwindow_handle_ptr_t win32_window::get_handle( void_t ) 
{
    return &_handle ;
}

//***********************************************************************
::std::string const & win32_window::get_name( void_t ) const
{
    return _name ;
}

//***********************************************************************
void_t win32_window::send_close( void_t )
{
    PostMessage( _handle.get_handle(), WM_CLOSE, 0, 0 ) ;
}

//***********************************************************************
void_t win32_window::send_toggle( natus::application::toggle_window_in_t di ) 
{
    if( di.toggle_fullscreen )
    {
        _is_fullscreen = !_is_fullscreen ;

        {
            DWORD ws_ex_style = WS_EX_APPWINDOW /*& ~(WS_EX_DLGMODALFRAME |
                          WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE )*/ ;
            SetWindowLongPtrA( _handle.get_handle(), GWL_EXSTYLE, ws_ex_style ) ;
        }

        {
            DWORD ws_style = 0 ;

            if( _is_fullscreen )
            {
                ws_style = WS_POPUP | SW_SHOWNORMAL ;
            }
            else
            {
                ws_style = WS_OVERLAPPEDWINDOW ;
            }

            SetWindowLongPtrA( _handle.get_handle(), GWL_STYLE, ws_style ) ;
        }

        {
            int_t start_x = 0, start_y = 0 ;
            int_t width = GetSystemMetrics( SM_CXSCREEN ) ;
            int_t height = GetSystemMetrics( SM_CYSCREEN ) ;

            if( natus::core::is_not(_is_fullscreen) )
                height += GetSystemMetrics( SM_CYCAPTION ) ;

            if( natus::core::is_not( _is_fullscreen ) )
            {
                width /= 2 ;
                height /= 2 ;
            }

            SetWindowPos( _handle.get_handle(), HWND_TOP, start_x,
                start_y, width, height, SWP_SHOWWINDOW ) ;
        }
    }

    /// @todo make it work.
    if( di.toggle_show_cursor )
    {
        _is_cursor = !_is_cursor ;
        if( natus::core::is_not( _is_cursor ) )
        {
            _cursor = GetCursor() ;
            SetCursor( NULL ) ;
            while( ShowCursor( FALSE ) > 0 ) ;
        }
        else
        {
            SetCursor( _cursor ) ;
            _cursor = NULL ;
            ShowCursor( TRUE ) ;
        }
        
    }
}

//***********************************************************************
HWND win32_window::create_window( window_info const & wi ) 
{
    window_info wil = wi ;

    HINSTANCE hinst = GetModuleHandle(0) ;
    
    static size_t window_number = 0 ;
    natus::std::string_t class_name = wi.window_name + " " + ::std::to_string(window_number++) ;

    HWND hwnd ;
    WNDCLASSA wndclass ;

    wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC ;
    wndclass.lpfnWndProc = this_t::StaticWndProc ;
    wndclass.cbClsExtra = 0 ;
    wndclass.cbWndExtra = 0 ;
    wndclass.hInstance = hinst ;
    wndclass.hIcon = LoadIcon(0, IDI_APPLICATION ) ;
    wndclass.hCursor = LoadCursor( 0, IDC_ARROW ) ;
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH) ;
    wndclass.lpszMenuName = 0 ;
    wndclass.lpszClassName = class_name.c_str() ;
        
    if( natus::log::global::error( !RegisterClassA( &wndclass ), 
        "[win32_window::create_window] : RegisterClassA" ) )
        exit(0) ;
    
    _name = wil.window_name ;

    DWORD ws_style ;
    DWORD ws_ex_style ;

    int start_x = wi.x ; //GetSystemMetrics(SM_CXSCREEN) >> 2 ;
    int start_y = wi.y ; // GetSystemMetrics(SM_CYSCREEN) >> 2 ;
    int width = wi.w ; // GetSystemMetrics(SM_CXSCREEN) >> 1 ;
    int height = wi.h ; // GetSystemMetrics(SM_CYSCREEN) >> 1 ;

    {
        ShowCursor( wil.show_cursor ) ;
        _is_cursor = wil.show_cursor ;
    }

    if( wil.fullscreen )
    {
        ws_ex_style = WS_EX_APPWINDOW /*& ~(WS_EX_DLGMODALFRAME |
                      WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE )*/ ;
        ws_style = WS_POPUP | SW_SHOWNORMAL;
        start_x = start_y = 0 ;        
        width = GetSystemMetrics(SM_CXSCREEN) ;
        height = GetSystemMetrics(SM_CYSCREEN) ;
        _is_fullscreen = true ;
    }
    else
    {
        ws_ex_style = WS_EX_APPWINDOW ;// | WS_EX_WINDOWEDGE ;

        if( wil.borderless )
        {
            ws_style = WS_POPUP | SW_SHOWNORMAL;
        }
        else
        {
            ws_style = WS_OVERLAPPEDWINDOW ; // | WS_CLIPSIBLINGS | WS_CLIPCHILDREN ;
            height += GetSystemMetrics( SM_CYCAPTION ) ;
        }

        
    }

    hwnd = CreateWindowEx( ws_ex_style,
        class_name.c_str(), wil.window_name.c_str(),
        ws_style, // | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        start_x,
        start_y,
        width,
        height,
        0,
        0,
        hinst,
        0 ) ;

    if( natus::log::global::error( hwnd == NULL, 
        "[win32_window::create_window] : CreateWindowA failed" ) )
        exit(0) ;

    if( wi.show ) ShowWindow( hwnd, SW_SHOW ) ;

    // Important action here. The user data is used pass the object
    // that will perform the callback in the static wndproc
    SetWindowLongPtr( hwnd, GWLP_USERDATA, (LONG_PTR)this ) ;

    _handle = win32_window_handle(hwnd) ;

    return hwnd ;
}

//***********************************************************************
void_t win32_window::destroy_window( void_t ) 
{
    if( !_handle.is_valid() ) return ;

    SetWindowLongPtr( _handle.get_handle(), GWLP_USERDATA, (LONG_PTR)nullptr ) ;
    DestroyWindow( _handle.set_handle(NULL) ) ;
}

//***********************************************************************
LRESULT CALLBACK win32_window::StaticWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    this_ptr_t wnd_ptr = (this_ptr_t)GetWindowLongPtr( hwnd, GWLP_USERDATA ) ;
    return wnd_ptr ? wnd_ptr->WndProc(hwnd, msg, wParam, lParam) : DefWindowProc(hwnd, msg, wParam, lParam) ;
}

//***********************************************************************
LRESULT CALLBACK win32_window::WndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
    case WM_SYSCOMMAND: break ;

    case WM_SHOWWINDOW:
        {
            natus::application::show_message const amsg { bool_t(wParam == TRUE) } ;
            for( auto * lptr : _msg_listeners )
            {                
                lptr->on_visible( amsg ) ;
            }
        }

        // pass-through
        // break ;
    case WM_SIZE:
        this_t::send_resize( hwnd ) ;
        break ;
        
    case WM_DPICHANGED:
        this_t::send_screen_dpi( hwnd, 
            uint_t(LOWORD(wParam)), uint_t(HIWORD(wParam)) ) ;
        break ;

    case WM_DISPLAYCHANGE:
        this_t::send_screen_size( hwnd, uint_t(LOWORD(lParam)), uint_t(HIWORD(lParam)) ) ;
        break ;
    
    case WM_ACTIVATE:
    {
        int bp=0; 
        (void)bp ;
    }
        break ;
    case WM_SETFOCUS:
    {
        int bp=0; 
        (void)bp ;
    }
        break ;

    case WM_KILLFOCUS:
    {
        int bp=0; 
        (void)bp ;
    }
        break ;

    case WM_CLOSE: 
    case WM_DESTROY: 
        {
            natus::application::close_message const amsg { true } ;
            for( auto * lptr : _msg_listeners )
            {
                lptr->on_close(amsg) ;
            }
        }
        // post a WM_USER message to the stream so the 
        // win32_application knows when a window is closed.
        PostMessage( hwnd, WM_USER, wParam, lParam ) ;
        return 0 ;
    }

    return DefWindowProc( hwnd, msg, wParam, lParam ) ;
}

//***********************************************************************
void_t win32_window::send_resize( HWND hwnd ) 
{
    RECT rect ;
    GetClientRect( hwnd, &rect ) ;

    natus::application::resize_message rm {
        size_t(rect.left), size_t(rect.top), 
        size_t(rect.right-rect.left), size_t(rect.bottom-rect.top)
    } ;

    for( auto * lptr : _msg_listeners )
        lptr->on_resize( rm ) ;
}

//***********************************************************************
void_t win32_window::get_monitor_info( HWND hwnd, MONITORINFO & imon_out ) 
{
    RECT rect ;
    GetClientRect( hwnd, &rect ) ;

    HMONITOR hmon;

    hmon = MonitorFromRect( &rect, MONITOR_DEFAULTTONEAREST ) ;
    imon_out.cbSize = sizeof(imon_out) ;
    GetMonitorInfo( hmon, &imon_out ) ;
}

//***********************************************************************
void_t win32_window::send_screen_dpi( HWND hwnd ) 
{
    HDC hdc = GetDC( hwnd ) ;
    if( hdc )
    {
        uint_t dpix = GetDeviceCaps( hdc, LOGPIXELSX ) ;
        uint_t dpiy = GetDeviceCaps( hdc, LOGPIXELSY ) ;
        send_screen_dpi( hwnd, dpix, dpiy ) ;
    }
    
}

//***********************************************************************
void_t win32_window::send_screen_dpi( HWND, uint_t dpix, uint_t dpiy ) 
{
    natus::application::screen_dpi_message const dpim { dpix, dpiy } ;

    for( auto * lptr : _msg_listeners )
        lptr->on_screen( dpim ) ;
}

//***********************************************************************
void_t win32_window::send_screen_size( HWND hwnd ) 
{
    uint_t const width = GetSystemMetrics( SM_CXSCREEN ) ;
    uint_t const height = GetSystemMetrics( SM_CYSCREEN ) ;
    send_screen_size( hwnd, width, height ) ;
}

//***********************************************************************
void_t win32_window::send_screen_size( HWND, uint_t width, uint_t height ) 
{
    natus::application::screen_size_message msg {
        width,height
    } ;

    for( auto * lptr : _msg_listeners )
        lptr->on_screen( msg ) ;
}

