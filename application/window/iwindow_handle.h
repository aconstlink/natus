#pragma once

#include "../protos.h"
#include "../result.h"
#include "../typedefs.h"
#include "../api.h"

namespace natus
{
    namespace application
    {
        class NATUS_APPLICATION_API iwindow_handle
        {
        public:

            virtual ~iwindow_handle( void_t ) {}
        };
    }
}