#pragma once

#pragma once

#include "../../typedefs.h"
#include "../../parser_structs.hpp"
#include "../../generator_structs.hpp"

#include <natus/ntd/vector.hpp>
#include <natus/ntd/map.hpp>

#include <sstream>
#include <regex>

namespace natus
{
    namespace nsl
    {
        namespace hlsl
        {
            // the glsl backend code generator
            class generator
            {
                natus_this_typedefs( generator ) ;

            private:

                natus::nsl::generateable_t _genable ;

                // not vertex attributes, only varyings
                natus::ntd::map< natus::ntd::string_t, natus::ntd::string_t > _outs ;

                natus::ntd::map< natus::ntd::string_t, natus::ntd::string_t > _ps_ins ;
                natus::ntd::map< natus::ntd::string_t, natus::ntd::string_t > _ps_outs ;

            public:

                generator( void_t ) noexcept {}
                generator( natus::nsl::generateable_rref_t gen ) noexcept : _genable( std::move( gen ) ) {}
                generator( this_cref_t rhv ) noexcept : _genable( rhv._genable ) {}
                generator( this_rref_t rhv ) noexcept : _genable( std::move( rhv._genable ) ) {}
                ~generator( void_t ) {}

            public:

                natus::nsl::generated_code_t generate( void_t ) noexcept
                {
                    natus::nsl::generated_code_t ret ;
                    ret.rres = _genable ;

                    natus::nsl::generated_code_t::shader_t shd ;
                    {
                    }
                    _ps_ins.clear() ;
                    _ps_outs.clear() ;
                    _outs.clear() ;

                    return std::move( ret ) ;
                }

                natus::nsl::generated_code_t::code_t generate( natus::nsl::post_parse::config_t::shader_cref_t s, natus::nsl::api_type const type ) noexcept
                {
                    natus::nsl::generated_code_t::code code ;

                    //ret.emplace_back( std::move( code ) ) ;
                    return std::move( code ) ;

                }
            };
            natus_typedef( generator ) ;
        }
    }
}