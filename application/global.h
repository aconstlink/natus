

#pragma once

#include "api.h"
#include "protos.h"
#include "typedefs.h"

#include "platform/platform_application.h"

namespace natus
{
    namespace application
    {
        class NATUS_APPLICATION_API global
        {
        public:

            static platform_application_res_t create_application( void_t ) ;
            static platform_application_res_t create_application( natus::application::app_res_t ) ;
        };
        natus_typedef( global ) ;
    }
}