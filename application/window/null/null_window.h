#pragma once

#include "../iwindow.h"

#include "null_window_handle.h"

// @todo gpu gpu
//#include <natus/gpx/window/iwindow.h>
//#include <natus/gpu/api/null/null_api.h>

namespace natus
{
    namespace application
    {
        namespace null
        {
            class NATUS_APPLICATION_API null_window : public iwindow//, public natus::gpx::iwindow
            {
                natus_this_typedefs( null_window ) ;

            private:

                iwindow_handle_rptr_t _handle ;
                natus::std::string_t _name = "null_window" ;

                // @todo gpu
                //natus::gpu::natus_null::null_api_ptr_t _api = nullptr ;

            public:

                null_window( void_t ) ;
                null_window( this_cref_t ) = delete ;
                null_window( this_rref_t ) ;
                virtual ~null_window( void_t ) ;

            public:

                static this_ptr_t create( natus::memory::purpose_cref_t ) ;
                static void_t destroy( this_ptr_t ) ;

            public:

                virtual natus::application::result subscribe( iwindow_message_listener_rptr_t ) ;
                virtual natus::application::result unsubscribe( iwindow_message_listener_rptr_t ) ;

                virtual natus::application::result destroy( void_t ) ;
                virtual iwindow_handle_rptr_t get_handle( void_t ) ;

                virtual natus::std::string_cref_t get_name( void_t ) const ;
                virtual void_t send_close( void_t ) ;
                virtual void_t send_toggle( natus::application::toggle_window_in_t ) {}

            public: // gpu iwindow interface

                virtual void_t render_thread_begin( void_t ) ;
                virtual void_t render_thread_end( void_t ) ;
                virtual void_t end_frame( void_t ) ;
                
                // @todo gpu
                //virtual natus_gpu::iapi_ptr_t get_api( void_t ) ;
                //virtual natus_gpu::api_type get_api_type( void_t ) ;
            };
            natus_typedef( null_window ) ;
        }
    }
}