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
        namespace glsl
        {
            // the glsl backend code generator
            class NATUS_NSL_API generator
            {
                natus_this_typedefs( generator ) ;

            private:

                static natus::ntd::string_t replace_buildin_symbols( natus::nsl::api_type const t, natus::ntd::string_t code ) noexcept ;                
                static natus::ntd::string_cref_t to_texture_type( natus::nsl::type_cref_t t ) noexcept ;
                static natus::ntd::string_t replace_types( natus::nsl::api_type const apit, natus::ntd::string_t code ) noexcept ;
                static natus::ntd::string_t determine_input_interface_block_name( natus::nsl::shader_type const cur, natus::nsl::shader_type const before ) noexcept ;
                static natus::ntd::string_t determine_output_interface_block_name( natus::nsl::shader_type const cur, natus::nsl::shader_type const after ) noexcept ;

            public:

                generator( void_t ) noexcept {}
                generator( this_cref_t rhv ) noexcept {}
                generator( this_rref_t rhv ) noexcept {}
                ~generator( void_t ) {}

            public:

                // 1. filter
                // 2. make variable names
                // 3. generate code
                natus::nsl::generated_code_t::shaders_t generate( natus::nsl::generatable_cref_t genable_, natus::nsl::variable_mappings_cref_t var_map_ ) noexcept ;

                natus::nsl::generated_code_t::code_t generate( natus::nsl::generatable_cref_t genable, natus::nsl::post_parse::config_t::shader_cref_t s, natus::nsl::variable_mappings_cref_t var_mappings, natus::nsl::api_type const type ) noexcept ;
            };
            natus_typedef( generator ) ;
        }
    }
}