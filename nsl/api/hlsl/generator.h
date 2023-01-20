
#pragma once

#pragma once

#include "../../typedefs.h"
#include "../../parser_structs.hpp"
#include "../../generator_structs.hpp"

#include <natus/ntd/vector.hpp>
#include <natus/ntd/map.hpp>

namespace natus
{
    namespace nsl
    {
        namespace hlsl
        {
            // the hlsl backend code generator
            class NATUS_NSL_API generator
            {
                natus_this_typedefs( generator ) ;

            public:

                generator( void_t ) noexcept {}
                generator( natus::nsl::generatable_rref_t gen ) noexcept {}
                generator( this_cref_t rhv ) noexcept {}
                generator( this_rref_t rhv ) noexcept {}
                ~generator( void_t ) {}

            public:
                
                static natus::ntd::string_t replace_buildin_symbols( natus::ntd::string_rref_t code ) noexcept ;

                static natus::ntd::string_t map_variable_type( natus::nsl::type_cref_t type ) noexcept ;

                static natus::ntd::string_t replace_types( natus::ntd::string_t code ) noexcept ;

                static natus::ntd::string_t map_variable_binding( natus::nsl::shader_type const st,
                    natus::nsl::flow_qualifier const fq, natus::nsl::binding binding ) noexcept ;

                natus::nsl::generated_code_t::shaders_t generate( natus::nsl::generatable_cref_t genable_, natus::nsl::variable_mappings_cref_t var_map_ ) noexcept ;

                natus::nsl::generated_code_t::code_t generate( natus::nsl::generatable_cref_t genable, 
                    natus::nsl::post_parse::config_t::shader_cref_t s, natus::nsl::variable_mappings_cref_t var_mappings, natus::nsl::api_type const type ) noexcept ;
            };
            natus_typedef( generator ) ;
        }
    }
}