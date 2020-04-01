#pragma once

#include "../api.h"
#include "../result.h"
#include "../typedefs.h"
#include "../protos.h"

#include "window_message.h"
#include "../event/toggle_window.h"

namespace natus
{
    namespace application
    {
        class NATUS_APPLICATION_API iwindow
        {
        public:

            /// allows to register a listener for all kinds of window messages.
            virtual natus::application::result subscribe( iwindow_message_listener_rptr_t ) = 0 ;
            virtual natus::application::result unsubscribe( iwindow_message_listener_rptr_t ) = 0 ;

            virtual natus::application::result destroy( void_t ) = 0 ;
            virtual iwindow_handle_rptr_t get_handle( void_t ) = 0 ;

            virtual natus::std::string_cref_t get_name( void_t ) const = 0 ;

            virtual void_t send_close( void_t ) = 0 ;
            virtual void_t send_toggle( natus::application::toggle_window_in_t ) = 0 ;
        };
        natus_typedef( iwindow ) ;
    }
}