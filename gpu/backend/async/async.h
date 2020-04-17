
#pragma once

#include "../id.hpp"
#include "../backend.h"

#include "completion.hpp"

namespace natus
{
    namespace gpu
    {
        class NATUS_GPU_API async_backend
        {
            natus_this_typedefs( async_backend ) ;

        private:

            backend_rptr_t _backend ;

        public:

            virtual natus::gpu::result prepare( natus::gpu::render_configuration_in_t,
                natus::gpu::completion_rptr_t ) noexcept ;

            virtual natus::gpu::result prepare( natus::gpu::id_rref_t, natus::gpu::render_configuration_in_t, natus::gpu::completion_rptr_t ) noexcept ;

            virtual natus::gpu::result render( natus::gpu::id_rref_t, 
                natus::gpu::completion_rptr_t ) noexcept ;

        public:

            /// submit all objects
            //void_t submit( void_t ) ;
        };
        natus_typedef( async_backend ) ;
    }
}