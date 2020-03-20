#pragma once

#include "itask.h"
#include "../mutex.h"

#include <vector>

namespace natus
{
    namespace concurrent
    {
        class NATUS_CONCURRENT_API task : public itask
        {
            natus_this_typedefs( task ) ;

            natus_typedefs( itask::tasks_t, tasks ) ;

        private:

            natus::concurrent::mutex_t _mtx ;

            /// tasks outgoing from this task
            tasks_t _tasks_out ;

            /// tasks incoming into this task
            tasks_t _tasks_in ;
            size_t _num_tasks_in = 0 ;

        private:

            task( this_cref_t ) {}

        public:

            task( void_t ) ;
            task( this_rref_t ) ;
            virtual ~task( void_t ) ;

        public:

            virtual itask_ptr_t then( itask_ptr_t ) final ;
            virtual void_t execute( locked_tasks_ref_t ) final ;

        public: // interface

            virtual void_t run( void_t ) = 0 ;
            virtual void_t destroy( void_t ) = 0 ;

        private:

            virtual size_t connect_incoming( itask_ptr_t ) final ;
            virtual size_t disconnect_incoming( itask_ptr_t ) final ;

        };
    }
}