#pragma once

#include "itask_scheduler.h"

#include "../primitive/sync_object.hpp"

namespace natus
{
    namespace concurrent
    {
        class NATUS_CONCURRENT_API task_scheduler : public itask_scheduler
        {
            natus_this_typedefs( task_scheduler ) ;

            natus_typedefs( natus::std::vector< itask_ptr_t >, tasks ) ;

        private:

            natus::concurrent::mutex_t _mtx_serial ;
            tasks_t _serials ;

            natus::concurrent::mutex_t _mtx_async ;
            tasks_t _asyncs ;

        public:

            task_scheduler( void_t ) ;
            task_scheduler( this_rref_t ) ;
            virtual ~task_scheduler( void_t ) ;

        public:

            static this_ptr_t create( natus::memory::purpose_cref_t ) ;
            static this_ptr_t create( this_rref_t, natus::memory::purpose_cref_t ) ;
            static void_t destroy( this_ptr_t ) ;


        public:

            virtual void_t update( void_t ) ;

        public:

            virtual void_t async_now( natus::concurrent::itask_ptr_t, natus::concurrent::sync_object_ptr_t = nullptr ) ;
            virtual void_t async_now( natus::concurrent::task_graph_rref_t, natus::concurrent::sync_object_ptr_t = nullptr ) ;
            virtual void_t serial_now( natus::concurrent::itask_ptr_t, natus::concurrent::sync_object_ptr_t = nullptr ) ;
            virtual void_t async_on_update( natus::concurrent::itask_ptr_t ) ;
            virtual void_t serial_on_update( natus::concurrent::itask_ptr_t ) ;

        public:

            virtual void_t destroy( void_t ) ;
        };
        natus_typedef( task_scheduler ) ;
    }
}