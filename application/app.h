
#pragma once

#include "api.h"
#include "typedefs.h"
#include "result.h"
#include "protos.h"

#include <natus/concurrent/typedefs.h>
#include <natus/gpu/protos.h>

namespace natus
{
    namespace application
    {
        class NATUS_APPLICATION_API app
        {
            natus_this_typedefs( app ) ;

        public:

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

        private:

            struct per_window_info
            {
                natus_this_typedefs( per_window_info ) ;

                natus::concurrent::mutex_t mtx ;
                natus::concurrent::thread_t rnd_thread ;
                natus::application::platform_window_rptr_t wnd ;
                natus::gpu::async_rptr_t async ;
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
                    natus_move_member_ptr( run, rhv ) ;
                }
                ~per_window_info( void_t ) {}
            };
            natus_typedef( per_window_info ) ;
            typedef natus::std::vector< per_window_info_t > windows_t ;
            natus::concurrent::mutex_t _wmtx ;
            windows_t _windows ;

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

            this_t::window_id_t create_window( 
                natus::std::string_cref_t name, this_t::window_info_in_t ) ;

            

            natus::gpu::async_rptr_t gpu_async( this_t::window_id_t const ) const ;

            natus::application::result request_change( this_t::window_info_in_t ) ;

        private:

            void_t destroy_window( this_t::per_window_info_ref_t ) ;
        };
        
    }
}