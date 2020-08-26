
#pragma once

#include "../backend.h"

namespace natus
{
    namespace gpu
    {
        class NATUS_GPU_API es3_backend : public natus::gpu::backend
        {
            natus_this_typedefs( es3_backend ) ;

        private:

            struct pimpl ;
            pimpl* _pimpl ;

        public:

            es3_backend( void_t ) noexcept ;
            es3_backend( this_cref_t ) = delete ;
            es3_backend( this_rref_t ) noexcept ;
            virtual ~es3_backend( void_t ) ;

        public:

            virtual void_t set_window_info( window_info_cref_t ) noexcept ;

            virtual natus::gpu::result configure( natus::gpu::geometry_configuration_res_t ) noexcept override ;
            virtual natus::gpu::result configure( natus::gpu::render_configuration_res_t ) noexcept override ;
            virtual natus::gpu::result configure( natus::gpu::shader_configuration_res_t ) noexcept override ;
            virtual natus::gpu::result configure( natus::gpu::image_configuration_res_t ) noexcept override ;

            virtual natus::gpu::result connect( natus::gpu::render_configuration_res_t, natus::gpu::variable_set_res_t ) noexcept ;
            virtual natus::gpu::result update( natus::gpu::geometry_configuration_res_t ) noexcept ;

            virtual natus::gpu::result render( natus::gpu::render_configuration_res_t, natus::gpu::backend::render_detail_cref_t ) noexcept override ;

            virtual void_t render_begin( void_t ) noexcept ;
            virtual void_t render_end( void_t ) noexcept ;

        private:

            //
        };
        natus_res_typedef( es3_backend ) ;
    }
}