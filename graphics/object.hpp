
#pragma once

#include "api.h"
#include "protos.h"
#include "typedefs.h"

#include "id.hpp"

namespace natus
{
    namespace graphics
    {
        class NATUS_GRAPHICS_API object
        {
            natus_this_typedefs( object ) ;

        private:

            natus::graphics::id_res_t _id = natus::graphics::id_t() ;

        public:

            object( void_t ) 
            {
            }

            object( id_res_t const & id ) : _id( id ){}
            object( id_res_t && id ) noexcept : _id( std::move(id) ){}
            object( this_cref_t rhv ) noexcept : _id( rhv._id ) {}
            object( this_rref_t rhv ) noexcept : _id( std::move( rhv._id ) ){}
            ~object( void_t ) noexcept {}

            this_ref_t operator = ( this_cref_t rhv ) noexcept
            {
                _id = rhv._id ;
                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv ) noexcept
            {
                _id = std::move( rhv._id ) ;
                return *this ;
            }

        public:

            natus::graphics::id_res_t & get_id( void_t ) noexcept { return _id ; }
            natus::graphics::id_res_t const & get_id( void_t ) const noexcept { return _id ; }
            
        };
    }
}
