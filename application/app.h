
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

        private: // per window info

            struct per_window_info
            {
                natus_this_typedefs( per_window_info ) ;

                natus::concurrent::mutex_t mtx ;
                natus::concurrent::thread_t rnd_thread ;
                natus::application::platform_window_res_t wnd ;
                natus::application::window_message_receiver_res_t msg_recv ;
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
                    natus_move_member_ptr( run, rhv ) ;
                }
                ~per_window_info( void_t ) {}
            };
            natus_typedef( per_window_info ) ;
            typedef natus::std::vector< per_window_info_t > windows_t ;
            natus::concurrent::mutex_t _wmtx ;
            windows_t _windows ;

        public: // async view

            /// This is a view onto a gpu async object. 
            /// This is required because the gpu async has system 
            /// functions which need to be called by the app. 
            /// App needs to access all private stuff.
            class async_view
            {
                natus_this_typedefs( async_view ) ;

                friend class app ;

            private:

                natus::gpu::async_res_t _async ;
                bool_ptr_t _access = nullptr ;

                async_view( natus::gpu::async_res_t const & r, bool_ptr_t b ) : 
                    _async( r ), _access( b ) {}
                async_view( natus::gpu::async_res_t && r, bool_ptr_t b ) : 
                    _async( ::std::move( r ) ), _access( b ) {}
                
            public:

                async_view( void_t ) {}
                async_view( this_cref_t rhv ) : 
                    _async( rhv._async ), _access(rhv._access) {}

                async_view( this_rref_t rhv ) : 
                    _async( ::std::move( rhv._async ) ), _access(rhv._access) {}

                ~async_view( void_t ) {}

                this_ref_t operator = ( this_cref_t rhv ) 
                {
                    _async = rhv._async ;
                    _access = rhv._access ;
                    return *this ;
                }

                this_ref_t operator = ( this_rref_t rhv )
                {
                    _async = ::std::move( rhv._async ) ;
                    _access = rhv._access ;
                    return *this ;
                }

                /// communication to the gpu system is only possible 
                /// during rendering type.
                bool_t is_accessable( void_t ) const noexcept { return *_access ; }

                natus::gpu::result configure( natus::gpu::async_id_res_t id, natus::gpu::geometry_configuration_res_t config ) noexcept 
                {
                    return _async->configure( id, config ) ;
                }

                natus::gpu::result configure( natus::gpu::async_id_res_t id, 
                    natus::gpu::render_configurations_t configs ) noexcept
                {
                    return _async->configure( id, configs ) ;
                }

                natus::gpu::result connect( natus::gpu::async_id_res_t id,
                    natus::gpu::variable_set_res_t vs ) noexcept
                {
                    return _async->connect( id, vs ) ;
                }

                natus::gpu::result render( natus::gpu::async_id_res_t id ) noexcept
                {
                    return _async->render( id ) ;
                }

            public:

            };
            natus_typedef( async_view ) ;

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

            virtual natus::application::result on_event( this_t::window_info_in_t ) 
            { return natus::application::result::ok ; }

        protected:

            typedef ::std::pair< this_t::window_id_t, this_t::async_view_t > wid_async_t ;

            this_t::wid_async_t create_window( 
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
        natus_soil_typedef( app ) ;
    }
}
