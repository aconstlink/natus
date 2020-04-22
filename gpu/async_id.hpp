#pragma once

#include "api.h"
#include "protos.h"
#include "typedefs.h"
#include "result.h"

#include "backend/id.hpp"

namespace natus
{
    namespace gpu
    {
        /// 
        class NATUS_GPU_API async_id
        {
            natus_this_typedefs( async_id ) ;

            friend class async ;

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

        private: // async interface

            natus::gpu::id_t set( natus::gpu::id_rref_t id, natus::gpu::result res )
            {
                _id = ::std::move( id ) ;
                _res = res ;

                return natus::gpu::id_t() ;
            }

            natus::gpu::id_t id( void_t ) { return ::std::move( _id ) ; }
        };
        natus_soil_typedef( async_id ) ;
    }
}

