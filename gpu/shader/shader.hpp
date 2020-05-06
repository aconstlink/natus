
#pragma once

#include "variable_binding.h"

#include "../backend/id.hpp"

#include <natus/std/string.hpp>
#include <natus/std/vector.hpp>

#include <functional>

namespace natus
{
    namespace gpu
    {
        template< typename T >
        class shader
        {
            natus_this_typedefs( shader ) ;

        private:

            struct variable_binding
            {
                natus::std::string_t name ;
                natus::gpu::binding_point bp ;
            };
            natus_typedef( variable_binding ) ;
            natus::std::vector< variable_binding_t > _bindings ;

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

            T & operator = ( this_cref_t rhv ) noexcept 
            {
                _code = rhv._code ;
                _hash = rhv._hash ;
                _bindings = rhv._bindings ;
                return (T&)*this ;
            }

            T & operator = ( this_rref_t rhv ) noexcept 
            {
                _code = ::std::move( rhv._code ) ;
                _hash = rhv._hash ;
                rhv._hash = 0 ;
                _bindings = ::std::move( rhv._bindings ) ;
                return (T&)*this ;
            }

            natus::std::string code( void_t ) const { return _code ; }

        public:

            T & add_input_binding( natus::gpu::binding_point const bp, 
                natus::std::string_cref_t name )
            {
                variable_binding vb ;
                vb.name = name ;
                vb.bp = bp ;

                _bindings.push_back( vb ) ;

                return (T&)*this ;
            }
        };
    }
} 