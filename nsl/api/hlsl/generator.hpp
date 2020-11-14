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
                
                struct varying
                {
                    natus::ntd::string_t name ;
                    natus::ntd::string_t code ;
                };
                natus_typedef( varying ) ;

                static natus::ntd::string_t map_variable_type( natus::nsl::type_cref_t type ) noexcept
                {
                    typedef std::pair< natus::nsl::type_t, natus::ntd::string_t > mapping_t ;
                    static mapping_t const __mappings[] =
                    {
                        mapping_t( natus::nsl::type_t(), "unknown" ),
                        mapping_t( natus::nsl::type_t::as_float(), "float" ),
                        mapping_t( natus::nsl::type_t::as_vec2(), "float2" ),
                        mapping_t( natus::nsl::type_t::as_vec3(), "float3" ),
                        mapping_t( natus::nsl::type_t::as_vec4(), "float4" ),
                        mapping_t( natus::nsl::type_t::as_mat2(), "float2x2" ),
                        mapping_t( natus::nsl::type_t::as_mat3(), "float3x3" ),
                        mapping_t( natus::nsl::type_t::as_mat4(), "float4x4" ),
                        mapping_t( natus::nsl::type_t::as_tex1d(), "Texture1D" ),
                        mapping_t( natus::nsl::type_t::as_tex2d(), "Texture2D" )
                    } ;

                    for( auto const & m : __mappings ) if( m.first == type ) return m.second ;
                    
                    return __mappings[0].second ;
                }

                static natus::ntd::string_t map_variable_binding( natus::nsl::shader_type const st,
                    natus::nsl::flow_qualifier const fq, natus::nsl::binding binding ) noexcept
                {
                    typedef std::pair< natus::nsl::binding, natus::ntd::string_t > mapping_t ;
                    static mapping_t const __mappings[] =
                    {
                        mapping_t( natus::nsl::binding::normal, "NORMAL" ),

                        mapping_t( natus::nsl::binding::color0, "COLOR0" ),
                        mapping_t( natus::nsl::binding::color1, "COLOR1" ),
                        mapping_t( natus::nsl::binding::color2, "COLOR2" ),
                        mapping_t( natus::nsl::binding::color3, "COLOR3" ),
                        mapping_t( natus::nsl::binding::color4, "COLOR4" ),
                        mapping_t( natus::nsl::binding::color5, "COLOR5" ),
                        mapping_t( natus::nsl::binding::color6, "COLOR6" ),
                        mapping_t( natus::nsl::binding::color7, "COLOR7" ),
                        
                        mapping_t( natus::nsl::binding::texcoord0, "TEXCOORD0" ),
                        mapping_t( natus::nsl::binding::texcoord1, "TEXCOORD1" ),
                        mapping_t( natus::nsl::binding::texcoord2, "TEXCOORD2" ),
                        mapping_t( natus::nsl::binding::texcoord3, "TEXCOORD3" ),
                        mapping_t( natus::nsl::binding::texcoord4, "TEXCOORD4" ),
                        mapping_t( natus::nsl::binding::texcoord5, "TEXCOORD5" ),
                        mapping_t( natus::nsl::binding::texcoord6, "TEXCOORD6" ),
                        mapping_t( natus::nsl::binding::texcoord7, "TEXCOORD7" )
                    } ;

                    static mapping_t const __mrt[] =
                    {
                        mapping_t( natus::nsl::binding::color0, "SV_TARGET0" ),
                        mapping_t( natus::nsl::binding::color1, "SV_TARGET1" ),
                        mapping_t( natus::nsl::binding::color2, "SV_TARGET2" ),
                        mapping_t( natus::nsl::binding::color3, "SV_TARGET3" ),
                        mapping_t( natus::nsl::binding::color4, "SV_TARGET4" )
                    } ;

                    if( fq == natus::nsl::flow_qualifier::in && binding == natus::nsl::binding::position ) 
                        return "POSITION" ;
                    if( fq == natus::nsl::flow_qualifier::out && binding == natus::nsl::binding::position ) 
                        return "SV_POSITION" ;
                    
                    // check render targets first
                    {
                        if( fq == natus::nsl::flow_qualifier::out && 
                            st == natus::nsl::shader_type::pixel_shader && 
                            natus::nsl::is_color( binding ) )
                        {
                            for( auto const& m : __mrt ) if( m.first == binding ) return m.second ;
                            
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
                                var.binding == natus::nsl::binding::position )
                            {
                            }
                        }
                    }

                    natus::nsl::generated_code_t::shaders_t ret ;

                    varying_t varying ;

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

                            shd.codes.emplace_back( this_t::generate( genable, s, var_map, natus::nsl::api_type::d3d11, varying ) ) ;
                        }

                        ret.emplace_back( std::move( shd ) ) ;
                    }

                    return std::move( ret ) ;

                }

                natus::nsl::generated_code_t::code_t generate( natus::nsl::generatable_cref_t genable, natus::nsl::post_parse::config_t::shader_cref_t s, natus::nsl::variable_mappings_cref_t var_mappings, natus::nsl::api_type const type, varying_inout_t varying_inout ) noexcept
                {
                    natus::nsl::generated_code_t::code code ;

                    std::stringstream text ;
                    
                    // make prototypes declarations from function signatures
                    // the prototype help with not having to sort funk definitions
                    {
                        text << "// Declarations // " << std::endl ;
                        for( auto const& f : genable.frags )
                        {
                            if( f.version != natus::nsl::language_class::hlsl ) continue ;

                            text << f.sig.return_type << " " ;
                            text << f.sym_long.expand( "_" ) << " ( " ;
                            size_t i = 0 ;
                            for( auto const& a : f.sig.args ) text << a + " _ARG_" + std::to_string(i++) + ", " ;
                            text.seekp( -2, std::ios_base::end ) ;
                            text << " ) ; " << std::endl ;
                        }
                        text << std::endl ;
                    }

                    // make all functions with replaced symbols
                    {
                        text << "// Definitions // " << std::endl ;
                        for( auto const& f : genable.frags )
                        {
                            if( f.version != natus::nsl::language_class::hlsl ) continue ;

                            // start by replacing the function names' symbol itself
                            {
                                auto const& frag = f.fragments[ 0 ] ;

                                auto const p0 = frag.find( f.sig.name ) ;
                                if( p0 == std::string::npos ) continue ;
                                auto const p1 = frag.find_first_of( ' ', p0 ) ;
                                text << frag.substr( 0, p0 ) + f.sym_long.expand( "_" ) + frag.substr( p1 ) ;
                            }

                            // then lets go over every symbol in the code
                            for( size_t i = 1; i < f.fragments.size(); ++i )
                            {
                                text << f.fragments[ i ] ;
                            }
                            text << std::endl ;
                        }
                        text << std::endl ;
                    }

                    // textures
                    {
                        text << "// Textures //" << std::endl ;

                        size_t tx_id = 0 ;
                        
                        for( auto const& v : s.variables )
                        {
                            if( v.fq != natus::nsl::flow_qualifier::global ) continue ;
                            if( !v.type.is_texture() ) continue ;

                            natus::ntd::string_t const type_ = this_t::map_variable_type( v.type ) ;
                            natus::ntd::string_t const register_tx_ =
                                "register(t" + std::to_string( tx_id ) + ") ;" ;
                            natus::ntd::string_t const register_sp_ =
                                "register(s" + std::to_string( tx_id ) + ") ;" ;

                            text << type_ << " " << v.name << " : " << register_tx_ << std::endl ;
                            text << "SamplerState smp_" << v.name << " : " << register_sp_ << std::endl ;
                            ++tx_id ;
                        }
                        text << std::endl ;
                    }

                    // cbuffers
                    {
                        text << "// Constant Buffers //" << std::endl ;

                        text << "cbuffer ConstantBuffer : register( b0 ) " << std::endl ;
                        text << "{" << std::endl ;
                        for( auto const & v : s.variables )
                        {
                            if( v.fq != natus::nsl::flow_qualifier::global ) continue ;
                            if( v.type.is_texture() ) continue ;
                            
                            text << this_t::map_variable_type( v.type ) << " " << v.name << " ;" << std::endl ;
                        }
                        text << "}" << std::endl << std::endl ;
                    }

                    
                    text << "// Inputs/Outputs //" << std::endl ;

                    // inputs
                    if( s.type == natus::nsl::shader_type::vertex_shader )
                    {
                        text << "struct VS_INPUT" << std::endl ;
                        text << "{" << std::endl ;
                        for( auto const& v : s.variables )
                        {
                            if( v.fq != natus::nsl::flow_qualifier::in ) continue ;

                            natus::ntd::string_t name = v.name ;
                            natus::ntd::string_t const type_ = this_t::map_variable_type( v.type ) ;
                            natus::ntd::string_t const binding_ = this_t::map_variable_binding( s.type, v.fq, v.binding ) ;

                            size_t const idx = natus::nsl::find_by( var_mappings, v.name, v.binding, v.fq, s.type ) ;
                            if( idx != size_t( -1 ) )
                            {
                                name = var_mappings[ idx ].new_name ;
                            }

                            text << type_ << " " << name << " : " << binding_ << " ;" << std::endl ;
                        }
                        text << "} ;" << std::endl << std::endl ;
                    }

                    if( !varying_inout.name.empty() )
                    {
                        text << varying_inout.code << std::endl ;
                    }

                    varying_t last_varying = varying_inout ;

                    natus::ntd::string_t output_name = "UNKNOWN_OUTPUT" ;
                    if( s.type == natus::nsl::shader_type::vertex_shader ) output_name = "VS_OUTPUT" ;
                    //else if( s.type == natus::nsl::shader_type::geometry_shader ) varying_pre = "GS_OUTPUT" ;
                    else if( s.type == natus::nsl::shader_type::pixel_shader ) output_name = "PS_OUTPUT" ;

                    // outputs
                    {
                        std::stringstream text2 ;

                        text2 << "struct " << output_name << std::endl ;
                        text2 << "{" << std::endl ;
                        for( auto const& v : s.variables )
                        {
                            if( v.fq != natus::nsl::flow_qualifier::out ) continue ;

                            natus::ntd::string_t name = v.name ;
                            natus::ntd::string_t const type_ = this_t::map_variable_type( v.type ) ;
                            natus::ntd::string_t const binding_ = this_t::map_variable_binding( s.type, v.fq, v.binding ) ;

                            size_t const idx = natus::nsl::find_by( var_mappings, v.name, v.binding, v.fq, s.type ) ;
                            if( idx != size_t( -1 ) )
                            {
                                name = var_mappings[ idx ].new_name ;
                            }

                            text2 << type_ << " " << name << " : " << binding_ << " ;" << std::endl ;
                        }
                        text2 << "} ;" << std::endl << std::endl ;

                        varying_inout.name = output_name ;
                        varying_inout.code = text2.str() ;
                        text << varying_inout.code << std::endl ;
                    }
                    

                    text << "// The shader // " << std::endl ;
                    {
                        natus::ntd::string_t funk_name = "UNKNOWN" ;
                        if( s.type == natus::nsl::shader_type::vertex_shader ) funk_name = "VS" ;
                        //else if( s.type == natus::nsl::shader_type::geometry_shader ) varying_pre = "GS" ;
                        else if( s.type == natus::nsl::shader_type::pixel_shader ) funk_name = "PS" ;

                        auto cpy_codes = s.codes ;
                        for( auto & c : cpy_codes )
                        {
                            if( c.version != natus::nsl::language_class::hlsl ) continue ;

                            size_t curlies = 0 ;
                            bool_t in_main = false ;
                            for( auto iter = c.lines.begin(); iter != c.lines.end(); ++iter )
                            {
                                if( iter->find( "void main" ) != std::string::npos )
                                {
                                    if( s.type == natus::nsl::shader_type::vertex_shader )
                                    {
                                        text << output_name << " " << funk_name << " ( VS_INPUT input )" << std::endl ;
                                        text << "{" << std::endl ; ++iter ;
                                        text << output_name << " output = (" << output_name << ")0 ; " << std::endl ;
                                    }
                                    else if( s.type == natus::nsl::shader_type::pixel_shader )
                                    {
                                        text << output_name << " PS( " << last_varying.name << " input )" << std::endl ;
                                        text << "{" << std::endl ; ++iter ;
                                        text << output_name << " output = (" << output_name << ")0 ; " << std::endl ;
                                    }
                                    curlies++ ;
                                    in_main = true ;
                                }
                                else
                                {
                                    if( in_main && *iter == "{" ) curlies++ ;
                                    else if( in_main && *iter == "}" ) curlies--  ;

                                    if( in_main && curlies == 0 )
                                    {
                                        text << "return output ;" << std::endl ;
                                        in_main = false ;
                                    }
                                    text << *iter << std::endl ;
                                }
                            }
                        }
                    }

                    auto shd = text.str() ;

                    // variable dependencies
                    {
                        for( auto const& v : genable.vars )
                        {
                            size_t const p0 = shd.find( v.sym_long.expand() ) ;
                            if( p0 == std::string::npos ) continue ;
                            size_t const p1 = shd.find_first_of( " ", p0 ) ;

                            shd = shd.substr( 0, p0 ) + v.value + shd.substr( p1 ) ;
                        }
                    }

                    // fragment dependencies
                    {
                        for( auto const& f : genable.frags )
                        {
                            for( auto const& d : f.deps )
                            {
                                size_t const p0 = shd.find( d.expand() ) ;
                                if( p0 == std::string::npos ) continue ;
                                size_t const p1 = shd.find_first_of( " ", p0 ) ;

                                shd = shd.substr( 0, p0 ) + d.expand( "_" ) + shd.substr( p1 ) ;
                            }
                        }
                    }

                    // shader dependencies
                    {
                        for( auto const& d : s.deps )
                        {
                            size_t const p0 = shd.find( d.expand() ) ;
                            if( p0 == std::string::npos ) continue ;
                            size_t const p1 = shd.find_first_of( " ", p0 ) ;

                            shd = shd.substr( 0, p0 ) + d.expand( "_" ) + shd.substr( p1 ) ;
                        }
                    }

                    // replace in code in/out/globals
                    {
                        size_t const off = shd.find( "// The shader" ) ;

                        for( auto const& v : var_mappings )
                        {
                            if( v.st != s.type ) continue ;

                            natus::ntd::string_t flow ;
                            natus::ntd::string_t struct_name ;

                            if( v.fq == natus::nsl::flow_qualifier::in ) 
                            {
                                flow = "in." ;
                                struct_name = "input." ;
                            }
                            else if( v.fq == natus::nsl::flow_qualifier::out ) 
                            {
                                flow = "out." ;
                                struct_name = "output." ;
                            }

                            // replace in./out. with 
                            {
                                natus::ntd::string_t const repl = flow + v.old_name ;
                                size_t p0 = shd.find( repl, off ) ;
                                while( p0 != std::string::npos )
                                {
                                    shd.replace( p0, repl.size(), struct_name + v.new_name ) ;
                                    p0 = shd.find( repl, p0 + 3 ) ;
                                }
                            }
                        }
                    }

                    code.shader = shd ;
                    code.api = type ;

                    return std::move( code ) ;
                }
            };
            natus_typedef( generator ) ;
        }
    }
}