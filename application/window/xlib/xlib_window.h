//------------------------------------------------------------
// snakeoil (c) Alexis Constantin Link
// Distributed under the MIT license
//------------------------------------------------------------
#ifndef _SNAKEOIL_APPLICATION_WINDOW_XLIB_XLIB_WINDOW_H_
#define _SNAKEOIL_APPLICATION_WINDOW_XLIB_XLIB_WINDOW_H_

#include "../iwindow.h"
#include "../window_info.h"

#include "xlib_window_handle.h"

#include <vector>

namespace so_app
{
    namespace so_xlib
    {
        class xlib_window : public iwindow
        {
            so_this_typedefs( xlib_window ) ;

            typedef std::vector< so_app::iwindow_listener_ptr_t > listeners_t ;
            typedef std::vector< so_app::iwindow_message_listener_ptr_t > msg_listeners_t ;

        private:

            xlib_window_handle _handle ;
            
            msg_listeners_t _msg_listeners ;

            std::string _name ;

        public:

            xlib_window( window_info const & ) ;
            xlib_window( Display * display, Window wnd ) ;
            
            xlib_window( this_rref_t rhv ) ;
            virtual ~xlib_window( void_t ) ;

        public: // interface

            static this_ptr_t create( this_rref_t rhv ) ;
            static this_ptr_t create( this_rref_t rhv, std::string const & msg ) ;
            static void_t destroy( this_ptr_t rhv ) ;

            
            virtual so_app::result subscribe( iwindow_message_listener_ptr_t ) ;
            virtual so_app::result unsubscribe( iwindow_message_listener_ptr_t ) ;

            virtual so_app::result destroy( void_t ) ;
            virtual iwindow_handle_ptr_t get_handle( void_t ) ;
            
            virtual std::string const & get_name( void_t ) ;

            /// allows to set a xevent straight to the window.
            /// this is primarilly used by the xlib application.
            virtual void_t xevent_callback( XEvent const & event ) ;
            
        private:

            void_t create_window( Display * display, Window wnd ) ;
            void_t create_window( window_info const & ) ;
            void_t destroy_window( void_t ) ;

            void_t remove_invalid_window_listeners( void_t ) ;

            /// we store the this pointer within xlib so it
            /// can be retrieved later on in the application's
            /// main loop
            void_t store_this_ptr_in_atom( Display * display, Window wnd ) ;
            
        private:

            void_t send_resize( so_app::resize_message const & ) ;
            
            
        private: 
            
            void_t send_screen_dpi( xlib_window_handle_ptr_t ) ;
            void_t send_screen_dpi( xlib_window_handle_ptr_t, uint_t dpix, uint_t dpiy ) ;
            void_t send_screen_size( xlib_window_handle_ptr_t ) ;
            void_t send_screen_size( xlib_window_handle_ptr_t, uint_t width, uint_t height ) ;

        protected: // virtual

        };
        so_typedefs( xlib_window, xlib_window ) ;
    }
}
    
#endif

