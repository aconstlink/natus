#pragma once

#include "../typedefs.h"
#include "../result.h"
#include "../protos.h"
#include "../log_level.h"

#include <string>

namespace natus
{
    namespace log
    {
        class isystem
        {
        public:
            virtual ~isystem( void_t ) {}
            virtual natus::log::result log( natus::log::log_level ll, ::std::string const& msg ) = 0 ;
            virtual natus::log::result add_logger( natus::log::ilogger_ptr_t lptr ) = 0 ;
            virtual natus::log::ilogger_ptr_t set_default_logger( natus::log::ilogger_ptr_t lptr ) = 0 ;
        };
    }
}

