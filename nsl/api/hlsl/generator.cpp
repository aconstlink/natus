

#include "generator.h"

#include <sstream>
#include <regex>

using namespace natus::nsl::hlsl ;

natus::ntd::string_t generator::replace_buildin_symbols( natus::ntd::string_t code ) noexcept
{
    natus::nsl::repl_syms_t repls =
    {
        {
            natus::ntd::string_t( ":cmul:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 2 ) return "mul ( INVALID_ARGS ) " ;
                return args[ 0 ] + " * " + args[ 1 ] ;
            }
        },
        {
            natus::ntd::string_t( ":mmul:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 2 ) return "mmul ( INVALID_ARGS ) " ;
                return "mul( " + args[ 1 ] + " , " + args[ 0 ] + " )" ;
            }
        },
        {
            natus::ntd::string_t( ":mul_asg:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 2 ) return "mul_asg ( INVALID_ARGS ) " ;
                return args[ 0 ] + " *= " + args[ 1 ] ;
            }
        },
        {
            natus::ntd::string_t( ":add:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() == 0 || args.size() > 2 ) return "add ( INVALID_ARGS ) " ;
                return args.size() == 1 ? " + " + args[ 0 ] : args[ 0 ] + " + " + args[ 1 ] ;
            }
        },
        {
            natus::ntd::string_t( ":sub:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() == 0 || args.size() > 2 ) return "sub ( INVALID_ARGS ) " ;
                return args.size() == 1 ? " - " + args[ 0 ] : args[ 0 ] + " - " + args[ 1 ] ;
            }
        },
        {
            natus::ntd::string_t( ":div:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 2 ) return "div ( INVALID_ARGS ) " ;
                return args[ 0 ] + " / " + args[ 1 ] ;
            }
        },
        {
            natus::ntd::string_t( ":pulse:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 3 ) return "pulse ( INVALID_ARGS ) " ;
                return  "( step ( " + args[ 0 ] + " , " + args[ 2 ] + " ) - " +
                    "step ( " + args[ 1 ] + " , " + args[ 2 ] + " ) )" ;
            }
        },
        {
            natus::ntd::string_t( ":texture:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 2 ) return "texture ( INVALID_ARGS ) " ;
                return  args[ 0 ] + ".Sample ( smp_" + args[ 0 ] + " , " + args[ 1 ] + " ) " ;
            }
        },
        {
            natus::ntd::string_t( ":rt_texture:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 2 ) return "texture ( INVALID_ARGS ) " ;
                return  args[ 0 ] + ".Sample ( smp_" + args[ 0 ] + " , "
                    "float2 ( " + args[ 1 ] + ".x , 1.0f - " + args[ 1 ] + ".y ) ) " ;
            }
        },
        {
            natus::ntd::string_t( ":ls:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 2 ) return " << ( INVALID_ARGS ) " ;
                return args[ 0 ] + " << " + args[ 1 ] ;
            }
        },
        {
            natus::ntd::string_t( ":rs:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 2 ) return ">> ( INVALID_ARGS ) " ;
                return args[ 0 ] + " >> " + args[ 1 ] ;
            }
        },
        {
            natus::ntd::string_t( ":lt:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 2 ) return "lt ( INVALID_ARGS ) " ;
                return args[ 0 ] + " < " + args[ 1 ] ;
            }
        },
        {
            natus::ntd::string_t( ":gt:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 2 ) return "gt ( INVALID_ARGS ) " ;
                return args[ 0 ] + " > " + args[ 1 ] ;
            }
        },
        {
            natus::ntd::string_t( ":ret:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 1 ) return "ret ( INVALID_ARGS ) " ;
                return "return " + args[ 0 ] ;
            }
        },
        {
            natus::ntd::string_t( ":mix:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 3 ) return "mix ( INVALID_ARGS ) " ;
                return "lerp (" + args[ 0 ] + " , " + args[ 1 ] + " , " + args[ 2 ] + " ) " ;
            }
        },
        {
            natus::ntd::string_t( ":inc:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 1 ) return "++ ( INVALID_ARGS ) " ;
                return "++ ( " + args[ 0 ] + " ) " ;
            }
        },
        {
            natus::ntd::string_t( ":dec:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 1 ) return "-- ( INVALID_ARGS ) " ;
                return "-- ( " + args[ 0 ] + " ) " ;
            }
        },
        {
            natus::ntd::string_t( ":inc_post:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 1 ) return "++ ( INVALID_ARGS ) " ;
                return "( " + args[ 0 ] + " ) ++ " ;
            }
        },
        {
            natus::ntd::string_t( ":dec_post:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 1 ) return "-- ( INVALID_ARGS ) " ;
                return "( " + args[ 0 ] + " ) -- " ;
            }
        },
        {
            natus::ntd::string_t( ":fract:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 1 ) return "frac ( INVALID_ARGS ) " ;
                return "frac ( " + args[ 0 ] + " ) " ;
            }
        },
        {
            natus::ntd::string_t( ":ceil:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 1 ) return "ceil ( INVALID_ARGS ) " ;
                return "ceil ( " + args[ 0 ] + " ) " ;
            }
        },
        {
            natus::ntd::string_t( ":floor:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 1 ) return "floor ( INVALID_ARGS ) " ;
                return "floor ( " + args[ 0 ] + " ) " ;
            }
        },
        {
            natus::ntd::string_t( ":dot:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 2 ) return "dot ( INVALID_ARGS ) " ;
                return "dot ( " + args[ 0 ] + " , " + args[ 1 ] + " ) " ;
            }
        },
        {
            natus::ntd::string_t( ":pow:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 2 ) return "pow ( INVALID_ARGS ) " ;
                return "pow ( " + args[ 0 ] + " , " + args[ 1 ] + " ) " ;
            }
        },
        {
            natus::ntd::string_t( ":make_array:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() < 3 ) return "make_array ( INVALID_ARGS ) " ;
                natus::ntd::string_t tmp ;
                for( size_t i=0; i<args.size()-3; ++i ) tmp += args[3+i] + " , " ;
                tmp = tmp.substr( 0, tmp.size() - 3 ) ;
                return args[0] + " " + args[1] + "[ " + args[2] + " ] = { " + tmp + " } " ;
            }
        },
        {
            natus::ntd::string_t( ":as_vec2:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 1 ) return "as_vec2 ( INVALID_ARGS ) " ;
                return "float2 ( " + args[ 0 ] + " , " + args[ 0 ] + " ) " ;
            }
        },
        {
            natus::ntd::string_t( ":as_vec3:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 1 ) return "as_vec3 ( INVALID_ARGS ) " ;
                return "float3 ( " + args[ 0 ] + " , " + args[ 0 ] + " , " + args[ 0 ] + " ) " ;
            }
        },
        {
            natus::ntd::string_t( ":as_vec4:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 1 ) return "as_vec4 ( INVALID_ARGS ) " ;
                return "float4 ( " + args[ 0 ] + " , " + args[ 0 ] + " , " + args[ 0 ] + " , " + args[ 0 ] + " ) " ;
            }
        },
        {
            natus::ntd::string_t( ":fetch_data:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 2 ) return "fetch_data ( INVALID_ARGS ) " ;
                return args[ 0 ] + ".Load( " + args[ 1 ] + " ) " ;
            }
        }
    } ;

    return natus::nsl::perform_repl( std::move( code ), repls ) ;
}

