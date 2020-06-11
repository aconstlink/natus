#pragma once

#include "../api.h"
#include "../protos.h"
#include "../result.h"
#include "../typedefs.h"
#include "../app.h"

#include <natus/concurrent/typedefs.h>

namespace natus
{
    namespace application
    {
        class NATUS_APPLICATION_API platform_application
        {
            natus_this_typedefs( platform_application ) ;

        private:

            natus::application::app_res_t _app ;
            
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
            platform_application( natus::application::app_res_t ) ;
            virtual ~platform_application( void_t ) ;

        public:
            
            int_t exec( void_t ) ;

        private:

            /// at the moment, the app object must be added by the applications' ctor
            /// this is due to multi-threading issues when creating a window in the app.
            natus::application::result set( natus::application::app_res_t ) ;

            natus::application::result start_update_thread( void_t ) ;
            void_t stop_update_thread( void_t ) ;

        public: // interface 

            virtual natus::application::result on_exec( void_t ) = 0 ;
        };
        natus_soil_typedef( platform_application ) ;
    }
}