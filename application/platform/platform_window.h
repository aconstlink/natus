

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

            // platform window -> other entity
            natus::std::vector< natus::application::iwindow_message_listener_res_t > _ins ;

            // other entity -> platform window
            natus::std::vector< natus::application::iwindow_message_listener_res_t > _outs ;

        public:

            platform_window( void_t ) {}
            platform_window( this_cref_t ) = delete ;
            platform_window( this_rref_t rhv ) 
            {
                _ins = ::std::move( rhv._ins ) ;
            }
            virtual ~platform_window( void_t ) {}

        public: // ins

            void_t register_in( natus::application::iwindow_message_listener_res_t l )
            {
                _ins.push_back( ::std::move( l ) ) ;
            }

            void_t unregister_in( natus::application::iwindow_message_listener_res_t l )
            {
                auto iter = ::std::find_if( _ins.begin(), _ins.end(), [&]( natus::application::iwindow_message_listener_res_t ls )
                {
                    return ls.get_sptr() == l.get_sptr() ;
                } ) ;

                if( iter == _ins.end() )
                    return ;

                _ins.erase( iter ) ;
            }

            typedef ::std::function< void ( natus::application::iwindow_message_listener_res_t ) > foreach_listener_funk_t ;
            void_t foreach_in( foreach_listener_funk_t funk )
            {
                for( auto & l : _ins )
                {
                    funk( l ) ;
                }
            }

        public: // outs

            void_t register_out( natus::application::iwindow_message_listener_res_t l )
            {
                _outs.push_back( ::std::move( l ) ) ;
            }

            void_t unregister_out( natus::application::iwindow_message_listener_res_t l )
            {
                auto iter = ::std::find_if( _outs.begin(), _outs.end(), [&] ( natus::application::iwindow_message_listener_res_t ls )
                {
                    return ls.get_sptr() == l.get_sptr() ;
                } ) ;

                if( iter == _outs.end() )
                    return ;

                _outs.erase( iter ) ;
            }

            typedef ::std::function< void ( natus::application::iwindow_message_listener_res_t ) > foreach_listener_funk_t ;
            void_t foreach_out( foreach_listener_funk_t funk )
            {
                for( auto& l : _outs )
                {
                    funk( l ) ;
                }
            }
        };
        natus_soil_typedef( platform_window ) ;
    }
}
