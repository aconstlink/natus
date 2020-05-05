#pragma once

#include "../platform_window.h"
#include "../window_info.h"
#include "../toggle_window.h"

#include <vector>
#include <windows.h>

namespace natus
{
    namespace application
    {
        namespace win32
        {
            class NATUS_APPLICATION_API window : public platform_window
            {
                natus_this_typedefs( window ) ;

            private:

                HWND _handle = NULL ;

                bool_t _is_fullscreen = false ;
                bool_t _is_cursor = false ;

                HCURSOR _cursor = NULL ;

                natus::std::string _name = "WIN32 Window";

            public:

                window( void_t ) ;
                window( window_info_cref_t ) ;
                window( this_cref_t rhv ) = delete ;
                window( this_rref_t rhv ) ;
                virtual ~window( void_t ) ;

            public: // interface

                HWND get_handle( void_t ) ;

                void_t send_toggle( natus::application::toggle_window_in_t ) ;

                void_t show_window(  window_info const & wi ) ;

            private:

                HWND create_window( window_info const& ) ;
                void_t destroy_window( void_t ) ;


            protected: // virtual

                /// Here the real code is executed in order to do the callback actions.
                virtual LRESULT CALLBACK WndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam ) ;

            public:

                /// used to pass to windows for the callback. It is the wrapper that calls the real proc function.
                static LRESULT CALLBACK StaticWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam ) ;

            private:

                void_t send_show( WPARAM ) ;
                void_t send_resize( HWND ) ;
                void_t send_destroy( void_t ) ;
            };
            natus_typedef( window ) ;
            typedef natus::soil::res< window > window_res_t ;
        }
    }
}
