
#pragma once

#include "../api.h"
#include "../typedefs.h"
#include "../result.h"
#include "../protos.h"
#include "../log_level.h"

namespace natus
{
    namespace log
    {
        class NATUS_LOG_API ilogger
        {
        public:

            virtual natus::log::result log( natus::log::log_level ll, ::std::string const& msg ) = 0 ;
        };
    }
}
