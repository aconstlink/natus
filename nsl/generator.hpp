
#pragma once

#include "api/glsl/generator.hpp"
//#include "api/hlsl.hpp"

#include <natus/ntd/vector.hpp>

namespace natus
{
    namespace nsl
    {
        // generator using api specific generators
        // to generate the source code
        class generator
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

            natus::nsl::generated_code_t generate( void_t ) noexcept
            {
                natus::nsl::generated_code_t ret ;

                natus::nsl::variable_mappings_t mappings ;
                
                natus::nsl::generated_code_t::shader_t shd ;
                {
                    for( auto const& s : _genable.config.shaders )
                    {
                        natus::nsl::shader_type const s_type = s.type ;

                        if( s_type == natus::nsl::shader_type::unknown )
                        {
                            natus::log::global_t::warning( "[generator] : unknown shader type" ) ;
                            continue;
                        }

                        for( auto const& v : s.variables )
                        {
                            natus::nsl::variable_mapping_t vm ;
                            vm.st = s_type ;

                            // everything is var first
                            natus::ntd::string_t flow = "var" ;

                            if( v.fq == natus::nsl::flow_qualifier::global )
                            {
                                flow = "u" ;
                            }
                            else if( s.type == natus::nsl::shader_type::vertex_shader && 
                                v.fq == natus::nsl::flow_qualifier::in )
                            {
                                flow = "in" ;
                            }
                            else if( s.type == natus::nsl::shader_type::pixel_shader &&
                                v.fq == natus::nsl::flow_qualifier::out )
                            {
                                flow = "out" ;
                            }
                            vm.new_name = flow + "_" + v.name ;
                            vm.old_name = v.name ;
                            vm.binding = v.binding ;
                            vm.fq = v.fq ;
                            mappings.emplace_back( std::move( vm ) ) ;
                        }
                    }
                }

                // glsl 
                {
                    ret.shaders = natus::nsl::glsl::generator_t().generate( _genable, mappings ) ;
                }

                // hlsl
                {
                }

                return std::move( ret ) ;
            }
        };
        natus_typedef( generator ) ;
    }
}