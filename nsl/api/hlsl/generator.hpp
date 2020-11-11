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


            public:

                generator( void_t ) noexcept {}
                generator( natus::nsl::generatable_rref_t gen ) noexcept {}
                generator( this_cref_t rhv ) noexcept {}
                generator( this_rref_t rhv ) noexcept {}
                ~generator( void_t ) {}

            public:

                static natus::ntd::string_t map_variable_type( natus::ntd::string_cref_t type ) noexcept
                {
                    typedef std::pair< natus::ntd::string_t, natus::ntd::string_t > mapping_t ;
                    static mapping_t const __mappings[] = 
                    { 
                        mapping_t("float", "float"), mapping_t("vec2", "float2"), mapping_t("vec3", "float3"),
                        mapping_t("vec4", "float4"), mapping_t("mat2", "float2x2"),mapping_t("mat3", "float3x3"),
                        mapping_t("mat4", "float4x4"), mapping_t("tex2d", "Texture2D")
                    } ;

                    for( auto const & m : __mappings )
                    {
                        if( m.first == type ) return m.second ;
                    }
                    return type ;
                }

                static natus::ntd::string_t map_variable_binding( natus::nsl::shader_type const st,
                    natus::nsl::flow_qualifier const fq, natus::ntd::string_cref_t binding ) noexcept
                {
                    typedef std::pair< natus::ntd::string_t, natus::ntd::string_t > mapping_t ;
                    static mapping_t const __mappings[] =
                    {
                        mapping_t( "normal", "NORMAL" ),

                        mapping_t( "color", "COLOR0" ),
                        mapping_t( "color0", "COLOR0" ),
                        mapping_t( "color1", "COLOR1" ),
                        mapping_t( "color2", "COLOR2" ),
                        mapping_t( "color3", "COLOR3" ),
                        mapping_t( "color4", "COLOR4" ),
                        mapping_t( "color5", "COLOR5" ),
                        mapping_t( "color6", "COLOR6" ),
                        mapping_t( "color7", "COLOR7" ),
                        mapping_t( "color8", "COLOR8" ),

                        mapping_t( "texcoord", "TEXCOORD0" ),
                        mapping_t( "texcoord0", "TEXCOORD0" ),
                        mapping_t( "texcoord1", "TEXCOORD1" ),
                        mapping_t( "texcoord2", "TEXCOORD2" ),
                        mapping_t( "texcoord3", "TEXCOORD3" ),
                        mapping_t( "texcoord4", "TEXCOORD4" ),
                        mapping_t( "texcoord5", "TEXCOORD5" ),
                        mapping_t( "texcoord6", "TEXCOORD6" ),
                        mapping_t( "texcoord7", "TEXCOORD7" ),
                        mapping_t( "texcoord8", "TEXCOORD8" )
                    } ;

                    static mapping_t const __mrt[] =
                    {
                        mapping_t( "color", "SV_TARGET0" ),
                        mapping_t( "color0", "SV_TARGET0" ),
                        mapping_t( "color1", "SV_TARGET1" ),
                        mapping_t( "color2", "SV_TARGET2" ),
                        mapping_t( "color3", "SV_TARGET3" ),
                    } ;

                    if( fq == natus::nsl::flow_qualifier::in && binding == "position" ) return "POSITION" ;
                    if( fq == natus::nsl::flow_qualifier::out && binding == "position" ) return "SV_POSITION" ;
                    
                    // check render targets first
                    {
                        size_t const pos = binding.find( "color" ) ;
                        if( fq == natus::nsl::flow_qualifier::out && st == natus::nsl::shader_type::pixel_shader && 
                            pos != std::string::npos )
                        {
                            for( auto const& m : __mappings )
                            {
                                if( m.first == binding ) return m.second ;
                            }
                        }
                    }

                    // do other bindings
                    {
                        for( auto const& m : __mappings )
                        {
                            if( m.first == binding ) return m.second ;
                        }
                    }

                    return "NO_BINDING_SPECIFIED" ;
                }

                natus::nsl::generated_code_t::shaders_t generate( natus::nsl::generatable_cref_t genable_, natus::nsl::variable_mappings_cref_t var_map_ ) noexcept
                {
                    natus::nsl::variable_mappings_t var_map = var_map_ ;
                    natus::nsl::generatable_t genable = genable_ ;

                    // start renaming internal variables
                    {
                        for( auto& var : var_map )
                        {
                            if( var.fq == natus::nsl::flow_qualifier::out &&
                                var.st == natus::nsl::shader_type::vertex_shader &&
                                var.binding == "position" )
                            {
                                var.new_name = "gl_Position" ;
                            }
                        }
                    }

                    natus::nsl::generated_code_t::shaders_t ret ;

                    for( auto const& s : genable.config.shaders )
                    {
                        natus::nsl::shader_type const s_type = s.type ;

                        natus::nsl::generated_code_t::shader_t shd ;

                        for( auto& var : var_map )
                        {
                            if( var.st != s_type ) continue ;

                            natus::nsl::generated_code_t::variable_t v ;
                            v.binding = var.binding ;
                            v.fq = var.fq ;
                            v.name = var.new_name ;
                            shd.variables.emplace_back( std::move( v ) ) ;
                        }

                        // generate the code
                        {
                            if( s_type == natus::nsl::shader_type::unknown )
                            {
                                natus::log::global_t::warning( "[glsl generator] : unknown shader type" ) ;
                                continue;
                            }
                            shd.type = s_type ;

                            shd.codes.emplace_back( this_t::generate( genable, s, var_map, natus::nsl::api_type::d3d11 ) ) ;
                        }

                        ret.emplace_back( std::move( shd ) ) ;
                    }

                    return std::move( ret ) ;

                }

                natus::nsl::generated_code_t::code_t generate( natus::nsl::generatable_cref_t genable, natus::nsl::post_parse::config_t::shader_cref_t s, natus::nsl::variable_mappings_cref_t var_mappings, natus::nsl::api_type const type ) noexcept
                {
                    natus::nsl::generated_code_t::code code ;

                    std::stringstream text ;

                    // cbuffers
                    {
                        text << "cbuffer ConstantBuffer : register( b0 ) " << std::endl ;
                        text << "{" << std::endl ;
                        for( auto const & v : s.variables )
                        {
                            if( v.fq != natus::nsl::flow_qualifier::global ) continue ;
                            text << this_t::map_variable_type( v.type ) << " " << v.name << " ;" << std::endl ;
                        }
                        text << "}" << std::endl << std::endl ;
                    }

                    // inputs
                    if( s.type == natus::nsl::shader_type::vertex_shader )
                    {
                        text << "struct VS_INPUT" << std::endl ;
                        text << "{" << std::endl ;
                        for( auto const& v : s.variables )
                        {
                            if( v.fq != natus::nsl::flow_qualifier::in ) continue ;
                            
                            natus::ntd::string_t const type_ = this_t::map_variable_type( v.type ) ;
                            natus::ntd::string_t const binding_ = this_t::map_variable_binding( s.type, v.fq, v.binding ) ;
                            text << type_ << " " << v.name << " : " << binding_ << " ;" << std::endl ;
                        }
                        text << "}" << std::endl ;
                    }

                    // varying stage-to-stage variables
                    {
                        text << "struct VARYING" << std::endl ;
                        text << "{" << std::endl ;
                        for( auto const& v : s.variables )
                        {
                            if( !(s.type == natus::nsl::shader_type::vertex_shader &&
                                v.fq == natus::nsl::flow_qualifier::out) &&
                                !( s.type == natus::nsl::shader_type::pixel_shader &&
                                    v.fq == natus::nsl::flow_qualifier::in ) ) continue ;

                            natus::ntd::string_t const type_ = this_t::map_variable_type( v.type ) ;
                            natus::ntd::string_t const binding_ = this_t::map_variable_binding( s.type, v.fq, v.binding ) ;
                            text << type_ << " " << v.name << " : " << binding_ << " ;" << std::endl ;
                        }
                        text << "}" << std::endl ;
                    }


                    auto shd = text.str() ;
                    code.api = type ;

                    //ret.emplace_back( std::move( code ) ) ;
                    return std::move( code ) ;
                }
            };
            natus_typedef( generator ) ;
        }
    }
}