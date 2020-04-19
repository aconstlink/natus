
#pragma once

#include "../id.hpp"
#include "../backend.h"

#include <natus/std/vector.hpp>

namespace natus
{
    namespace gpu
    {
        class NATUS_GPU_API async_backend
        {
            natus_this_typedefs( async_backend ) ;

        private:

            /// decorated backend.
            backend_rptr_t _backend ;

            struct prepare_data
            {
                natus::gpu::async_id_rptr_t aid ;
                natus::gpu::render_configuration_t config ;
            };
            typedef natus::std::vector< prepare_data > prepares_t ;
            prepares_t _prepares ;
            
            struct render_data
            {
                natus::gpu::async_id_rptr_t aid ;
            };
            typedef natus::std::vector< render_data > renders_t ;
            renders_t _renders ;

        public:

            async_backend( void_t ) ;
            async_backend( backend_rptr_t ) ;
            async_backend( this_cref_t ) = delete ;
            async_backend( this_rref_t ) ;
            ~async_backend( void_t ) ;

        public:

            virtual natus::gpu::result prepare( natus::gpu::async_id_rptr_t, natus::gpu::render_configuration_in_t ) noexcept ;

            virtual natus::gpu::result render( natus::gpu::async_id_rptr_t ) noexcept ;

        public:

            /// submit all objects
            //void_t submit( void_t ) ;

            void_t update( void_t ) noexcept ;
        };
        natus_soil_typedef( async_backend ) ;

        /// 
        class NATUS_GPU_API async_id
        {
            natus_this_typedefs( async_id ) ;

            friend class async_backend ;

        private: 

            natus::gpu::id_t _id ;
            natus::gpu::result _res ;

        public:

            async_id( void_t ) noexcept {}
            async_id( this_cref_t ) = delete ;
            async_id( this_rref_t rhv ) noexcept
            {
                _id = ::std::move( rhv._id ) ;
                _res = rhv._res ;
            }
            ~async_id( void_t ) noexcept {}

        private: // async_backend interface

            natus::gpu::id_t set( natus::gpu::id_rref_t id, natus::gpu::result res ) 
            {
                _id = ::std::move( id ) ;
                _res = res ;
                
                return natus::gpu::id_t() ;
            }

            natus::gpu::id_t id( void_t ) { return ::std::move( _id ) ;  }
        };
        natus_soil_typedef( async_id ) ;
    }
}