natus::ntd::string_t generator::map_variable_type( natus::nsl::type_cref_t type ) noexcept
{
    typedef std::pair< natus::nsl::type_t, natus::ntd::string_t > mapping_t ;
    static mapping_t const __mappings[] =
    {
        mapping_t( natus::nsl::type_t(), "unknown" ),
        mapping_t( natus::nsl::type_t::as_void(), "void" ),
        mapping_t( natus::nsl::type_t::as_int(), "int" ),
        mapping_t( natus::nsl::type_t::as_uint(), "uint" ),
        mapping_t( natus::nsl::type_t::as_float(), "float" ),
        mapping_t( natus::nsl::type_t::as_vec2(), "float2" ),
        mapping_t( natus::nsl::type_t::as_vec3(), "float3" ),
        mapping_t( natus::nsl::type_t::as_vec4(), "float4" ),
        mapping_t( natus::nsl::type_t::as_mat2(), "float2x2" ),
        mapping_t( natus::nsl::type_t::as_mat3(), "float3x3" ),
        mapping_t( natus::nsl::type_t::as_mat4(), "float4x4" ),
        mapping_t( natus::nsl::type_t::as_tex1d(), "Texture1D" ),
        mapping_t( natus::nsl::type_t::as_tex2d(), "Texture2D" ),
        mapping_t( natus::nsl::type_t::as_tex2d_array(), "Texture2DArray" ),
        mapping_t( natus::nsl::type_t::as_data_buffer(), "Buffer< float4 >" )

    } ;

    for( auto const& m : __mappings ) if( m.first == type ) return m.second ;

    return __mappings[ 0 ].second ;
}

