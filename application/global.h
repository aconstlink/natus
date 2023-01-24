

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
            natus_this_typedefs( global ) ;

        public:

            static platform_application_res_t create_application( void_t )  noexcept ;
            static platform_application_res_t create_application( natus::application::app_res_t ) noexcept ;
            static int_t create_and_exec_application( natus::application::app_res_t ) noexcept ;
        };
        natus_typedef( global ) ;
    }
}