#pragma once

#include "api.h"
#include "protos.h"
#include "typedefs.h"
#include "result.h"

#include "backend/id.hpp"

#include <natus/concurrent/typedefs.h>

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
            natus::gpu::async_result _res = natus::gpu::async_result::invalid ;
            natus::concurrent::mutex_t _mtx ;

        public:

            async_id( void_t ) noexcept {}
            async_id( this_cref_t ) = delete ;
            async_id( this_rref_t rhv ) noexcept
            {
                _id = ::std::move( rhv._id ) ;
                _res = ::std::move( rhv._res ) ;
            }
            ~async_id( void_t ) noexcept {}

        private: // async interface

            natus::gpu::id_t set( natus::gpu::id_rref_t id, natus::gpu::async_result res )
            {
                {
                    natus::concurrent::lock_guard_t lk( _mtx ) ;
                    _res = res ;
                }

                _id = ::std::move( id ) ;

                return natus::gpu::id_t() ;
            }

            natus::gpu::id_t id( void_t ) { return ::std::move( _id ) ; }

            natus::gpu::async_result swap( natus::gpu::async_result const res )
            {
                natus::concurrent::lock_guard_t lk( _mtx ) ;
                auto const old = _res ;
                _res = res ;
                return old ;

            }
        };
        natus_soil_typedef( async_id ) ;
    }
}

