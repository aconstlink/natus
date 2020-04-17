
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

            virtual natus::gpu::id_t prepare( natus::gpu::render_configuration_in_t ) noexcept override ;

            virtual natus::gpu::id_t prepare( id_rref_t id, 
                natus::gpu::render_configuration_in_t ) noexcept override ;

            virtual natus::gpu::id_t render( id_rref_t id ) noexcept override ;

        private:

            //
        };
        natus_typedef( gl3_backend ) ;
    }
}