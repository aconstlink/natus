
#pragma once

#include "../api.h"
#include "../typedefs.h"

#include "window_message.h"

#include <natus/concurrent/mutex.hpp>

namespace natus
{
    namespace application
    {
        // system level listener facility
        // nothing the user ever sees.
        class NATUS_APPLICATION_API iwindow_message_listener
        {
        public: // callbacks

            virtual void_t on_screen( screen_dpi_message_cref_t ) noexcept = 0 ;
            virtual void_t on_screen( screen_size_message_cref_t ) noexcept = 0 ;

            virtual void_t on_resize( resize_message_cref_t ) noexcept = 0 ;
            virtual void_t on_visible( show_message_cref_t ) noexcept = 0 ;
            virtual void_t on_close( close_message_cref_t ) noexcept = 0 ;

            virtual void_t on_vsync( vsync_message_cref_t ) noexcept = 0 ;
            virtual void_t on_fullscreen( fullscreen_message_cref_t ) noexcept = 0 ;
        };
        natus_soil_typedef( iwindow_message_listener ) ;

        class window_message_receiver : public iwindow_message_listener
        {
            natus_this_typedefs( window_message_receiver ) ;

        public: 

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

                natus::application::vsync_message vsync_msg ;
                bool_t vsync_msg_changed = false ;

                natus::application::fullscreen_message fulls_msg ;
                bool_t fulls_msg_changed = false ;
            };
            natus_typedef( state_vector ) ;
            typedef bool_t change_state_t ;

        private:

            /// lock the states vector
            natus::concurrent::mutex_t _mtx ;
            state_vector_t _states ;
            change_state_t _has_any_change = false ;

        public:

            window_message_receiver( void_t ) {}
            window_message_receiver( this_cref_t rhv )
            {
                _states = rhv._states ;
                _has_any_change = rhv._has_any_change ;
            }
            virtual ~window_message_receiver( void_t ) {}

            bool_t swap_and_reset( state_vector_out_t states ) noexcept
            {
                natus::concurrent::lock_guard_t lk( _mtx ) ;
                bool_t changed = _has_any_change ;
                if( changed )
                {
                    states = _states ;
                }
                this_t::reset_change_flags() ;
                return changed ;
            }

        private:

            void_t reset_change_flags( void_t ) noexcept
            {
                _states.resize_changed = false ;
                _states.show_changed = false ;
                _states.close_changed = false ;
                _states.msize_msg_changed = false ;
                _states.dpi_msg_changed = false ;
                _states.vsync_msg_changed = false ;
                _states.fulls_msg_changed = false ;
                _has_any_change = false ;
            }

        public: // listener interface

            virtual void_t on_screen( screen_dpi_message_cref_t msg ) noexcept
            {
                natus::concurrent::lock_t lk( _mtx ) ;
                _states.dpi_msg_changed = true ;
                _states.dpi_msg = msg ;
                _has_any_change = true ;
            }

            virtual void_t on_screen( screen_size_message_cref_t msg ) noexcept
            {
                natus::concurrent::lock_t lk( _mtx ) ;
                _states.msize_msg_changed = true ;
                _states.msize_msg = msg ;
                _has_any_change = true ;
            }

            virtual void_t on_resize( resize_message_cref_t msg ) noexcept
            {
                natus::concurrent::lock_t lk( _mtx ) ;
                _states.resize_changed = true ;
                _states.resize_msg = msg ;
                _has_any_change = true ;
            }

            virtual void_t on_visible( show_message_cref_t msg ) noexcept
            {
                natus::concurrent::lock_t lk( _mtx ) ;
                _states.show_changed = true ;
                _states.show_msg = msg ;
                _has_any_change = true ;
            }

            virtual void_t on_close( close_message_cref_t msg ) noexcept
            {
                natus::concurrent::lock_t lk( _mtx ) ;
                _states.close_changed = true ;
                _states.close_msg = msg ;
                _has_any_change = true ;
            }

            virtual void_t on_vsync( vsync_message_cref_t msg ) noexcept
            {
                natus::concurrent::lock_t lk( _mtx ) ;
                _states.vsync_msg_changed = true ;
                _states.vsync_msg = msg ;
                _has_any_change = true ;
            }

            virtual void_t on_fullscreen( fullscreen_message_cref_t msg ) noexcept
            {
                natus::concurrent::lock_t lk( _mtx ) ;
                _states.fulls_msg_changed = true ;
                _states.fulls_msg = msg ;
                _has_any_change = true ;
            }
        };
        natus_soil_typedef( window_message_receiver ) ;
    }
}