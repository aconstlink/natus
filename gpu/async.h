
#pragma once

#include "async_id.hpp"
#include "backend/backend.h"

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

            struct prepare_data
            {
                natus::gpu::async_id_res_t aid ;
                natus::gpu::render_configurations_t config ;
            };
            typedef natus::std::vector< prepare_data > prepares_t ;
            prepares_t _prepares ;
            
            struct render_data
            {
                natus::gpu::async_id_res_t aid ;
            };
            typedef natus::std::vector< render_data > renders_t ;
            renders_t _renders ;

        public:

            async( void_t ) ;
            async( backend_res_t ) ;
            async( this_cref_t ) = delete ;
            async( this_rref_t ) ;
            ~async( void_t ) ;

        public:

            virtual natus::gpu::result prepare( natus::gpu::async_id_res_t, natus::gpu::render_configurations_in_t ) noexcept ;

            virtual natus::gpu::result render( natus::gpu::async_id_res_t ) noexcept ;

        public:

            /// submit all objects
            //void_t submit( void_t ) ;

            void_t update( void_t ) noexcept ;
        };
        natus_soil_typedef( async ) ;
    }
}