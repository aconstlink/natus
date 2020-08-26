
#pragma once

#include "variable_binding.h"

#include "../id.hpp"

#include <natus/ntd/string.hpp>
#include <natus/ntd/vector.hpp>

#include <functional>

namespace natus
{
    namespace graphics
    {
        class shader
        {
            natus_this_typedefs( shader ) ;

        private:

            natus::ntd::string_t _code ;

            size_t _hash = 0 ;

        public:

            shader( void_t ) 
            {}

            shader( natus::ntd::string_in_t code ) : _code( code ) 
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

            natus::ntd::string code( void_t ) const { return _code ; }
        };
        natus_typedef( shader ) ;
    }
} 