
#pragma once

#include "system.h"

namespace natus
{
    namespace profile
    {
        class NATUS_PROFILE_API global
        {
            natus_this_typedefs( global ) ;

        private:

            static natus::profile::system_t _sys ;

        public:

           static natus::profile::system_ref_t sys( void_t ) noexcept ;

        };
        natus_typedef( global ) ;
    }
}