
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
            static void_t yield( std::function< bool_t ( void_t ) > funk ) noexcept ;

            static void_t schedule( natus::concurrent::task_res_t, natus::concurrent::schedule_type const ) noexcept ;


            // uses task memory arena for task creation
            // instead of the default one.
            static task_res_t make_task( natus::concurrent::task_t::task_funk_t f ) noexcept ;
        };
        natus_typedef( global ) ;
    }
}