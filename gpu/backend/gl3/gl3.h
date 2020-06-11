
#pragma once

#include "../backend.h"

namespace natus
{
    namespace gpu
    {
        class NATUS_GPU_API gl3_backend : public natus::gpu::backend
        {
            natus_this_typedefs( gl3_backend ) ;

        private:

            struct pimpl ;
            pimpl * _pimpl ;

        public:

            gl3_backend( void_t ) noexcept ;
            gl3_backend( this_cref_t ) = delete ;
            gl3_backend( this_rref_t ) noexcept ;
            virtual ~gl3_backend( void_t ) ;

        public:

            virtual void_t set_window_info( window_info_cref_t ) noexcept ;

            virtual natus::gpu::id_t configure( id_rref_t id, 
                natus::gpu::geometry_configuration_res_t ) noexcept override ;

            virtual natus::gpu::id_t configure( id_rref_t id, 
                natus::gpu::render_configuration_res_t ) noexcept override ;

            virtual id_t connect( id_rref_t id, natus::gpu::variable_set_res_t ) noexcept ;
            virtual id_t update( id_rref_t id, natus::gpu::geometry_configuration_res_t ) noexcept ;

            virtual natus::gpu::id_t render( id_rref_t id ) noexcept override ;

            virtual void_t render_begin( void_t ) noexcept ;
            virtual void_t render_end( void_t ) noexcept ;

        private:

            //
        };
        natus_soil_typedef( gl3_backend ) ;
    }
}