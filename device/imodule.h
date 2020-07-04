#pragma once

#include "api.h"
#include "typedefs.h"

namespace natus
{
    namespace device
    {
        class NATUS_DEVICE_API imodule
        {

        public:

            virtual void_t update( void_t ) = 0 ;
        };
        natus_soil_typedef( imodule ) ;
    }
}