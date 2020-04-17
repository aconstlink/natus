
#pragma once

#include "../backend/id.hpp"

#include <natus/std/string.hpp>

namespace natus
{
    namespace gpu
    {
        class NATUS_GPU_API pixel_shader : public backend_id
        {
            natus_this_typedefs( pixel_shader ) ;

        private:

            natus::std::string_t _code ;

        public:

            pixel_shader( void_t ) {}
            pixel_shader( natus::std::string_in_t code ) : _code( code ) {}
            pixel_shader( this_cref_t ) = delete ;
            pixel_shader( this_rref_t rhv ) : backend_id( ::std::move( rhv ) ) {
                _code = ::std::move( rhv._code ) ;
            }
            virtual ~pixel_shader( void_t ) {}
        };
    }
}