

#pragma once

#include "../api.h"
#include "../typedefs.h"
#include "../protos.h"

namespace natus
{
    namespace application
    {
        class NATUS_APPLICATION_API platform_window
        {
        public:

            virtual ~platform_window( void_t ) {}
        };
        natus_soil_typedef( platform_window ) ;
    }
}