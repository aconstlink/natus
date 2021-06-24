
#include "system.h"

using namespace natus::profile ;

//********************************************
void_t system::add_entry( natus::profile::entry_cref_t e ) noexcept 
{
    natus::concurrent::lock_guard_t lk( _mtx ) ;

    _entries.emplace_back( e ) ;
}

//********************************************
void_t system::add_entry( natus::profile::entry_rref_t e ) noexcept 
{
    natus::concurrent::lock_guard_t lk( _mtx ) ;

    _entries.emplace_back( std::move( e ) ) ;
}

//********************************************
void_t system::unique_entry( natus::profile::entry_cref_t e ) noexcept 
{
    {
        natus::concurrent::lock_guard_t lk( _mtx ) ;

        for( auto & e_ : _entries )
        {
            if( e_.get_name() == e.get_name() )
            {
                e_ = e ;
                return ;
            }
        }
    }

    this_t::add_entry( e ) ;
}

//********************************************
void_t system::unique_entry( natus::profile::entry_rref_t e ) noexcept 
{
    {
        natus::concurrent::lock_guard_t lk( _mtx ) ;

        for( auto & e_ : _entries )
        {
            if( e_.get_name() == e.get_name() )
            {
                e_ = e ;
                return ;
            }
        }
    }

    this_t::add_entry( std::move( e ) ) ;
}

//********************************************
system::entries_t system::get_and_reset_entries( void_t ) noexcept 
{
    // here the entries are temporary, 
    // maybe there are unique/persistent entries too
    return std::move( _entries ) ;
}

//********************************************
void_t system::reset( void_t ) noexcept 
{
    natus::concurrent::lock_guard_t lk( _mtx ) ;
    _entries.clear() ;
}