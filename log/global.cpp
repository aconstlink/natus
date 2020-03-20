//------------------------------------------------------------
// natus (c) Alexis Constantin Link
// Distributed under the MIT license
//------------------------------------------------------------
#include "global.h"

#include "system/system.h"

using namespace natus::log ;

std::mutex global::_mtx ;
global::this_ptr_t global::_ptr = nullptr ;

//*************************************************************************************
global::global( void_t )
{
    __default_log_system = natus::log::system_t::create() ;
}

//*************************************************************************************
global::global( this_rref_t rhv )
{
    natus_move_member_ptr( __default_log_system, rhv ) ;
}

//*************************************************************************************
global::~global( void_t )
{
    natus::log::system::destroy( __default_log_system ) ;
}

//*************************************************************************************
global::this_ptr_t global::init( void_t )
{
    if( natus::core::is_not_nullptr( global_t::_ptr ) )
        return this_t::_ptr ;

    {
        ::std::lock_guard<std::mutex> lk( this_t::_mtx ) ;

        if( natus::core::is_not_nullptr( global_t::_ptr ) )
            return this_t::_ptr ;

        // again, do not use natus memory
        // memory is above log
        this_t::_ptr = new this_t() ;

        this_t::status( "[online] : natus log" ) ;
    }

    return this_t::_ptr ;
}

//*************************************************************************************
void_t global::deinit( void_t )
{
    if( natus::core::is_nullptr( global_t::_ptr ) )
        return ;

    delete global_t::_ptr ;
    this_t::_ptr = nullptr ;
}

//*************************************************************************************
global::this_ptr_t global::get( void_t )
{
    return this_t::init() ;
}

//*************************************************************************************
void_t global::add_logger( natus::log::ilogger_ptr_t ptr )
{
    this_t::get()->__default_log_system->add_logger( ptr ) ;
}

//*************************************************************************************
natus::log::store_logger_cptr_t global::get_store( void_t )
{
    return this_t::get()->__default_log_system->get_store() ;
}

//*************************************************************************************
void_t global::message( log_level level, string_cref_t msg )
{
    this_t::get()->__default_log_system->log( level, msg ) ;
}

//*************************************************************************************
bool_t global::message( bool_t condition, log_level level, string_cref_t msg )
{
    if( condition ) natus::log::global::message( level, msg ) ;
    return condition ;
}

//*************************************************************************************
void_t global::status( string_cref_t msg )
{
    natus::log::global::message( natus::log::log_level::status, msg ) ;
}

//*************************************************************************************
bool_t global::status( bool_t condition, string_cref_t msg )
{
    return natus::log::global::message( condition, natus::log::log_level::status, msg ) ;
}

//*************************************************************************************
void_t global::warning( string_cref_t msg )
{
    natus::log::global::message( natus::log::log_level::warning, msg ) ;
}

//*************************************************************************************
bool_t global::warning( bool_t condition, string_cref_t msg )
{
    return natus::log::global::message( condition, natus::log::log_level::warning, msg ) ;
}

//*************************************************************************************
void_t global::error( string_cref_t msg )
{
    natus::log::global::message( natus::log::log_level::error, msg ) ;
}

//*************************************************************************************
void_t global::error_and_exit( string_cref_t msg )
{
    natus::log::global::message( natus::log::log_level::error, msg ) ;
    natus::core::break_if( true ) ;
}

//*************************************************************************************
void_t global::error_and_exit( bool_t condition, string_cref_t msg )
{
    natus::core::break_if( natus::log::global::message( condition, natus::log::log_level::error, msg ) ) ;
}

//*************************************************************************************
bool_t global::error( bool_t condition, string_cref_t msg )
{
    return natus::log::global::message( condition, natus::log::log_level::error, msg ) ;
}


//*************************************************************************************
void_t global::critical( string_cref_t msg )
{
    natus::log::global::message( natus::log::log_level::critical, msg ) ;
}

//*************************************************************************************
bool_t global::critical( bool_t condition, string_cref_t msg )
{
    return natus::log::global::message( condition, natus::log::log_level::critical, msg ) ;
}
