

#pragma once

#include "../api.h"
#include "../typedefs.h"
#include "../protos.h"

#include "window_message.h"
#include "window_message_receiver.hpp"

#include <natus/std/vector.hpp>

#include <algorithm>

namespace natus
{
    namespace application
    {
        class NATUS_APPLICATION_API platform_window
        {
            natus_this_typedefs( platform_window ) ;

        private:

            natus::std::vector< natus::application::iwindow_message_listener_res_t > _listeners ;

        public:

            platform_window( void_t ) {}
            platform_window( this_cref_t ) = delete ;
            platform_window( this_rref_t rhv ) 
            {
                _listeners = ::std::move( rhv._listeners ) ;
            }
            virtual ~platform_window( void_t ) {}

        public:

            void_t register_listener( natus::application::iwindow_message_listener_res_t l )
            {
                _listeners.push_back( ::std::move( l ) ) ;
            }

            void_t unregister_listener( natus::application::iwindow_message_listener_res_t l )
            {
                auto iter = ::std::find_if( _listeners.begin(), _listeners.end(), [&]( natus::application::iwindow_message_listener_res_t ls )
                {
                    return ls.get_sptr() == l.get_sptr() ;
                } ) ;

                if( iter == _listeners.end() )
                    return ;

                _listeners.erase( iter ) ;
            }

            typedef ::std::function< void ( natus::application::iwindow_message_listener_res_t ) > foreach_listener_funk_t ;
            void_t foreach_listener( foreach_listener_funk_t funk )
            {
                for( auto & l : _listeners )
                {
                    funk( l ) ;
                }
            }
        };
        natus_soil_typedef( platform_window ) ;
    }
}
