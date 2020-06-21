
#pragma once

#include "async_id.hpp"
#include "backend/backend.h"

#include <natus/concurrent/typedefs.h>
#include <natus/std/vector.hpp>

namespace natus
{
    namespace gpu
    {
        class NATUS_GPU_API async
        {
            natus_this_typedefs( async ) ;

        private:

            /// decorated backend.
            backend_res_t _backend ;

            bool_t _window_info_set = false  ;
            natus::gpu::backend_t::window_info_t _window_info ;
            natus::concurrent::mutex_t _window_info_mtx ;

            struct shader_config_data
            {
                natus::gpu::result_res_t res ;
                natus::gpu::shader_configuration_res_t config ;
            };
            typedef natus::std::vector< shader_config_data > shader_configs_t ;
            shader_configs_t _shader_configs ;
            natus::concurrent::mutex_t _shader_configs_mtx ;

            struct rconfig_data
            {
                natus::gpu::result_res_t res ;
                natus::gpu::render_configuration_res_t config ;
            };
            typedef natus::std::vector< rconfig_data > rconfigs_t ;
            rconfigs_t _rconfigs ;
            natus::concurrent::mutex_t _rconfigs_mtx ;

            struct connect_data
            {
                natus::gpu::result_res_t res ;
                natus::gpu::variable_set_res_t vs ;
                natus::gpu::render_configuration_res_t config ;
            };
            typedef natus::std::vector< connect_data > connects_t ;
            connects_t _connects ;
            natus::concurrent::mutex_t _connects_mtx ;

            struct gconfig_data
            {
                natus::gpu::result_res_t res ;
                natus::gpu::geometry_configuration_res_t config ;
            };
            typedef natus::std::vector< gconfig_data > gconfigs_t ;
            gconfigs_t _gconfigs ;

            natus::concurrent::mutex_t _gconfigs_mtx ;

            struct gupdate_data
            {
                natus::gpu::result_res_t res ;
                natus::gpu::geometry_configuration_res_t config ;
            };
            typedef natus::std::vector< gupdate_data > gupdates_t ;
            gupdates_t _gupdates ;

            natus::concurrent::mutex_t _gupdates_mtx ;
            
            struct render_data
            {
                natus::gpu::result_res_t res ;
                natus::gpu::render_configuration_res_t config ;
                natus::gpu::backend::render_detail_t detail ;
            };
            typedef natus::std::vector< render_data > renders_t ;
            renders_t _renders ;
            natus::concurrent::mutex_t _renders_mtx ;


        private: // sync

            natus::concurrent::mutex_t _frame_mtx ;
            natus::concurrent::condition_variable_t _frame_cv ;
            size_t _num_enter = 0 ;

        public:

            async( void_t ) ;
            async( backend_res_t ) ;
            async( this_cref_t ) = delete ;
            async( this_rref_t ) ;
            ~async( void_t ) ;

        public:

            natus::gpu::result set_window_info( natus::gpu::backend_t::window_info_cref_t ) noexcept ;

            this_ref_t configure( natus::gpu::geometry_configuration_res_t, natus::gpu::result_res_t = natus::gpu::result_res_t() ) noexcept ;

            this_ref_t configure( natus::gpu::render_configuration_res_t, natus::gpu::result_res_t = natus::gpu::result_res_t() ) noexcept ;

            this_ref_t configure( natus::gpu::shader_configuration_res_t, natus::gpu::result_res_t = natus::gpu::result_res_t() ) noexcept ;

            this_ref_t connect( natus::gpu::render_configuration_res_t, natus::gpu::variable_set_res_t, natus::gpu::result_res_t = natus::gpu::result_res_t() ) noexcept ;
            this_ref_t update( natus::gpu::geometry_configuration_res_t, natus::gpu::result_res_t = natus::gpu::result_res_t() ) noexcept ;

            this_ref_t render( natus::gpu::render_configuration_res_t, natus::gpu::backend::render_detail_cref_t, 
                natus::gpu::result_res_t = natus::gpu::result_res_t() ) noexcept ;

        public:

            /// render thread update function - DO NOT USE.
            void_t system_update( void_t ) noexcept ;

        public: // sync

            bool_t enter_frame( void_t ) ;
            void_t leave_frame( void_t ) ;
            void_t wait_for_frame( void_t ) ;
        };
        natus_soil_typedef( async ) ;

        // restricts access to the async
        class async_view
        {
            natus_this_typedefs( async_view ) ;

        private:

            natus::gpu::async_res_t _async ;
            bool_ptr_t _access = nullptr ;

        public: 

            async_view( natus::gpu::async_res_t const& r, bool_ptr_t b ) :
                _async( r ), _access( b ) {}
            async_view( natus::gpu::async_res_t&& r, bool_ptr_t b ) :
                _async( ::std::move( r ) ), _access( b ) {}

        public:

            async_view( void_t ) {}
            async_view( this_cref_t rhv ) :
                _async( rhv._async ), _access( rhv._access ) {}

            async_view( this_rref_t rhv ) :
                _async( ::std::move( rhv._async ) ), _access( rhv._access ) {}

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

            this_ref_t configure( natus::gpu::geometry_configuration_res_t config,
                natus::gpu::result_res_t res = natus::gpu::result_res_t() ) noexcept
            {
                _async->configure( config ) ;
                return *this ;
            }

            this_ref_t configure( natus::gpu::render_configuration_res_t config, 
                natus::gpu::result_res_t res = natus::gpu::result_res_t() ) noexcept
            {
                _async->configure( config, res ) ;
                return *this ;
            }

            this_ref_t configure( natus::gpu::shader_configuration_res_t config,
                natus::gpu::result_res_t res = natus::gpu::result_res_t() ) noexcept
            {
                _async->configure( config, res ) ;
                return *this ;
            }

            this_ref_t connect( natus::gpu::render_configuration_res_t config,
                natus::gpu::variable_set_res_t vs,
                natus::gpu::result_res_t res = natus::gpu::result_res_t() ) noexcept
            {
                _async->connect( config, vs, res ) ;
                return *this ;
            }

            this_ref_t update( natus::gpu::geometry_configuration_res_t gs,
                natus::gpu::result_res_t res = natus::gpu::result_res_t() ) noexcept
            {
                _async->update( gs, res ) ;
                return *this ;
            }

            this_ref_t render( natus::gpu::render_configuration_res_t config, 
                natus::gpu::result_res_t res = natus::gpu::result_res_t() ) noexcept
            {
                natus::gpu::backend_t::render_detail_t detail ;
                _async->render( config, detail, res ) ;
                return *this ;
            }

            this_ref_t render( natus::gpu::render_configuration_res_t config, natus::gpu::backend::render_detail_cref_t detail, 
                natus::gpu::result_res_t res = natus::gpu::result_res_t() ) noexcept
            {
                _async->render( config, detail, res ) ;
                return *this ;
            }

        public:

        };
        natus_typedef( async_view ) ;
    }
}