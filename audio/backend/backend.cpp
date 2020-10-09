
#include "backend.h"
#include <natus/concurrent/mutex.hpp>
#include <natus/core/assert.h>

using namespace natus::audio ;


backend::backend( natus::audio::backend_type const bt ) : _bt( bt ) {}
backend::backend( this_rref_t rhv ) 
{
    _backend_id = rhv._backend_id ;
    rhv._backend_id = 0 ;

    _bt = rhv._bt ;
    rhv._bt = backend_type::unknown ;
}
backend::~backend( void_t ) {}
