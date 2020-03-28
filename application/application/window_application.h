#pragma once

#include "iapplication.h"
#include "../window/window_message.h"

//#include <natus/gpx/protos.h>
#include <natus/std/vector.hpp>
#include <natus/concurrent/typedefs.h>

namespace natus
{
    namespace application
    {
        class NATUS_APPLICATION_API window_application : public iapplication
        {
            natus_this_typedefs( window_application ) ;

            struct window_data
            {
                iwindow_ptr_t window_ptr ;
            };
            typedef natus::std::vector< window_data > windows_t ;

        public:

            class application_state
            {
                natus_this_typedefs( application_state ) ;

                friend class window_application ;

            public:

                application_state( void_t ) {}
                application_state( this_cref_t ) = delete ;
                application_state( this_rref_t rhv ) { _running = rhv._running ; }
                ~application_state( void_t ) {}

            public:

                bool_t is_running( void_t ) const
                {
                    return _running ;
                }

                void_t update_finised( void_t )
                {
                    {
                        natus::concurrent::lock_guard_t lk( _mtx ) ;
                        update_ended = true ;
                    }
                    _cv.notify_all() ;
                }

            private:

                mutable natus::concurrent::mutex_t _mtx ;
                mutable natus::concurrent::condition_variable_t _cv ;
                bool_t update_ended = true ;

                bool_t _running = true ;

            private:

                void_t stop( void_t ) { _running = false ; }

                void_t update_begin( void_t )
                {
                    natus::concurrent::lock_t lk( _mtx ) ;
                    update_ended = false ;
                }

                void_t wait_for_finish( void_t ) const
                {
                    natus::concurrent::lock_t lk( _mtx ) ;
                    while( natus::core::is_not( update_ended ) ) _cv.wait( lk ) ;
                }
            };
            natus_typedef( application_state ) ;

        private:

            /// all registered windows
            windows_t _windows ;

            /// this application provides one update thread
            /// where all user data is executed.
            natus::concurrent::thread_t _update_thread ;

            /// allows to inform the update thread owner how the application is
            /// it may also be possible to reveice window events and discover the
            /// same window events
            application_state_ptr_t _app_state_ptr = nullptr  ;

        private:

            /// non-copyable
            window_application( this_cref_t ) {}

        public:

            window_application( void_t ) ;
            window_application( this_rref_t ) ;
            virtual ~window_application( void_t ) ;

            /// register a non rendering window.
            natus::application::result register_window( natus::application::iwindow_ptr_t wptr ) ;

        public: // interface

            virtual natus::application::result destroy( void_t ) = 0 ;

            /// overwritten in order to start the internal update thread.
            /// this way, the start of the thread can be deferred to this point
            /// which allows to omit certain synchronization.
            /// a derived call is required to implement exec_derived() in order 
            /// to do its execution work.
            virtual natus::application::result exec( void_t ) final ;

            /// allows to specify the update function. The function will
            /// be executed in parallel.
            natus::application::result exec( ::std::function< void_t( application_state_ptr_t ) > ) ;

        protected:

            virtual natus::application::result exec_derived( void_t ) = 0 ;

        protected:

            /// designed to be called from a derived class object.
            natus::application::result shutdown_and_kill_all_windows( void_t ) ;

        private:

            natus::application::result shutdown_update_thread( void_t ) ;
        };
        natus_typedef( window_application ) ;
    }
}