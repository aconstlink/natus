#pragma once

#include "itask.h"

namespace natus
{
    namespace concurrent
    {
        class NATUS_CONCURRENT_API serial_executor
        {
            natus_this_typedefs( serial_executor ) ;

            natus_typedefs( natus::std::vector< itask_ptr_t >, tasks ) ;

        public:

            void_t consume( itask_ptr_t ) ;
            void_t consume( tasks_rref_t ) ;
        };
        natus_typedef( serial_executor ) ;
    }
}
