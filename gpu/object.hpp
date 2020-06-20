
#pragma once

#include "api.h"
#include "typedefs.h"

#include "id.hpp"

namespace natus
{
    namespace gpu
    {
        class NATUS_GPU_API object
        {
            natus_this_typedefs( object ) ;

        private:

            natus::gpu::id_res_t _id = natus::gpu::id_t() ;

        public:

            object( void_t ) 
            {
            }

            object( id_res_t const & id ) : _id( id ){}
            object( id_res_t && id ) : _id( ::std::move(_id) ){}
            object( this_cref_t rhv ) : _id( rhv._id ) {}
            object( this_rref_t rhv ) : _id( ::std::move( rhv._id ) ){}
            ~object( void_t ) {}

        public:

            natus::gpu::id_res_t & get_id( void_t ) noexcept { return _id ; }
            natus::gpu::id_res_t const & get_id( void_t ) const noexcept { return _id ; }
            
        };
    }
}