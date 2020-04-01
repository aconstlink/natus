#pragma once

#include "../api.h"
#include "../protos.h"

#include "window_message.h"

namespace natus
{
    namespace application
    {
        class NATUS_APPLICATION_API iwindow_message_listener
        {
        public: // callbacks

            virtual void_t on_screen( screen_dpi_message_cref_t ) = 0 ;
            virtual void_t on_screen( screen_size_message_cref_t ) = 0 ;

            virtual void_t on_resize( resize_message_cref_t ) = 0 ;
            virtual void_t on_visible( show_message_cref_t ) = 0 ;
            virtual void_t on_close( close_message_cref_t ) = 0 ;

        public: // getter

            virtual bool_t has_screen_changed( screen_dpi_message_ref_t ) = 0 ;
            virtual bool_t has_screen_changed( screen_size_message_ref_t ) = 0 ;

            virtual bool_t has_resize_changed( resize_message_ref_t ) = 0 ;
            virtual bool_t has_visible_changed( show_message_ref_t ) = 0 ;
            virtual bool_t has_close_changed( close_message_ref_t ) = 0 ;

        public: // management 

            /// the listener destroys itself.
            virtual void_t destroy( void_t ) = 0 ;
        };
        natus_typedef( iwindow_message_listener ) ;
        
    }
}
