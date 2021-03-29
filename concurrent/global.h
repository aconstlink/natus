
#pragma once

#include "api.h"
#include "typedefs.h"
#include "protos.h"
#include "mutex.hpp"
#include "sync_object.hpp"
#include "task/task.hpp"

namespace natus
{
    namespace concurrent
    {
        class NATUS_CONCURRENT_API global
        {
            natus_this_typedefs( global ) ;

        private:

            struct singleton_data ;
            static singleton_data * _dptr ;
            static mutex_t _mtx ;

        private:

            static singleton_data * init( void_t ) noexcept ;

        public: 

            static void_t deinit( void_t ) ;
            static void_t update( void_t ) ;

            // yield the current thread of execution 
            // until the sync object is signaled
            // if the thread is part of the thread pool, the thread
            // will be exchanged with another worker until the old thread  
            // resumes execution.
            static void_t yield( natus::concurrent::sync_object_res_t ) noexcept ;

            static void_t schedule( natus::concurrent::task_res_t, natus::concurrent::schedule_type const ) noexcept ;
        };
        natus_typedef( global ) ;
    }
}