
#include "std_cerr_logger.h"

#include <iostream>

using namespace natus::log ;

//*************************************************************************************
std_cerr_logger::std_cerr_logger( void_t ) {}

//*************************************************************************************
std_cerr_logger::~std_cerr_logger( void_t ) {}

//*************************************************************************************
natus::log::result std_cerr_logger::log( natus::log::log_level ll, ::std::string const & msg ) 
{
    ::std::lock_guard< ::std::mutex > lk(_mtx) ;
    ::std::cerr << natus::log::to_string_short(ll) << ((ll == natus::log::log_level::raw) ? "" : " ") << msg << ::std::endl ;

    return natus::log::result::ok ;
}

