#pragma once

#include "api.h"
#include "typedefs.h"

namespace natus
{
    namespace device
    {
        class NATUS_DEVICE_API system
        {
            natus_this_typedefs( system ) ;

        public:

            system( void_t ) ;
            system( this_cref_t ) = delete ;
            system( this_rref_t ) ;
            ~system( void_t ) ;

        public:

            void_t update( void_t ) ;
        };
        natus_soil_typedef( system ) ;
    }
}