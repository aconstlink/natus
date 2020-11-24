
#pragma once

#include "api/glsl/generator.h"
#include "api/hlsl/generator.h"

#include <natus/ntd/vector.hpp>

namespace natus
{
    namespace nsl
    {
        // generator using api specific generators
        // to generate the source code
        class NATUS_NSL_API generator
        {
            natus_this_typedefs( generator ) ;

        private:

            natus::nsl::generatable_t _genable ;

        public:

            generator( void_t ) noexcept {}
            generator( natus::nsl::generatable_rref_t gen ) noexcept : _genable( std::move( gen ) ) {}
            generator( this_cref_t rhv ) noexcept : _genable( rhv._genable ) {}
            generator( this_rref_t rhv ) noexcept : _genable( std::move( rhv._genable ) ) {}
            ~generator( void_t ) {}

        public: 

            natus::nsl::generated_code_t generate( void_t ) noexcept ;
        };
        natus_typedef( generator ) ;
    }
}