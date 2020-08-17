
#include "global.h"
#include "system.h"

#include <natus/log/global.h>

using namespace natus::io ;

natus::concurrent::mutex_t global::_mtx ;
global * global::_ptr = nullptr ;

//***********************************************************************
global::global( void_t )
{
    _ios = natus::io::system_t::create( natus::io::system_t(),
        "[global::global] : job_scheduler" ) ;
}

//***********************************************************************
global::global( this_rref_t rhv )
{
    natus_move_member_ptr( _ios, rhv ) ;
}

//***********************************************************************
global::~global( void_t )
{
    natus::io::system_t::destroy( _ios ) ;
}

//***********************************************************************
global::this_ptr_t global::create( this_rref_t rhv, natus::memory::purpose_cref_t p )
{
    return natus::concurrent::memory::alloc( ::std::move( rhv ), p ) ;
}

//***********************************************************************
void_t global::destroy( this_ptr_t ptr )
{
    natus::concurrent::memory::dealloc( ptr ) ;
}

//***********************************************************************
global::this_ptr_t global::init( void_t )
{
    if( natus::core::is_not_nullptr( _ptr ) )
        return this_t::_ptr ;

    {
        natus::concurrent::lock_guard_t lk( this_t::_mtx ) ;

        if( natus::core::is_not_nullptr( _ptr ) )
            return this_t::_ptr ;

        this_t::_ptr = this_t::create( this_t(),
            "[natus::io::global::init]" ) ;

        natus::log::global::status( "[online] : natus io" ) ;
    }

    return this_t::_ptr ;
}

//***********************************************************************
void_t global::deinit( void_t )
{
    if( natus::core::is_nullptr( _ptr ) )
        return ;

    this_t::destroy( _ptr ) ;
    _ptr = nullptr ;
}

//***********************************************************************
global::this_ptr_t global::get( void_t )
{
    return this_t::init() ;
}

//***********************************************************************
natus::io::system_ptr_t global::io_system( void_t )
{
    return this_t::get()->_ios ;
}

//***********************************************************************
natus::io::load_handle_t global::load( natus::io::path_cref_t file_path )
{
    return this_t::io_system()->load( file_path ) ;
}

//***********************************************************************
natus::io::load_handle_t global::load( natus::io::path_cref_t file_path,
    size_t const offset, size_t const sib ) 
{
    return this_t::io_system()->load( file_path, offset, sib ) ;
}

//***********************************************************************
natus::io::store_handle_t global::store( natus::io::path_cref_t file_path, char_cptr_t data, size_t sib )
{
    return this_t::io_system()->store( file_path, data, sib ) ;
}