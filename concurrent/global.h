
#pragma once

#include "api.h"
#include "typedefs.h"
#include "protos.h"
#include "mutex.hpp"

namespace natus
{
    namespace concurrent
    {
        class NATUS_CONCURRENT_API global
        {
            natus_this_typedefs( global ) ;

        private:

            job_scheduler_ptr_t _job_scheduler_ptr = nullptr ;
            //itask_scheduler_ptr_t _task_scheduler_ptr = nullptr ;

        private:

            static mutex_t _mtx ;
            static this_ptr_t _ptr ;

        public:

            global( void_t ) ;
            global( this_cref_t ) = delete ;
            global( this_rref_t ) ;
            ~global( void_t ) ;

        public:

            static this_ptr_t create( this_rref_t, natus::memory::purpose_cref_t ) ;
            static void_t destroy( this_ptr_t ) ;

        public:

            static this_ptr_t init( void_t ) ;
            static void_t deinit( void_t ) ;
            static void_t update( void_t ) ;
            static this_ptr_t get( void_t ) ;
            static job_scheduler_ptr_t job_scheduler( void_t ) ;
            //static itask_scheduler_ptr_t task_scheduler( void_t ) ;
        };
        natus_typedef( global ) ;
    }
}