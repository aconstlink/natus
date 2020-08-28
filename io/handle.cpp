
#include "handle.h"
#include "system.h"

#include <natus/core/break_if.hpp>
#include <natus/core/macros/move.h>

using namespace natus::io ;

//************************************************************************************
load_handle::load_handle( natus::io::internal_item_ptr_t ptr, natus::io::system_ptr_t ios_ptr )
{
    _data_ptr = ptr ;
    _ios = ios_ptr ;
}

//************************************************************************************
load_handle::load_handle( void_t )
{}

//************************************************************************************
load_handle::load_handle( this_rref_t rhv )
{
    natus_move_member_ptr( _data_ptr, rhv ) ;
    natus_move_member_ptr( _ios, rhv ) ;
}

//************************************************************************************
load_handle::~load_handle( void_t )
{
    natus::core::break_if( natus::core::is_not_nullptr( _data_ptr ) ) ;
}

//************************************************************************************
load_handle::this_ref_t load_handle::operator = ( this_rref_t rhv )
{
    natus_move_member_ptr( _data_ptr, rhv ) ;
    natus_move_member_ptr( _ios, rhv ) ;
    return *this ;
}

//************************************************************************************
natus::io::result load_handle::wait_for_operation( natus::io::load_completion_funk_t funk )
{
    if( natus::core::is_nullptr( _ios ) ) 
        return natus::io::result::invalid_handle ;

    auto * ios = _ios ;
    return ios->wait_for_operation( this_t( ::std::move( *this )), funk ) ;
}

bool_t load_handle::can_wait( void_t ) const noexcept { return _data_ptr != nullptr ; }

//************************************************************************************
store_handle::store_handle( natus::io::internal_item_ptr_t ptr, natus::io::system_ptr_t ios_ptr  )
{
    _data_ptr = ptr ;
    _ios = ios_ptr ;
}

//************************************************************************************
store_handle::store_handle( void_t )
{}

//************************************************************************************
store_handle::store_handle( this_rref_t rhv )
{
    natus_move_member_ptr( _data_ptr, rhv ) ;
    natus_move_member_ptr( _ios, rhv ) ;
}

//************************************************************************************
store_handle::~store_handle( void_t )
{
    natus::core::break_if( natus::core::is_not_nullptr(_data_ptr) ) ;
}

//************************************************************************************
store_handle::this_ref_t store_handle::operator = ( this_rref_t rhv )
{
    natus_move_member_ptr( _data_ptr, rhv ) ;
    natus_move_member_ptr( _ios, rhv ) ;
    return *this ;
}

//************************************************************************************
natus::io::result store_handle::wait_for_operation( void_t )
{
    if( natus::core::is_nullptr( _ios ) )
        return natus::io::result::invalid_handle ;

    auto * ios = _ios ;
    return ios->wait_for_operation( this_t( ::std::move( *this ) ), [=] ( natus::io::result ) { } ) ;
}

//************************************************************************************
natus::io::result store_handle::wait_for_operation( natus::io::store_completion_funk_t funk )
{
    if( natus::core::is_nullptr( _ios ) )
        return natus::io::result::invalid ;

    auto * ios = _ios ;
    return ios->wait_for_operation( this_t( ::std::move( *this ) ), funk ) ;
}

//************************************************************************************