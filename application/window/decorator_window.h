#pragma once

#include "iwindow.h"

namespace natus::application
{
    namespace application
    {
        /// The decorator window allows to create a chain of
        /// windows. It was designed primarily to support render windows
        /// that decorate normal system windows. This in turn allows the
        /// render windows(or any decorating window) to access the 
        /// decorated window for further window specific information.
        class NATUS_APPLICATION_API decorator_window : public iwindow
        {
            natus_this_typedefs( decorator_window ) ;

        private:

            iwindow_ptr_t _decorated_ptr = nullptr ;

        public:

            decorator_window( void_t ) ;
            decorator_window( iwindow_ptr_t wnd_ptr ) ;
            decorator_window( this_rref_t ) ;
            virtual ~decorator_window( void_t ) ;

        public: // interface

            virtual natus::application::result subscribe( iwindow_message_listener_ptr_t ) ;
            virtual natus::application::result unsubscribe( iwindow_message_listener_ptr_t ) ;

            virtual natus::application::result destroy( void_t ) ;
            virtual iwindow_handle_ptr_t get_handle( void_t ) ;

            virtual natus::std::string_cref_t get_name( void_t ) const final ;

            virtual void_t send_close( void_t ) ;
            virtual void_t send_toggle( natus::application::toggle_window_in_t ) ;

        public:

            bool_t has_decorated( void_t ) const ;
            bool_t has_no_decorated( void_t ) const ;

        protected:

            natus::application::result set_decorated( iwindow_ptr_t wnd_ptr ) ;
            iwindow_ptr_t get_decorated( void_t ) ;
        };
    }
}