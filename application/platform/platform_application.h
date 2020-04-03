#pragma once

#include "../api.h"
#include "../protos.h"
#include "../result.h"
#include "../typedefs.h"

namespace natus
{
    namespace application
    {
        class NATUS_APPLICATION_API platform_application
        {
        public:

            virtual natus::application::result destroy( void_t ) = 0 ;
            virtual natus::application::result exec( void_t ) = 0 ;
        };
    }
}