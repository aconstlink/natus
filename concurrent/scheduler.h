#pragma once

#include "task/task.hpp"
#include "task/loose_thread_scheduler.hpp"

namespace natus
{
    namespace concurrent
    {
        class NATUS_CONCURRENT_API scheduler
        {
            natus_this_typedefs( scheduler ) ;
            typedef natus::memory::res_t< this_t > this_res_t ;

        private:

            loose_thread_scheduler_t _loose ;

        public:

            struct update_accessor
            {
                static void_t udpate( this_res_t s ) noexcept 
                {
                    s->update() ;
                }
            };
            natus_typedef( update_accessor ) ;
            friend struct update_accessor ;

        private:

            void_t update( void_t ) noexcept ;

        public:

            scheduler( void_t ) noexcept ;
            scheduler( this_cref_t ) = delete ;
            scheduler( this_rref_t ) noexcept ;
            ~scheduler( void_t ) noexcept ;

        public:

            void_t schedule_loose( natus::concurrent::task_res_t ) noexcept ;
        };
    }
}