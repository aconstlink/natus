
#pragma once

#include "../backend/id.hpp"

#include <natus/std/string.hpp>

namespace natus
{
    namespace gpu
    {
        class NATUS_GPU_API geometry_shader
        {
            natus_this_typedefs( geometry_shader ) ;

        private:

            natus::std::string_t _code ;

        public:

            geometry_shader( void_t ) {}
            geometry_shader( natus::std::string_in_t code ) : _code( code ) {}
            geometry_shader( this_cref_t ) = delete ;
            geometry_shader( this_rref_t rhv ) {
                _code = ::std::move( rhv._code ) ;
            }
            virtual ~geometry_shader( void_t ) {}

        };
    }
}