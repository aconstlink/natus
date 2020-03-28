#pragma once

#include "../api.h"
#include "../protos.h"
#include "../result.h"
#include "../typedefs.h"

#include "iwindow_message_listener.h"

#include <natus/concurrent/typedefs.h>
#include <natus/concurrent/container/double_buffered/item.hpp>

namespace natus
{
    namespace application
    {
        class window_message_receiver : public iwindow_message_listener
        {
            natus_this_typedefs( window_message_receiver ) ;

            struct state_vector
            {
                natus::application::resize_message resize_msg ;
                bool_t resize_changed = false ;

                natus::application::show_message show_msg ;
                bool_t show_changed = false ;

                natus::application::close_message close_msg ;
                bool_t close_changed = false ;

                natus::application::screen_dpi_message dpi_msg ;
                bool_t dpi_msg_changed = false ;

                natus::application::screen_size_message msize_msg ;
                bool_t msize_msg_changed = false ;
            };

            natus_typedef( state_vector ) ;
            typedef natus::concurrent::natus_double_buffered::item< state_vector > state_vector_db_t ;
            typedef natus::concurrent::natus_double_buffered::item< bool_t > change_state_t ;

        private:

            /// lock the states vector
            natus::concurrent::mutex_t _mtx ;
            state_vector_db_t _states ;

            change_state_t _has_any_change ;

        public:

            static NATUS_APPLICATION_API this_ptr_t create( natus::std::string_cref_t purpose ) ;
            static NATUS_APPLICATION_API this_ptr_t create( this_rref_t, 
                natus::std::string_cref_t purpose ) ;
            static NATUS_APPLICATION_API void_t destroy( this_ptr_t ) ;

        private:

            /// non-copyable
            window_message_receiver( this_cref_t ) {}

        public:

            window_message_receiver( void_t ) ;
            window_message_receiver( this_rref_t ) ;
            virtual ~window_message_receiver( void_t ) ;

            /// swap the read/write buffers.
            NATUS_APPLICATION_API void_t swap( void_t ) ;

            NATUS_APPLICATION_API bool_t has_any_change( void_t ) ;

        private:

            /// sets all changes flags to false for
            /// the current write buffer.
            void_t reset_change_flags( void_t ) ;

            void_t set_changed( void_t ) ;

        public: // listener interface

            virtual void_t on_screen( screen_dpi_message_cref_t ) ;
            virtual void_t on_screen( screen_size_message_cref_t ) ;

            virtual void_t on_resize( resize_message_cref_t ) ;
            virtual void_t on_visible( show_message_cref_t ) ;
            virtual void_t on_close( close_message_cref_t ) ;

            virtual bool_t has_screen_changed( screen_dpi_message_ref_t ) ;
            virtual bool_t has_screen_changed( screen_size_message_ref_t ) ;

            virtual bool_t has_resize_changed( resize_message_ref_t ) ;
            virtual bool_t has_visible_changed( show_message_ref_t ) ;
            virtual bool_t has_close_changed( close_message_ref_t ) ;

            virtual void_t destroy( void_t ) ;
        };
    }
}