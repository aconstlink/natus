
#include "manager.h"

#include <natus/log/global.h>
#include <iostream>
#include <cstdlib>

using namespace natus::memory ;

//*************************************************************************************
manager::manager( void_t ) 
{

}

//*************************************************************************************
manager::~manager( void_t ) 
{

}

//*************************************************************************************
manager::this_ptr_t manager::create( void_t )
{
    return new this_t() ;
}

//*************************************************************************************
void_t manager::destroy( this_ptr_t ptr )
{
    delete ptr ;
}

//*************************************************************************************
void_ptr_t manager::alloc( size_t const sib, natus::memory::purpose_cref_t purpose ) 
{
    if( sib == 0 ) return nullptr ;

    void_ptr_t ptr = malloc( sib ) ;
    {
        lock_t lk(_mtx) ;
        _ptr_to_info[ptr] = memory_info{sib, purpose} ;
        _allocated_sib += sib ;
    }
    return ptr ;
}

//*************************************************************************************
void_ptr_t manager::alloc( size_t const sib ) 
{
    return this_t::alloc( sib, "[manager::alloc]" ) ;
}

//*************************************************************************************
void manager::dealloc( void_ptr_t ptr ) 
{
    if( ptr == nullptr ) return ;

    {
        lock_t lk(_mtx) ;
        auto iter = _ptr_to_info.find( ptr ) ;

        if( natus::log::global::error( iter==_ptr_to_info.end(),
            "[manager::dealloc] : ptr location not found") ) 
            return ;        

        _allocated_sib -= iter->second.sib ;
        _ptr_to_info.erase( iter ) ;
    }
    ::free( ptr ) ;
}

//*************************************************************************************
size_t manager::get_sib( void_t ) const
{
    return _allocated_sib ;
}

//*************************************************************************************
bool_t manager::get_purpose( void_ptr_t ptr, natus::memory::purpose_ref_t pout ) const 
{
    if( natus::core::is_nullptr(ptr) ) 
        return false ;

    {
        lock_t lk( _mtx );
        auto const iter = _ptr_to_info.find( ptr );

        if( natus::log::global::error( iter == _ptr_to_info.end(),
            "[manager::get_purpose] : ptr location not found" ) )
            return false ;

        pout = iter->second.purpose ;        
    }

    return true ;
}

//*************************************************************************************
void_t manager::dump_to_std( void_t ) const 
{
    lock_t lk(_mtx) ;

    ::std::cout << "***************************************************" << ::std::endl ;
    ::std::cout << "[manager::dump_to_std] : Dump to std [" << _allocated_sib << "] sib" << ::std::endl ;

    for( auto iter : _ptr_to_info )
    {
        ::std::cout << iter.second.purpose << "; sib: " << ::std::to_string(iter.second.sib) << ::std::endl ;
    }

    if( _ptr_to_info.empty() )
    {
        ::std::cout << "* Memory manager has no entries." << ::std::endl ;
    }
}

//*************************************************************************************
void_t manager::destroy( void_t )
{
    this_t::destroy( this ) ;
}