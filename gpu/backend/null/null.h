

#pragma once 

#include "../backend.h"

namespace natus
{
    namespace gpu
    {
        class NATUS_GPU_API null_backend : public natus::gpu::backend
        {
            natus_this_typedefs( null_backend ) ;

        public:

            null_backend( void_t ) ;
            null_backend( this_cref_t ) = delete ;
            null_backend( this_rref_t ) ;
            virtual ~null_backend( void_t ) ;

        public:

            virtual void_t set_window_info( window_info_cref_t ) noexcept ;

            virtual natus::gpu::id_t configure( id_rref_t id, natus::gpu::geometry_configuration_res_t ) noexcept override ;

            virtual natus::gpu::result configure( natus::gpu::render_configuration_res_t ) noexcept override ;

            virtual id_t configure( id_rref_t id, natus::gpu::shader_configuration_res_t ) noexcept override ;

            virtual id_t connect( id_rref_t id, natus::gpu::variable_set_res_t ) noexcept ;
            virtual id_t update( id_rref_t id, natus::gpu::geometry_configuration_res_t ) noexcept ;

            virtual natus::gpu::result render( natus::gpu::render_configuration_res_t, natus::gpu::backend::render_detail_cref_t ) noexcept override ;

            virtual void_t render_begin( void_t ) noexcept ;
            virtual void_t render_end( void_t ) noexcept ;
        };
        natus_soil_typedef( null_backend ) ;
    }
}