
#pragma once

#include "typedefs.h"
#include "enums.hpp"
#include "symbol.hpp"

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

            struct code 
            {
                natus::nsl::api_type api ;
                natus::nsl::shader_type type ;
                natus::ntd::string_t shader ;
            };
            natus_typedef( code ) ;
            natus::ntd::vector< code > codes ;
        };
        natus_typedef( generated_code ) ;
    }
}