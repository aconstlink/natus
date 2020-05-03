
#pragma once

#include "../backend/id.hpp"

#include <natus/std/string.hpp>

namespace natus
{
    namespace gpu
    {
        class NATUS_GPU_API pixel_shader 
        {
            natus_this_typedefs( pixel_shader ) ;

        private:

            natus::std::string_t _code ;

            size_t _hash = 0 ;

        public:

            pixel_shader( void_t ) {}
            pixel_shader( natus::std::string_in_t code ) : _code( code ) 
            {
                _hash = ::std::hash<::std::string>{}( code ) ;
            }
            pixel_shader( this_cref_t rhv ) {
                *this = rhv ;
            }
            pixel_shader( this_rref_t rhv ) {
                *this = ::std::move( rhv ) ;
            }
            virtual ~pixel_shader( void_t ) {}

        public:

            this_ref_t operator = ( this_cref_t rhv )
            {
                _code = rhv._code ;
                _hash = rhv._hash ;
                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv )
            {
                _code = ::std::move( rhv._code ) ;
                _hash = rhv._hash ;
                rhv._hash = 0 ;
                return *this ;
            }

            natus::std::string code( void_t ) const { return _code ; }
        };
        natus_soil_typedef( pixel_shader ) ;
    }
}