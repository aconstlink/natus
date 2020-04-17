
#pragma once

#include "../backend/id.hpp"

#include <natus/std/string.hpp>

namespace natus
{
    namespace gpu
    {
        class NATUS_GPU_API vertex_shader : public backend_id
        {
            natus_this_typedefs( vertex_shader ) ;

        private:

            natus::std::string_t _code ;

        public:

            vertex_shader( void_t ) {}
            vertex_shader( natus::std::string_in_t code ) : _code( code ) {}
            vertex_shader( this_cref_t ) = delete ;
            vertex_shader( this_rref_t rhv ) : backend_id( ::std::move( rhv) ) {
                _code = ::std::move( rhv._code ) ;
            }
            virtual ~vertex_shader( void_t ) {}

        };
        natus_typedef( vertex_shader ) ;
    }
}