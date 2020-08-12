
#pragma once

#include "api.h"
#include "typedefs.h"
#include "result.h"
#include "protos.h"
#include "platform/platform_window.h"

#include <natus/gpu/async.h>
#include <natus/concurrent/typedefs.h>

namespace natus
{
    namespace application
    {
        class NATUS_APPLICATION_API app
        {
            natus_this_typedefs( app ) ;

            friend class platform_application ;

        public: // window info

            typedef size_t window_id_t ;

            struct window_info
            {
                int_t x = 100 ;
                int_t y = 100 ;
                int_t w = 400 ;
                int_t h = 400 ;

                bool_t vsync = true ;
                bool_t borders = true ;
                bool_t fullscreen = false ;
            };
            natus_typedef( window_info ) ;

            struct window_event_info
            {                
                uint_t w = 400 ;
                uint_t h = 400 ;
            };
            natus_typedef( window_event_info ) ;

        public: // window accessor

            class NATUS_APPLICATION_API window_view
            {
                natus_this_typedefs( window_view ) ;

                friend class app ;

            private:

                window_id_t _id ;
                natus::application::window_message_receiver_res_t _msg_wnd ;
                natus::application::window_message_receiver_res_t _msg_gfx ;

            public: 

                window_view( void_t ) ;
                window_view( this_rref_t ) ;
                window_view( this_cref_t ) ;
                ~window_view( void_t ) ;

                this_ref_t operator = ( this_rref_t ) ;
                this_ref_t operator = ( this_cref_t ) ;

            private:

                window_view( window_id_t id, natus::application::window_message_receiver_res_t wnd,
                    natus::application::window_message_receiver_res_t gfx ) ;

            public:

                window_id_t id( void_t ) const noexcept ;
                void_t resize( size_t const w, size_t const h ) noexcept ;
                void_t vsync( bool_t const ) noexcept ;
                void_t fullscreen( bool_t const ) noexcept ;
            };
            natus_res_typedef( window_view ) ;

        private: // per window info

            struct per_window_info
            {
                natus_this_typedefs( per_window_info ) ;

                natus::concurrent::mutex_t mtx ;
                natus::concurrent::thread_t rnd_thread ;
                natus::application::platform_window_res_t wnd ;
                natus::application::window_message_receiver_res_t msg_recv ;
                natus::application::window_message_receiver_res_t msg_send ;
                natus::application::window_message_receiver_res_t gfx_send ;
                natus::gpu::async_res_t async ;
                window_info_t wi ;
                bool_ptr_t run ;
                per_window_info( void_t ) {}
                per_window_info( this_cref_t ) = delete ;
                per_window_info( this_rref_t rhv ) 
                {
                    rnd_thread = ::std::move( rhv.rnd_thread ) ;
                    wnd = ::std::move( rhv.wnd ) ;
                    async = ::std::move( rhv.async ) ;
                    wi = ::std::move( rhv.wi ) ;
                    msg_recv = ::std::move( rhv.msg_recv ) ;
                    msg_send = ::std::move( rhv.msg_send ) ;
                    natus_move_member_ptr( run, rhv ) ;
                }
                ~per_window_info( void_t ) {}
            };
            natus_typedef( per_window_info ) ;
            typedef natus::std::vector< per_window_info_t > windows_t ;
            natus::concurrent::mutex_t _wmtx ;
            windows_t _windows ;

        private:

            // async view access
            bool_ptr_t _access = nullptr ;
            size_t _update_count = 0 ;
            size_t _render_count = 0 ;

        public:

            app( void_t ) ;
            app( this_cref_t ) = delete ;
            app( this_rref_t ) ;
            virtual ~app( void_t ) ;

        public:

            virtual natus::application::result on_init( void_t ) = 0 ;
            virtual natus::application::result on_update( void_t ) = 0 ;
            virtual natus::application::result on_render( void_t ) = 0 ;
            virtual natus::application::result on_shutdown( void_t ) = 0 ;

            virtual natus::application::result on_event( window_id_t const, this_t::window_event_info_in_t ) 
            { return natus::application::result::ok ; }

        protected:

            typedef ::std::pair< this_t::window_view_t, natus::gpu::async_view_t > window_async_t ;

            this_t::window_async_t create_window( 
                natus::std::string_cref_t name, this_t::window_info_in_t ) ;

            natus::application::result request_change( this_t::window_info_in_t ) ;

        private:

            void_t destroy_window( this_t::per_window_info_ref_t ) ;

        private: // platform application interface

            bool_t before_update( void_t ) ;
            bool_t after_update( void_t ) ;
            bool_t before_render( void_t ) ;
            bool_t after_render( void_t ) ;

        };
        natus_res_typedef( app ) ;
    }
}
