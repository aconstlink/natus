
#include "system.h"
#include <natus/log/global.h>

using namespace natus::application ;

natus::application::system_ptr_t system::_system_ptr = nullptr ;

//***********************************************************************
natus::application::result system::init( void_t )
{
    if( natus::log::global_t::warning( _system_ptr != nullptr, 
        "[natus::application::system::init] : system singleton already initialized." ) ) 
        return natus::application::result::ok ;

    _system_ptr = natus::application::memory::alloc( system(), 
        "[natus::application::system::init] : system" ) ;

    return natus::application::result::ok ;
}

//***********************************************************************
natus::application::result system::deinit( void_t ) 
{
    natus::application::memory::dealloc( _system_ptr ) ;
    _system_ptr = nullptr ;

    return natus::application::result::ok ;
}

//***********************************************************************

