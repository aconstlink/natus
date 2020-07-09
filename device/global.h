#pragma once

#include "system.h"

namespace natus
{
    namespace device
    {
        class NATUS_DEVICE_API global
        {
            natus_this_typedefs( global ) ;



        public:

            static natus::device::system_res_t system( void_t ) ;

        };
        natus_typedef( global ) ;
    }
}