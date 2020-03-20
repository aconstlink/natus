
#pragma once

#include "logger.h"

#include <mutex>

namespace natus
{
    namespace log
    {
        class std_cerr_logger : public natus::log::ilogger
        {
            ::std::mutex _mtx ;

        public:

            std_cerr_logger( void_t ) ;
            virtual ~std_cerr_logger( void_t ) ;

        public:

            virtual natus::log::result log( natus::log::log_level ll, ::std::string const& msg ) ;
        };
    }
}