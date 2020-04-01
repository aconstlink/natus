#pragma once

#include "../iwindow.h"
#include "../window_info.h"

#include "win32_info.h"
#include "win32_window_handle.h"

#include <vector>

namespace natus
{
    namespace application
    {
        namespace win32
        {
            class NATUS_APPLICATION_API win32_window : public iwindow
            {
                natus_this_typedefs( win32_window ) ;

                typedef natus::std::vector< natus::application::iwindow_listener_ptr_t > listeners_t ;
                typedef natus::std::vector< natus::application::iwindow_message_listener_rptr_t > msg_listeners_t ;

            private:

                win32_window_handle_rptr_t _handle ;

                msg_listeners_t _msg_listeners ;

                natus::std::string _name ;

                bool_t _is_fullscreen = false ;
                bool_t _is_cursor = false ;

                HCURSOR _cursor = NULL ;

            public:

                win32_window( void_t ) ;
                win32_window( window_info const& ) ;
                win32_window( win32_window_handle_rref_t ) ;
                win32_window( this_rref_t rhv ) ;
                virtual ~win32_window( void_t ) ;

            public: // interface

                static this_ptr_t create( this_rref_t rhv ) ;
                static this_ptr_t create( this_rref_t rhv, natus::memory::purpose_cref_t msg ) ;
                static void_t destroy( this_ptr_t rhv ) ;


                virtual natus::application::result subscribe( iwindow_message_listener_rptr_t ) ;
                virtual natus::application::result unsubscribe( iwindow_message_listener_rptr_t ) ;

                virtual natus::application::result destroy( void_t ) ;
                virtual iwindow_handle_rptr_t get_handle( void_t ) ;

                virtual std::string const& get_name( void_t ) const ;

                virtual void_t send_close( void_t ) ;
                virtual void_t send_toggle( natus::application::toggle_window_in_t ) ;

            private:

                HWND create_window( window_info const& ) ;
                void_t destroy_window( void_t ) ;

                void_t remove_invalid_window_listeners( void_t ) ;

            private:

                void_t send_resize( HWND ) ;

            private:

                void_t get_monitor_info( HWND, MONITORINFO& ) ;
                void_t send_screen_dpi( HWND ) ;
                void_t send_screen_dpi( HWND, uint_t dpix, uint_t dpiy ) ;
                void_t send_screen_size( HWND ) ;
                void_t send_screen_size( HWND, uint_t width, uint_t height ) ;

            protected: // virtual

                /// Here the real code is executed in order to do the callback actions.
                virtual LRESULT CALLBACK WndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam ) ;

            public:

                /// used to pass to windows for the callback. It is the wrapper that calls the real proc function.
                static LRESULT CALLBACK StaticWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam ) ;
            };
            natus_typedefs( win32_window, win32_window ) ;
            typedef natus::soil::rptr< win32_window_ptr_t > win32_window_rptr_t ;
        }
    }
}