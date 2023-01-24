
#include "global.h"


#include <natus/memory/global.h>

using namespace natus::audio ;

global_t::this_ptr_t global_t::_singleton = natus::memory::global_t::alloc( global_t(), "natus::audio::global" ) ;

//***
global::global( void_t )
{
}

//***
global::global( this_rref_t /*rhv*/ ) 
{
}

//***
global::~global( void_t ) 
{
}

//***
global::this_ptr_t global::get( void_t ) 
{
    return _singleton ;
}

//***
void_t global::deinit( void_t ) noexcept 
{
    if( _singleton != nullptr ) 
        natus::memory::global_t::dealloc( _singleton ) ;
}