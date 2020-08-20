#pragma once

#include "itask.h"

#include <future>

namespace natus
{
    namespace concurrent
    {
        class NATUS_CONCURRENT_API async_executor
        {
            natus_this_typedefs( async_executor ) ;

            typedef ::std::future<void_t> future_t ;
            natus_typedefs( natus::ntd::vector<future_t>, futures ) ;

            natus_typedefs( natus::ntd::vector< itask_ptr_t >, tasks ) ;

        public:

            /// blocks until the task graph is processed.
            /// all tasks will be destroyed after completion.
            void_t consume_and_wait( natus::concurrent::itask_ptr_t ) ;

            void_t consume_and_wait( tasks_rref_t ) ;

        };
        natus_typedef( async_executor ) ;
    }
}