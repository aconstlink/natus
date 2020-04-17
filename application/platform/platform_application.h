#pragma once

#include "../api.h"
#include "../protos.h"
#include "../result.h"
#include "../typedefs.h"

#include <natus/concurrent/typedefs.h>

namespace natus
{
    namespace application
    {
        class NATUS_APPLICATION_API platform_application
        {
            natus_this_typedefs( platform_application ) ;

        private:

            natus::application::app_rptr_t _app ;
            
            struct shared_data
            {
                bool_t update_running = false ;
            };
            shared_data* _sd ;

            natus::concurrent::mutex_t _mtx ;
            natus::concurrent::thread_t _thr ;

        public:

            platform_application( void_t ) ;
            platform_application( this_cref_t ) = delete ;
            platform_application( this_rref_t ) ;
            virtual ~platform_application( void_t ) ;

        public:

            virtual natus::application::result set( natus::application::app_rptr_t ) ;
            natus::application::result exec( void_t ) ;

        private:

            natus::application::result start_update_thread( void_t ) ;
            void_t stop_update_thread( void_t ) ;

        public: // interface 

            virtual natus::application::result on_exec( void_t ) = 0 ;
        };
        natus_typedef( platform_application ) ;
        typedef natus::soil::rptr< platform_application_ptr_t > platform_application_rptr_t ;
        typedef natus::soil::rptr< platform_application_ptr_t > application_rptr_t ;
    }
}