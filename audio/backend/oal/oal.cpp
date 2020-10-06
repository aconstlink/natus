
#include "oal.h"

using namespace natus::audio ;

struct natus::audio::oal_backend::pimpl
{
};

oal_backend::oal_backend( void_t ) noexcept
{
    _pimpl = natus::memory::global_t::alloc( pimpl(), "[natus::audio::oal_backend::pimpl]" ) ;
}

oal_backend::oal_backend( this_rref_t rhv ) noexcept
{
    natus_move_member_ptr( _pimpl, rhv ) ;
}

oal_backend::~oal_backend( void_t ) noexcept
{
    natus::memory::global_t::dealloc( _pimpl ) ;
}

natus::audio::result oal_backend::configure( natus::audio::capture_object_res_t ) noexcept
{
    // pimpl config

    return natus::audio::result::ok ;
}

natus::audio::result oal_backend::capture( natus::audio::capture_object_res_t, bool_t ) noexcept
{
    // pimpl capture 

    return natus::audio::result::ok ;
}

void_t oal_backend::begin( void_t ) noexcept
{
}

void_t oal_backend::end( void_t ) noexcept
{
}
