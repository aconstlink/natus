
#include "xaudio2.h"

#include <xaudio2.h>

using namespace natus::audio ;

struct natus::audio::xaudio2_backend::pimpl
{
    pimpl( void_t )
    {
    }

    ~pimpl( void_t )
    {
    }

    bool_t init( void_t ) noexcept
    {
        return true ;
    }

    void_t release( void_t ) 
    {
        
    }

    size_t construct_buffer_object( size_t oid, natus::ntd::string_cref_t name, natus::audio::buffer_object_ref_t )
    {
        return oid ;
    }

    void_t release_buffer_object( size_t const oid ) noexcept
    {
    }

    bool_t update( size_t const oid, natus::audio::buffer_object_ref_t buffer ) noexcept
    {
        return true ;
    }

    void_t execute( size_t const oid, natus::audio::buffer_object_ref_t obj, 
        natus::audio::backend::execute_detail_cref_t det ) noexcept
    {
    }

    void_t update( void_t ) noexcept
    {
        // stuff can be done here per frame
        // track state of playing sources
    }
} ;
//**************************************************************************
xaudio2_backend::xaudio2_backend( void_t ) noexcept  : backend( natus::audio::backend_type::xaudio2 )
{
    this_t::set_bid( this_t::create_backend_id() ) ;
}

//**************************************************************************
xaudio2_backend::xaudio2_backend( this_rref_t rhv ) noexcept : backend( std::move( rhv ) )
{
    natus_move_member_ptr( _pimpl, rhv ) ;
}

//**************************************************************************
xaudio2_backend::~xaudio2_backend( void_t ) noexcept 
{
    this_t::release() ;
}

//**************************************************************************
xaudio2_backend::this_ref_t xaudio2_backend::operator = ( this_rref_t rhv ) noexcept 
{
    natus_move_member_ptr( _pimpl, rhv ) ;
    return *this ;
}

//**************************************************************************
natus::audio::result xaudio2_backend::configure( natus::audio::capture_type const, 
    natus::audio::capture_object_res_t ) noexcept 
{
    return natus::audio::result::ok ;
}

//**************************************************************************
natus::audio::result xaudio2_backend::capture( natus::audio::capture_object_res_t, bool_t const b ) noexcept 
{
    return natus::audio::result::ok ;
}

//**************************************************************************
natus::audio::result xaudio2_backend::release( natus::audio::capture_object_res_t ) noexcept 
{
    return natus::audio::result::ok ;
}

//**************************************************************************
natus::audio::result xaudio2_backend::configure( natus::audio::buffer_object_res_t ) noexcept 
{
    return natus::audio::result::ok ;
}

//**************************************************************************
natus::audio::result xaudio2_backend::update( natus::audio::buffer_object_res_t ) noexcept 
{
    return natus::audio::result::ok ;
}

//**************************************************************************
natus::audio::result xaudio2_backend::execute( natus::audio::buffer_object_res_t, natus::audio::backend::execute_detail_cref_t ) noexcept 
{
    return natus::audio::result::ok ;
}

//**************************************************************************
natus::audio::result xaudio2_backend::release( natus::audio::buffer_object_res_t ) noexcept 
{
    return natus::audio::result::ok ;
}

//**************************************************************************
void_t xaudio2_backend::init( void_t ) noexcept 
{
    if( _pimpl == nullptr )
    {
        _pimpl = natus::memory::global_t::alloc( pimpl(), "[natus::audio::oal_backend::pimpl]" ) ;
        _pimpl->init() ;
    }
}

//**************************************************************************
void_t xaudio2_backend::release( void_t ) noexcept 
{
    if( _pimpl != nullptr )
    {
        _pimpl->release() ;
    }
    natus::memory::global_t::dealloc( _pimpl ) ;
    _pimpl = nullptr ;
}

//**************************************************************************
void_t xaudio2_backend::begin( void_t ) noexcept 
{
    _pimpl->update() ;
}

//**************************************************************************
void_t xaudio2_backend::end( void_t ) noexcept 
{
}

//**************************************************************************
size_t xaudio2_backend::create_backend_id(  void_t ) noexcept 
{
    static size_t const id = size_t( natus::audio::backend_type::xaudio2 ) ;
    return id ;
}

//**************************************************************************