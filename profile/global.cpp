

#include "global.h"

using namespace natus::profile ;

natus::profile::system_t natus::profile::global_t::_sys ;


//******************************************************
natus::profile::system_ref_t global::sys( void_t ) noexcept 
{
    return _sys ;
}