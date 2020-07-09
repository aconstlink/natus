

#include "global.h"

using namespace natus::device ;

natus::device::system_res_t global::system( void_t ) 
{
    static natus::device::system_res_t sys = natus::device::system_t() ;

    return sys ;
}