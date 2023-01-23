

#include "global.h"

using namespace natus::device ;

natus::device::system_ptr_t global::_sys = nullptr ;

natus::device::system_ptr_t global::system( void_t ) 
{
    if( _sys == nullptr ) 
        _sys = natus::memory::global_t::alloc< natus::device::system_t >( "Global Device System" )  ;

    return _sys ;
}

void_t global::deinit( void_t ) noexcept 
{
    natus::memory::global_t::dealloc( _sys ) ;
}