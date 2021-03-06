

#include "backend.h"

#include <natus/log/global.h>
#include <natus/concurrent/mutex.hpp>

using namespace natus::graphics ;

//****
size_t backend::create_backend_id( void_t ) noexcept
{
    static natus::concurrent::mutex_t mtx ;
    static size_t id = 0 ;

    natus_assert( id <= natus::graphics::max_backends ) ;

    natus::concurrent::lock_guard_t lk( mtx ) ;
    return id++ ;
} 

//****
backend::backend( void_t ) noexcept
{
    //_backend_id = this_t::create_backend_id() ;
}

//****
backend::backend( natus::graphics::backend_type const bt ) : _bt( bt )
{
    //_backend_id = this_t::create_backend_id() ;
}

//****
backend::backend( this_rref_t rhv ) noexcept
{
    //_backend_id = rhv._backend_id ;
    //rhv._backend_id = size_t( -1 ) ;
    _bt = rhv._bt ;
    rhv._bt = backend_type::unknown ;
}

//****
backend::~backend( void_t ) 
{
    natus::log::global_t::status( this_t::get_bid() != size_t( -1 ),
        natus_log_fn( "destruction of backend type [" + natus::graphics::to_string(_bt) + "] with id : " + ::std::to_string( this_t::get_bid() ) ) ) ;
}