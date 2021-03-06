

#include "global.h"

#include <natus/ntd/vector.hpp>
#include <natus/log/global.h>

#include <algorithm>

using namespace natus::system ;

//*****************************************************************
natus::system::os_type global::compiled_os_type( void_t ) 
{
#if defined( NATUS_OS_WINDOWS )
    natus::log::global_t::status( "[global::what_os_type] : have Windows OS" ) ;
    return natus::system::os_type::os_windows ;
#elif defined( NATUS_OS_LINUX )
    natus::log::global_t::status( "[global::what_os_type] : have Linux OS" ) ;
    return natus::system::os_type::os_linux ;
#elif defined( NATUS_OS_ANDROID )
    natus::log::global_t::status( "[global::what_os_type] : have Android OS" ) ;
    return natus::system::os_type::os_android ;
#elif defined( NATUS_OS_MACOS )
    natus::log::global_t::status( "[global::what_os_type] : have Mac OS" ) ;
    return natus::system::os_type::os_macos ;
#endif
    natus::log::global_t::status( "unknown" ) ;
    return natus::system::os_type::os_unknown ;
}


//*****************************************************************
natus::ntd::vector<natus::system::window_type> global::compiled_window_types( void_t ) 
{
    natus::ntd::vector< natus::system::window_type > ret ;

#if defined( NATUS_OS_WINDOWS )
    ret.push_back( natus::system::window_type::win32 ) ;
#elif defined( NATUS_OS_LINUX )
    //ret.push_back( natus::system::window_type::wayland ) ;
#else
    ret.push_back( natus::system::window_type::unknown ) ;
#endif

#if defined( NATUS_GRAPHICS_WGL )
    ret.push_back( natus::system::window_type::wgl ) ;
#endif

#if defined( NATUS_GRAPHICS_GLX )
    ret.push_back( natus::system::window_type::x11 ) ;
    ret.push_back( natus::system::window_type::glx ) ;
#endif

#if defined( NATUS_GRAPHICS_EGL )
    ret.push_back( natus::system::window_type::egl ) ;
#endif

    return ::std::move( ret ) ;
}

//*****************************************************************
bool_t global::has_compiled_window_type( natus::system::window_type const wt ) 
{
    auto const types = this_t::compiled_window_types() ;
    auto const iter = ::std::find( types.begin(), types.end(), wt ) ;
    return iter != types.end() ;
}

//*****************************************************************
natus::ntd::vector< natus::system::graphics_type > global::compiled_graphics_types( void_t ) 
{
    natus::ntd::vector< natus::system::graphics_type > ret ;

#if defined( NATUS_GRAPHICS_OPENGL )
    ret.push_back( natus::system::graphics_type::opengl ) ;
#elif defined( NATUS_GRAPHICS_OPENGLES )
    ret.push_back( natus::system::graphics_type::opengles ) ;
#elif defined( NATUS_GRAPHICS_DIRECT3D )
    ret.push_back( natus::system::graphics_type::directx ) ;
#endif

    return ::std::move( ret ) ;
}

//*****************************************************************
bool_t global::has_compiled_graphics_type( natus::system::graphics_type const gt ) 
{
    auto const types = this_t::compiled_graphics_types() ;
    auto const iter = ::std::find( types.begin(), types.end(), gt ) ;
    return iter != types.end() ;
}
