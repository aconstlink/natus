//------------------------------------------------------------
// snakeoil (c) Alexis Constantin Link
// Distributed under the MIT license
//------------------------------------------------------------
#ifndef _SNAKEOIL_APPLICATION_APPLICATION_XLIB_XLIB_APPLICATION_H_
#define _SNAKEOIL_APPLICATION_APPLICATION_XLIB_XLIB_APPLICATION_H_

#include "../window_application.h"

#include <X11/Xlib.h>

namespace so_app
{
    namespace so_xlib
    {
        class xlib_application : public window_application
        {
            typedef window_application base_t ;
            so_this_typedefs( xlib_application ) ;

        private:
                        
            /// the main display connection. Required to be
            /// a singleton in order to be able for single
            /// windows to be connected to this display.
            /// I would like to manage all windows and dispatch
            /// all window messaged centralized in this 
            /// application.
            static Display * _display ;
            static size_t _display_use_count ;
            
            void_t connect_display( void_t ) ;
            void_t disconnect_display( void_t ) ;
            Display * move_display( void_t ) ;
            
        public:

            xlib_application( void_t ) ;
            xlib_application( this_rref_t ) ;
            virtual ~xlib_application( void_t ) ;

            /// singleton. It is required due to the fact
            /// that windows and application can not be 
            /// brought together.
            static Display * get_display( void_t ) ;
            
        public:

            static this_ptr_t create( void_t ) ;
            static this_ptr_t create( this_rref_t ) ;
            static void_t destroy( this_ptr_t ) ;

        public:

            virtual so_app::result destroy( void_t ) ;
            virtual so_app::result exec_derived( void_t ) ;
            
        };
    }
    
}

#endif

