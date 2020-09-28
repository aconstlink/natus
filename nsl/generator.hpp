
#pragma once

#include "typedefs.h"
#include "enums.hpp"
#include "symbol.hpp"
#include "parser_structs.hpp"

#include <natus/ntd/vector.hpp>

namespace natus
{
    namespace nsl
    {
        struct generateable
        {
            natus::nsl::symbols_t missing ;
            natus::nsl::post_parse::library_t::variables_t vars ;
            natus::nsl::post_parse::library_t::fragments_t frags ;
            natus::nsl::post_parse::config_t config ;
        };
        natus_typedef( generateable ) ;

        struct generated_code
        {
            // for possible later observation like bindings
            natus::nsl::generateable_t rres ;

            struct variable
            {
                natus::ntd::string_t name ;
                natus::ntd::string_t binding ;
            };
            natus_typedef( variable ) ;

            struct code
            {
                natus::nsl::api_type api ;
                natus::ntd::string_t shader ;
            };
            natus_typedef( code ) ;

            struct shader
            {
                natus::nsl::shader_type type ;
                natus::ntd::vector< variable > variables ;
                natus::ntd::vector< code > codes ;
            };
            natus_typedef( shader ) ;
            natus::ntd::vector< shader_t > shaders ;

            typedef std::function< void_t ( natus::nsl::shader_type, code_cref_t ) > for_each_code_t ;
            void_t sorted_by_api_type( natus::nsl::api_type const t,  for_each_code_t funk ) const noexcept
            {
                for( auto const & s : shaders )
                {
                    for( auto const& c : s.codes )
                    {
                        if( c.api == t ) funk( s.type, c ) ;
                    }
                }
                
            }
        };
        natus_typedef( generated_code ) ;
    }
}