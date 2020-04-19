

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

            virtual natus::gpu::id_t prepare( natus::gpu::render_configurations_in_t ) noexcept override ;

            virtual natus::gpu::id_t prepare( id_rref_t id,
                natus::gpu::render_configurations_in_t ) noexcept override ;

            virtual natus::gpu::id_t render( id_rref_t id ) noexcept override ;

        };
        natus_soil_typedef( null_backend ) ;
    }
}