
#pragma once

#include "variable_binding.h"

#include "../id.hpp"

#include <natus/std/string.hpp>
#include <natus/std/vector.hpp>

#include <functional>

namespace natus
{
    namespace gpu
    {
        class shader
        {
            natus_this_typedefs( shader ) ;

        private:

            natus::std::string_t _code ;

            size_t _hash = 0 ;

        public:

            shader( void_t ) 
            {}

            shader( natus::std::string_in_t code ) : _code( code ) 
            {
                _hash = ::std::hash<::std::string>{}( code ) ;
            }

            shader( this_cref_t rhv ) 
            {
                *this = rhv ;
            }

            shader( this_rref_t rhv ) 
            {
                *this = ::std::move( rhv ) ;
            }

            virtual ~shader( void_t ) 
            {}

            this_ref_t operator = ( this_cref_t rhv ) noexcept 
            {
                _code = rhv._code ;
                _hash = rhv._hash ;
                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv ) noexcept 
            {
                _code = ::std::move( rhv._code ) ;
                _hash = rhv._hash ;
                rhv._hash = 0 ;                
                return *this ;
            }

            natus::std::string code( void_t ) const { return _code ; }
        };
        natus_typedef( shader ) ;
    }
} 