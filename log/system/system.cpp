
#include "system.h"
#include "../logger/std_cerr_logger.h"

#include <algorithm>

using namespace natus::log ;

//*************************************************************************************
system::system( void_t )
{
    _this_default_logger = new std_cerr_logger( ) ;
    _default_logger = _this_default_logger ;
    this_t::add_logger( _default_logger ) ;
    this_t::add_logger( &_slogger ) ;
}

//*************************************************************************************
system::system( this_rref_t rhv )
{
    natus_move_member_ptr( _this_default_logger, rhv ) ;
    natus_move_member_ptr( _default_logger, rhv ) ;
    _loggers = ::std::move( rhv._loggers ) ;
    _slogger = ::std::move( rhv._slogger ) ;
}

//*************************************************************************************
system::~system( void_t )
{
    natus_assert( _default_logger == _this_default_logger ) ;
    delete _this_default_logger ;
}

//*************************************************************************************
system::this_ptr_t system::create( void_t )
{
    /// do not use natus memory
    /// at the moment, natus memory is above log
    return new this_t() ;
}

//*************************************************************************************
void_t system::destroy( this_ptr_t ptr )
{
    delete ptr ;
}

//*************************************************************************************
natus::log::result system::log( natus::log::log_level ll, ::std::string const & msg )
{
    for( auto * logger : _loggers )
    {
        logger->log( ll, msg ) ;
    }
    return natus::log::result::ok ;
}

//*************************************************************************************
natus::log::result system::add_logger( natus::log::ilogger_ptr_t lptr )
{
    if( natus::core::is_nullptr( lptr ) )
    {
        return natus::log::result::invalid_argument ;
    }

    auto iter = ::std::find( _loggers.begin(), _loggers.end(), lptr ) ;
    if( iter != _loggers.end() )
    {
        return natus::log::result::ok ;
    }

    _loggers.push_back( lptr ) ;

    return lptr->log( natus::log::log_level::status, "Logger online" ) ;
}

//*************************************************************************************
natus::log::ilogger_ptr_t system::set_default_logger( natus::log::ilogger_ptr_t lptr )
{
    natus::log::ilogger_ptr_t ret_ptr = _default_logger ;
    _default_logger = lptr == nullptr ? _this_default_logger : lptr ;
    return ret_ptr == _this_default_logger ? nullptr : ret_ptr ;
}

//*************************************************************************************
natus::log::store_logger_cptr_t system::get_store( void_t ) const
{
    return &_slogger ;
}