natus::ntd::string_t generator::replace_types( natus::ntd::string_t code ) noexcept
{
    size_t p0 = 0 ;
    size_t p1 = code.find_first_of( ' ' ) ;
    while( p1 != std::string::npos )
    {
        auto const dist = p1 - p0 ;

        auto token = code.substr( p0, dist ) ;
        natus::nsl::type_t const t = natus::nsl::to_type( token ) ;
        if( t.base != natus::nsl::type_base::unknown )
        {
            code.replace( p0, dist, this_t::map_variable_type( t ) ) ;
        }
        p0 = p1 + 1 ;
        p1 = code.find_first_of( ' ', p0 ) ;
    }

    return std::move( code ) ;
}

natus::ntd::string_t generator::map_variable_binding( natus::nsl::shader_type const st,
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
        mapping_t( natus::nsl::binding::texcoord7, "TEXCOORD7" ),

        mapping_t( natus::nsl::binding::vertex_id, "SV_VertexID" ),
        mapping_t( natus::nsl::binding::primitive_id, "SV_PrimitiveID" ),
        mapping_t( natus::nsl::binding::instance_id, "SV_InstanceID" )

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

natus::nsl::generated_code_t::shaders_t generator::generate( natus::nsl::generatable_cref_t genable_, natus::nsl::variable_mappings_cref_t var_map_ ) noexcept
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

    // replace buildins
    {
        for( auto& s : genable.config.shaders )
        {
            for( auto& c : s.codes )
            {
                for( auto& l : c.lines )
                {
                    l = this_t::replace_buildin_symbols( std::move( l ) ) ;
                }
            }
        }

        for( auto& frg : genable.frags )
        {
            for( auto& f : frg.fragments )
            {
                //for( auto& l : c.lines )
                {
                    f = this_t::replace_buildin_symbols( std::move( f ) ) ;
                }
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
                natus::log::global_t::warning( "[hlsl generator] : unknown shader type" ) ;
                continue;
            }
            shd.type = s_type ;

            shd.codes.emplace_back( this_t::generate( genable, s, var_map, natus::nsl::api_type::d3d11, varying ) ) ;
        }

        ret.emplace_back( std::move( shd ) ) ;
    }

    return std::move( ret ) ;

}

