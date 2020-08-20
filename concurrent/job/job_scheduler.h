#pragma once

#include "job.h"
#include "../mutex.hpp"
#include "../typedefs.h"

#include <natus/ntd/vector.hpp>

namespace natus
{
    namespace concurrent
    {
        class NATUS_CONCURRENT_API job_scheduler
        {
            natus_this_typedefs( job_scheduler ) ;

            natus_typedefs( natus::ntd::vector< natus::concurrent::thread_t >, threads ) ;

        public:

            natus_typedefs( ::std::function < void_t ( natus::concurrent::ijob_ptr_t ) >, finish_callback ) ;

        private:

            struct job_data
            {
                natus::concurrent::ijob_ptr_t job_ptr ;
                finish_callback_t ff ;
            };
            natus_typedef( job_data ) ;

            natus_typedefs( natus::ntd::vector< job_data_t >, jobs ) ;

        private:

            struct shared_data
            {
                natus_this_typedefs( shared_data ) ;

            public:

                shared_data( void_t ) {}
                shared_data( this_cref_t ) = delete ;
                shared_data( this_rref_t rhv )
                {
                    _jobs = ::std::move( rhv._jobs ) ;
                    _jpos = rhv._jpos ;
                    _running = rhv._running ;
                }
                ~shared_data( void_t ) {}

            public:

                natus::concurrent::mutex_t _mtx_jobs ;
                natus::concurrent::condition_variable_t _cv_jobs ;
                jobs_t _jobs ;
                size_t _jpos = 0 ;

                bool_t _running = true ;


                natus::concurrent::mutex_t _mtx_online ;
                natus::concurrent::condition_variable_t _cv_online ;
                size_t _threads_online = 0 ;
            };
            natus_typedef( shared_data ) ;

        private:

            threads_t _threads ;
            shared_data_ptr_t _sd_ptr = nullptr ;

        public:

            job_scheduler( void_t ) ;
            job_scheduler( this_cref_t ) = delete ;
            job_scheduler( this_rref_t ) ;
            ~job_scheduler( void_t ) ;

        public:

            static this_ptr_t create( natus::memory::purpose_cref_t ) ;
            static void_t destroy( this_ptr_t ) ;

        public:

            natus::concurrent::result schedule( natus::concurrent::ijob_ptr_t, finish_callback_t ) ;

        private:

            void_t wait_for_threads_online( size_t const num_threads ) ;

        };
        natus_typedef( job_scheduler ) ;
    }
}