natus::nsl::generated_code_t::code_t generator::generate( natus::nsl::generatable_cref_t genable, natus::nsl::post_parse::config_t::shader_cref_t s, natus::nsl::variable_mappings_cref_t var_mappings, natus::nsl::api_type const type, varying_inout_t varying_inout ) noexcept
{
    natus::nsl::generated_code_t::code code ;

    std::stringstream text ;

    // make prototypes declarations from function signatures
    // the prototype help with not having to sort funk definitions
    {
        text << "// Declarations // " << std::endl ;
        for( auto const& f : genable.frags )
        {
            text << this_t::map_variable_type( f.sig.return_type ) << " " ;
            text << f.sym_long.expand( "_" ) << " ( " ;
            for( auto const& a : f.sig.args )
            {
                text << this_t::map_variable_type( a.type )
                    << " " << a.name << ", " ;
            }
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
            // make signature
            {
                text << this_t::map_variable_type( f.sig.return_type ) << " " ;
                text << f.sym_long.expand( "_" ) << " ( " ;
                for( auto const& a : f.sig.args )
                {
                    text << this_t::map_variable_type( a.type ) + " " + a.name + ", "  ;
                }
                text.seekp( -2, std::ios_base::end ) ;
                text << " )" << std::endl ;
            }

            // make body
            {
                text << "{" << std::endl ;
                for( auto const& l : f.fragments )
                {
                    text << this_t::replace_types( l ) << std::endl ;
                }
                text << "}" << std::endl ;
            }
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
        for( auto const& v : s.variables )
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
            if( v.fq != natus::nsl::flow_qualifier::in &&
                v.fq != natus::nsl::flow_qualifier::local ) continue ;

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
        for( auto& c : cpy_codes )
        {
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
                    } else if( s.type == natus::nsl::shader_type::pixel_shader )
                    {
                        text << output_name << " PS( " << last_varying.name << " input )" << std::endl ;
                        text << "{" << std::endl ; ++iter ;
                        text << output_name << " output = (" << output_name << ")0 ; " << std::endl ;
                    }
                    curlies++ ;
                    in_main = true ;
                } else
                {
                    if( in_main && *iter == "{" ) curlies++ ;
                    else if( in_main && *iter == "}" ) curlies--  ;

                    if( in_main && curlies == 0 )
                    {
                        text << "return output ;" << std::endl ;
                        in_main = false ;
                    }
                    text << " " << *iter << std::endl ;
                }
            }
        }
    }

    auto shd = text.str() ;

    // variable dependencies
    {
        for( auto const& v : genable.vars )
        {
            size_t p0 = shd.find( v.sym_long.expand() ) ; 
                    
            while( p0 != std::string::npos ) 
            {
                size_t const p1 = shd.find_first_of( " ", p0 ) ;
                shd = shd.substr( 0, p0 ) + v.value + shd.substr( p1 ) ;
                p0 = shd.find( v.sym_long.expand(), p1 ) ;
            }
        }
    }

    // fragment dependencies
    {
        for( auto const& f : genable.frags )
        {
            for( auto const& d : f.deps )
            {
                size_t p0 = shd.find( d.expand() ) ; 
                    
                while( p0 != std::string::npos ) 
                {
                    size_t const p1 = shd.find_first_of( " ", p0 ) ;
                    shd = shd.substr( 0, p0 ) + d.expand( "_" ) + shd.substr( p1 ) ;
                    p0 = shd.find( d.expand(), p1 ) ;
                }
            }
        }
    }

    // shader dependencies
    {
        for( auto const& d : s.deps )
        {
            size_t p0 = shd.find( d.expand() ) ; 
                    
            while( p0 != std::string::npos ) 
            {
                size_t const p1 = shd.find_first_of( " ", p0 ) ;
                shd = shd.substr( 0, p0 ) + d.expand( "_" ) + shd.substr( p1 ) ;
                p0 = shd.find( d.expand(), p1 ) ;
            }
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
            } else if( v.fq == natus::nsl::flow_qualifier::out )
            {
                flow = "out." ;
                struct_name = "output." ;
            } else if( v.fq == natus::nsl::flow_qualifier::local )
            {
                flow = "" ;
                struct_name = "input." ;
            } 

            // replace in./out. with 
            {
                natus::ntd::string_t const what = flow + v.old_name ;
                size_t p0 = shd.find( what, off ) ;
                while( p0 != std::string::npos )
                {
                    auto const with = struct_name + v.new_name ;
                    shd.replace( p0, what.size(), with ) ;
                    p0 = shd.find( what, p0 + with.size() ) ;
                }
            }
        }
    }

    {
        shd = std::regex_replace( shd,
            std::regex( " __float__ \\( ([0-9]+) \\, ([0-9]+) \\) " ),
            " $1.$2 " ) ;
        shd = std::regex_replace( shd,
            std::regex( " __uint__ \\( ([0-9]+) \\) " ),
            " $1u " ) ;
        shd = std::regex_replace( shd,
            std::regex( " __int__ \\( ([0-9]+) \\) " ),
            " $1 " ) ;
    }

    {
        shd = this_t::replace_types( std::move( shd ) ) ;
    }

    code.shader = shd ;
    code.api = type ;

    return std::move( code ) ;
}