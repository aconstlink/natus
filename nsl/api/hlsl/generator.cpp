

#include "generator.h"

#include <sstream>
#include <regex>

using namespace natus::nsl::hlsl ;

namespace this_file
{
    natus::core::types::bool_t fragment_by_opcode( natus::nsl::buildin_type bit, natus::nsl::post_parse::library_t::fragment_out_t ret ) noexcept
    {
        #if 0
        // texture offset is available in hlsl but kept this as an example.
        if( bit == natus::nsl::buildin_type::texture_offset )
        {
            natus::nsl::signature_t::arg_t a { natus::nsl::type_t::as_tex2d(), "tex" } ;
            natus::nsl::signature_t::arg_t b { natus::nsl::type_t::as_vec2(natus::nsl::type_base::tfloat), "uv" } ;
            natus::nsl::signature_t::arg_t c { natus::nsl::type_t::as_vec2(natus::nsl::type_base::tint), "off" } ;

            natus::nsl::signature_t sig = natus::nsl::signature_t
            { 
                natus::nsl::type_t::as_vec4(), "__d3d11_texture_offset__", { a, b, c } 
            } ;

            natus::ntd::vector< natus::ntd::string_t > lines 
            {                 
                "return :texture_( tex, uv + uvr ) ;"
            } ;

            natus::nsl::post_parse::library_t::fragment_t frg ;
            frg.sym_long = natus::nsl::symbol_t("__d3d11_texture_offset__") ;
            frg.sig = std::move( sig ) ;
            frg.fragments = std::move( lines ) ;
            frg.buildins.emplace_back( natus::nsl::post_parse::used_buildin{ 0, 0, 
                natus::nsl::get_build_in( natus::nsl::buildin_type::texture_dims ) }) ;

            ret = std::move( frg ) ;

            return true ;
        }
        else 
        #endif

        if( bit == natus::nsl::buildin_type::texture_dims )
        {
            natus::nsl::signature_t::arg_t a { natus::nsl::type_t::as_tex2d(), "tex" } ;
            natus::nsl::signature_t::arg_t b { natus::nsl::type_t::as_int(), "lod" } ;

            natus::nsl::signature_t sig = natus::nsl::signature_t
            { 
                natus::nsl::type_t::as_vec2(natus::nsl::type_base::tuint), "__d3d11_texture_dims__", { a, b } 
            } ;

            natus::ntd::vector< natus::ntd::string_t > lines 
            { 
                "uint width = 0 ; uint height = 0 ; int elements = 0 ; int depth = 0 ; int num_levels = 0 ; int num_samples = 0 ;",
                "tex.GetDimensions( lod, width, height, num_levels ) ;",
                "return uint2( width, height ) ;"
            } ;

            natus::nsl::post_parse::library_t::fragment_t frg ;
            frg.sym_long = natus::nsl::symbol_t("__d3d11_texture_dims__") ;
            frg.sig = std::move( sig ) ;
            frg.fragments = std::move( lines ) ;

            ret = std::move( frg ) ;

            return true ;
        }

        return false ;
    }

    //******************************************************************************************************************************************
    static natus::ntd::string_t determine_input_structure_name( natus::nsl::shader_type const cur, natus::nsl::shader_type const before ) noexcept 
    {
        // input assembler to vertex shader
        if( cur == natus::nsl::shader_type::vertex_shader )
            return "ia_to_" + natus::nsl::short_name( cur ) ;

        return natus::nsl::short_name( before ) + "_to_" + natus::nsl::short_name( cur ) ;
    }

    //******************************************************************************************************************************************
    static natus::ntd::string_t determine_output_structure_name( natus::nsl::shader_type const cur, natus::nsl::shader_type const after ) noexcept 
    {
        if( cur == natus::nsl::shader_type::pixel_shader )
            return "into_the_pixel_pot" ;
    
        if( cur != natus::nsl::shader_type::pixel_shader && 
            after == natus::nsl::shader_type::unknown )
            return "streamout" ; // must be streamout!!

        return natus::nsl::short_name( cur ) + "_to_" + natus::nsl::short_name( after ) ;
    }
}


natus::ntd::string_t generator::replace_buildin_symbols( natus::ntd::string_rref_t code ) noexcept
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
            natus::ntd::string_t( ":add_asg:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 2 ) return "add_asg ( INVALID_ARGS ) " ;
                return args[ 0 ] + " += " + args[ 1 ] ;
            }
        },
        {
            natus::ntd::string_t( ":sub_asg:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 2 ) return "sub_asg ( INVALID_ARGS ) " ;
                return args[ 0 ] + " -= " + args[ 1 ] ;
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
            natus::ntd::string_t( ":lor:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 2 ) return "lor ( INVALID_ARGS ) " ;
                return args[ 0 ] + " || " + args[ 1 ] ;
            }
        },
        {
            natus::ntd::string_t( ":land:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 2 ) return "land ( INVALID_ARGS ) " ;
                return args[ 0 ] + " && " + args[ 1 ] ;
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
            natus::ntd::string_t( ":plus:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 1 ) return "plus ( INVALID_ARGS ) " ;
                return " + " + args[ 0 ] ;
            }
        },
        {
            natus::ntd::string_t( ":minus:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 1 ) return "minus ( INVALID_ARGS ) " ;
                return " - " + args[ 0 ] ;
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
            natus::ntd::string_t( ":smoothstep:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 3 ) return "smoothstep ( INVALID_ARGS ) " ;
                return  "smoothstep ( " + args[ 0 ] + " , " + args[ 1 ] + " , " + args[ 2 ] + " )" ;
            }
        },
        {
            natus::ntd::string_t( ":smoothpulse:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 3 ) return "smoothpulse ( INVALID_ARGS ) " ;
                return  "( smoothstep ( " + args[ 0 ] + " , " + args[ 1 ] + " , " + args[ 4 ] + " ) - " +
                    "smoothstep ( " + args[ 2 ] + " , " + args[ 3 ] + " , " + args[ 4 ] + " ) )" ;
            }
        },
        {
            natus::ntd::string_t( ":clamp:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 3 ) return "clamp ( INVALID_ARGS ) " ;
                return  "clamp ( " + args[ 0 ] + " , " + args[ 1 ] + " , " + args[ 2 ] + " )" ;
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
            natus::ntd::string_t( ":rt_texcoords:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 1 ) return "rt_texcoords ( INVALID_ARGS ) " ;
                return  "float2 ( " + args[ 0 ] + ".x , 1.0f - " + args[ 0 ] + ".y ) " ;
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
            natus::ntd::string_t( ":rt_texture_offset:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 3 ) return "rt_texture_offset ( INVALID_ARGS ) " ;
                return  args[ 0 ] + ".Sample ( smp_" + args[ 0 ] + " , "
                    "float2 ( " + args[ 1 ] + ".x , 1.0f - " + args[ 1 ] + ".y ) , int2( 1, -1 ) * " + args[2] + " ) " ;
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
            natus::ntd::string_t( ":leq:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 2 ) return "leq ( INVALID_ARGS ) " ;
                return args[ 0 ] + " <= " + args[ 1 ] ;
            }
        },
        {
            natus::ntd::string_t( ":geq:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 2 ) return "geq ( INVALID_ARGS ) " ;
                return args[ 0 ] + " >= " + args[ 1 ] ;
            }
        },
        {
            natus::ntd::string_t( ":neq:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 2 ) return "neq ( INVALID_ARGS ) " ;
                return args[ 0 ] + " != " + args[ 1 ] ;
            }
        },
        {
            natus::ntd::string_t( ":eqeq:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 2 ) return "eqeq ( INVALID_ARGS ) " ;
                return args[ 0 ] + " == " + args[ 1 ] ;
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
            natus::ntd::string_t( ":lt_vec:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 2 ) return "lt_vec ( INVALID_ARGS ) " ;
                return args[ 0 ] + " < " + args[ 1 ] ;
            }
        },
        {
            natus::ntd::string_t( ":gt_vec:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 2 ) return "gt_vec ( INVALID_ARGS ) " ;
                return args[ 0 ] + " > " + args[ 1 ] ;
            }
        },
        {
            natus::ntd::string_t( ":lte_vec:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 2 ) return "lte_vec ( INVALID_ARGS ) " ;
                return args[ 0 ] + " <= " + args[ 1 ] ;
            }
        },
        {
            natus::ntd::string_t( ":gte_vec:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 2 ) return "gte_vec ( INVALID_ARGS ) " ;
                return args[ 0 ] + " >= " + args[ 1 ] ;
            }
        },
        {
            natus::ntd::string_t( ":eqeq_vec:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 2 ) return "eqeq_vec ( INVALID_ARGS ) " ;
                return args[ 0 ] + " == " + args[ 1 ] ;
            }
        },
        {
            natus::ntd::string_t( ":neq_vec:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 2 ) return "neq_vec ( INVALID_ARGS ) " ;
                return args[ 0 ] + " != " + args[ 1 ] ;
            }
        },
        {
            natus::ntd::string_t( ":any:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 1 ) return "any ( INVALID_ARGS ) " ;
                return "any( " + args[ 0 ] + " ) " ;
            }
        },
        {
            natus::ntd::string_t( ":all:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 1 ) return "all ( INVALID_ARGS ) " ;
                return "all( " + args[ 0 ] + " ) " ;
            }
        },
        {
            natus::ntd::string_t( ":ret:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 1 ) return "ret ( INVALID_ARGS ) " ;
                return args[0].empty() ? "return" : "return " + args[ 0 ] ;
            }
        },
        {
            natus::ntd::string_t( ":mix:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 3 ) return "mix ( INVALID_ARGS ) " ;
                return "lerp ( " + args[ 0 ] + " , " + args[ 1 ] + " , " + args[ 2 ] + " ) " ;
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
            natus::ntd::string_t( ":abs:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 1 ) return "abs ( INVALID_ARGS ) " ;
                return "abs ( " + args[ 0 ] + " ) " ;
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
            natus::ntd::string_t( ":min:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 2 ) return "min ( INVALID_ARGS ) " ;
                return "min ( " + args[ 0 ] + " , " + args[ 1 ] + " ) " ;
            }
        },
        {
            natus::ntd::string_t( ":max:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 2 ) return "max ( INVALID_ARGS ) " ;
                return "max ( " + args[ 0 ] + " , " + args[ 1 ] + " ) " ;
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
        },
        {
            natus::ntd::string_t( ":texture_offset:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 3 ) return "texture_offset ( INVALID_ARGS ) " ;
                return  args[ 0 ] + ".Sample ( smp_" + args[ 0 ] + " , " + args[ 1 ] + " , " + args[2] + " ) " ;
            }
        },
        {
            natus::ntd::string_t( ":texture_dims:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() == 1 ) return "__d3d11_texture_dims__( " + args[ 0 ] + " , 0 ) " ;
                if( args.size() == 2 ) return "__d3d11_texture_dims__( " + args[ 0 ] + " , " + args[ 1 ] + " ) " ;

                return "texture_dims ( INVALID_ARGS ) " ;
            }
        },
        {
            natus::ntd::string_t( ":emit_vertex:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 0 ) return "emit_vertex( INVALID_ARGS ) " ;
                return "prim_stream.Append( __output__ )"  ;
            }
        },
        {
            natus::ntd::string_t( ":end_primitive:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 0 ) return "end_primitive( INVALID_ARGS ) " ;
                return "prim_stream.RestartStrip() ; " ;
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
        mapping_t( natus::nsl::type_t::as_bool(), "bool" ),
        mapping_t( natus::nsl::type_t::as_int(), "int" ),
        mapping_t( natus::nsl::type_t::as_vec2(natus::nsl::type_base::tint), "int2" ),
        mapping_t( natus::nsl::type_t::as_vec3(natus::nsl::type_base::tint), "int3" ),
        mapping_t( natus::nsl::type_t::as_vec4(natus::nsl::type_base::tint), "int4" ),
        mapping_t( natus::nsl::type_t::as_uint(), "uint" ),
        mapping_t( natus::nsl::type_t::as_vec2(natus::nsl::type_base::tuint), "uint2" ),
        mapping_t( natus::nsl::type_t::as_vec3(natus::nsl::type_base::tuint), "uint3" ),
        mapping_t( natus::nsl::type_t::as_vec4(natus::nsl::type_base::tuint), "uint4" ),
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

    if( st == natus::nsl::shader_type::vertex_shader && 
        fq == natus::nsl::flow_qualifier::in && 
        binding == natus::nsl::binding::position )
        return "POSITION" ;
    else if( binding == natus::nsl::binding::position )
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

natus::nsl::generated_code_t::shaders_t generator::generate( natus::nsl::generatable_cref_t genable_, 
    natus::nsl::variable_mappings_cref_t var_map_ ) noexcept
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

    // exchange inout with in and out variables
    {
        auto iter = var_map.begin() ;
        while( iter != var_map.end() )
        {
            if( iter->fq == natus::nsl::flow_qualifier::inout )
            {
                natus::nsl::variable_mappings_t::value_type v = *iter ;
                {
                    v.fq = natus::nsl::flow_qualifier::in ;
                    *iter = v ;
                }
                {
                    v.fq = natus::nsl::flow_qualifier::out ;
                    iter = var_map.insert( iter, v ) + 1 ;
                }
            }
            ++iter ;
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

    // inject composite buildins
    {
        auto test_configs = [&]( natus::nsl::buildin_type const test_bi )
        {
            for( auto& s : genable.config.shaders )
            {
                for( auto& c : s.codes )
                {
                    for( auto& ubi : c.buildins)
                    {
                        if( test_bi == ubi.bi.t ) return true ;
                    }
                }
            }
            return false ;
        } ;

        auto test_fragments = [&]( natus::nsl::buildin_type const test_bi )
        {
            for( auto& frg : genable.frags )
            {
                for( auto const & ubi : frg.buildins )
                {
                    if( test_bi == ubi.bi.t ) return true ;
                }
            }
            return false ;
        } ;

        for( size_t i=0; i<size_t(natus::nsl::buildin_type::num_build_ins); ++i )
        {
            natus::nsl::buildin_type test_bi = natus::nsl::buildin_type(i) ;
            if( test_configs( test_bi ) || test_fragments( test_bi ) )
            {
                natus::nsl::post_parse::library_t::fragment_t new_frg ;
                if( this_file::fragment_by_opcode( test_bi, new_frg ) ) 
                {
                    genable.frags.emplace_back( std::move( new_frg ) ) ;
                }
                continue ;
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
                natus::log::global_t::warning( "[hlsl generator] : unknown shader type" ) ;
                continue;
            }
            shd.type = s_type ;

            shd.codes.emplace_back( this_t::generate( genable, s, var_map, natus::nsl::api_type::d3d11 ) ) ;
        }

        ret.emplace_back( std::move( shd ) ) ;
    }

    return std::move( ret ) ;

}

natus::nsl::generated_code_t::code_t generator::generate( natus::nsl::generatable_cref_t genable, natus::nsl::post_parse::config_t::shader_cref_t s, 
    natus::nsl::variable_mappings_cref_t var_mappings, natus::nsl::api_type const type ) noexcept
{
    natus::nsl::generated_code_t::code code ;

    std::stringstream text ;

    natus::nsl::shader_type_array_t shader_types ;

    // getting all involved shader types so interface 
    // blocks can be formed later on.
    {
        for( auto & i : shader_types ) i = natus::nsl::shader_type::unknown ;

        size_t i=0; 
        for( auto const & shd : genable.config.shaders ) shader_types[i++] = shd.type ;
        natus::nsl::sort_shader_type_array( shader_types ) ;
    }

    natus::nsl::shader_type const sht_cur = s.type ;
    natus::nsl::shader_type const sht_before = natus::nsl::shader_type_before( s.type, shader_types ) ;
    natus::nsl::shader_type const sht_after = natus::nsl::shader_type_after( s.type, shader_types ) ;

    natus::ntd::string_t const input_struct_name = this_file::determine_input_structure_name( sht_cur, sht_before ) ;
    natus::ntd::string_t const output_struct_name = this_file::determine_output_structure_name( sht_cur, sht_after ) ;

    text << "// config name: " << genable.config.name << std::endl << std::endl ;

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
    if( sht_cur == natus::nsl::shader_type::vertex_shader )
    {
        text << "struct " << input_struct_name << std::endl ;
        text << "{" << std::endl ;
        for( auto const& v : var_mappings )
        {
            if( v.st != natus::nsl::shader_type::vertex_shader ) continue ;
            if( v.fq != natus::nsl::flow_qualifier::in ) continue ;
            if( v.fq == natus::nsl::flow_qualifier::local ) continue ;
            
            natus::ntd::string_t name = v.new_name ;
            natus::ntd::string_t const type_ = this_t::map_variable_type( v.t ) ;
            natus::ntd::string_t const binding_ = this_t::map_variable_binding( s.type, v.fq, v.binding ) ;

            text << type_ << " " << name << " : " << binding_ << " ;" << std::endl ;
        }
        text << "} ;" << std::endl << std::endl ;
    }
    // inputs from previous outputs
    else 
    {
        text << "struct " << input_struct_name << std::endl ;
        text << "{" << std::endl ;
        for( auto const& v : var_mappings )
        {
            if( v.st != sht_before ) continue ;
            if( v.fq != natus::nsl::flow_qualifier::out ) continue ;
            if( v.fq == natus::nsl::flow_qualifier::local ) continue ;
            
            natus::ntd::string_t name = v.new_name ;
            natus::ntd::string_t const type_ = this_t::map_variable_type( v.t ) ;
            natus::ntd::string_t const binding_ = this_t::map_variable_binding( s.type, natus::nsl::flow_qualifier::in, v.binding ) ;

            text << type_ << " " << name << " : " << binding_ << " ;" << std::endl ;
        }
        text << "} ;" << std::endl << std::endl ;
    }

    // outputs
    {
        text << "struct " << output_struct_name << std::endl ;
        text << "{" << std::endl ;
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

            text << type_ << " " << name << " : " << binding_ << " ;" << std::endl ;
        }
        text << "} ;" << std::endl << std::endl ;
    }


    text << "// The shader // " << std::endl ;
    {
        natus::ntd::string_t funk_name = "UNKNOWN" ;
        if( s.type == natus::nsl::shader_type::vertex_shader ) funk_name = "VS" ;
        else if( s.type == natus::nsl::shader_type::geometry_shader ) funk_name = "GS" ;
        else if( s.type == natus::nsl::shader_type::pixel_shader ) funk_name = "PS" ;

        size_t num_in_verts = 0 ;
        size_t max_vertex_count = 0 ;
        natus::ntd::string_t input_prim_name ;
        natus::ntd::string_t output_stream_name ;

        for( auto const & pd : s.primitive_decls ) 
        {
            if( pd.fq == natus::nsl::flow_qualifier::out )
            {
                max_vertex_count = pd.max_vertices ;
                break ;
            }

            if( pd.fq == natus::nsl::flow_qualifier::in )
            {
                switch( pd.pdt )
                {
                case natus::nsl::primitive_decl_type::points: 
                    num_in_verts = 1 ; input_prim_name = "point" ; output_stream_name = "PointStream" ; break ;
                case natus::nsl::primitive_decl_type::lines: 
                    num_in_verts = 2 ; input_prim_name = "line" ; output_stream_name = "LineStream" ; break ;
                case natus::nsl::primitive_decl_type::triangles: 
                    num_in_verts = 3 ; input_prim_name = "triangle " ; output_stream_name = "TriangleStream" ; break ;
                default: break ;
                }
            }
        }

        natus::ntd::string_t locals ;

        // locals
        {
            for( auto const& v : s.variables )
            {
                if( v.fq != natus::nsl::flow_qualifier::local ) continue ;

                natus::ntd::string_t name = v.name ;
                natus::ntd::string_t const type_ = this_t::map_variable_type( v.type ) ;
                natus::ntd::string_t const binding_ = this_t::map_variable_binding( s.type, v.fq, v.binding ) ;

                // the new name will be replaced further down the road
                locals += ", " + type_ + " " + name + " : " + binding_ ;
            }
        }

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
                        text << output_struct_name << " " << funk_name << " ( " << input_struct_name << " __input__" << locals << " )" << std::endl ;
                        text << "{" << std::endl ; ++iter ;
                        text << output_struct_name << " __output__ = (" << output_struct_name << ")0 ; " << std::endl ;
                    } 
                    else if( s.type == natus::nsl::shader_type::geometry_shader )
                    {
                        text << "[ maxvertexcount( " << std::to_string( max_vertex_count ) << " ) ]" << std::endl ;
                        text << "void " << funk_name << " ( " 
                            << input_prim_name << input_struct_name << " __input__["<< std::to_string( num_in_verts ) <<"], " 
                            << "inout " << output_stream_name << "<" << output_struct_name << ">" << " prim_stream" << locals << " )" << std::endl ;
                        text << "{" << std::endl ; ++iter ;
                        text << output_struct_name << " __output__ = (" << output_struct_name << ")0 ; " << std::endl ;
                    }
                    else if( s.type == natus::nsl::shader_type::pixel_shader )
                    {
                        text << output_struct_name << " PS( " << input_struct_name << " __input__ )" << std::endl ;
                        text << "{" << std::endl ; ++iter ;
                        text << output_struct_name << " __output__ = (" << output_struct_name << ")0 ; " << std::endl ;
                    }
                    curlies++ ;
                    in_main = true ;
                } 
                // early exit
                else if( in_main && iter->find( "return ;" ) != std::string::npos )
                {
                    if( sht_cur != natus::nsl::shader_type::geometry_shader )
                        text << "return __output__ ;" << std::endl ;
                    else
                        text << "return ;" << std::endl ;
                }
                else
                {
                    if( in_main && *iter == "{" ) curlies++ ;
                    else if( in_main && *iter == "}" ) curlies--  ;

                    if( in_main && curlies == 0 && sht_cur != natus::nsl::shader_type::geometry_shader )
                    {
                        text << "return __output__ ;" << std::endl ;
                        in_main = false ;
                    }
                    text << " " << *iter << std::endl ;
                }

                
            }
        }
    }

    auto shd = text.str() ;

    if( s.type == natus::nsl::shader_type::geometry_shader )
    {
        size_t num_in_verts = 0 ;
        for( auto const & pd : s.primitive_decls ) 
        {
            if( pd.fq == natus::nsl::flow_qualifier::in )
            {
                switch( pd.pdt )
                {
                case natus::nsl::primitive_decl_type::points: 
                    num_in_verts = 1 ;  break ;
                case natus::nsl::primitive_decl_type::lines: 
                    num_in_verts = 2 ; break ;
                case natus::nsl::primitive_decl_type::triangles: 
                    num_in_verts = 3 ; break ;
                default: break ;
                }
            }
        }
        std::regex rex( "in.length *\\( *\\) *") ;
        shd = std::regex_replace( shd, rex, std::to_string(num_in_verts) ) ; 
    }

    // variable dependencies
    {
        for( auto const& v : genable.vars )
        {
            auto const find_what = v.sym_long.expand() + " " ;
            size_t p0 = shd.find( find_what ) ; 
                    
            while( p0 != std::string::npos ) 
            {
                size_t const p1 = shd.find_first_of( " ", p0 ) ;
                shd = shd.substr( 0, p0 ) + this_t::replace_buildin_symbols( natus::ntd::string_t(v.value) ) + shd.substr( p1 ) ;
                p0 = shd.find( find_what, p1 ) ;
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

        // sub-optimal, but since the usage of regexp, the sub string
        // need to be processed so not the whole file is processed.
        natus::ntd::string_t sub = shd.substr( off ) ;

        for( auto const& v : var_mappings )
        {
            if( v.st != s.type ) continue ;
            if( v.fq == natus::nsl::flow_qualifier::local ) continue ;

            natus::ntd::string_t flow ;
            natus::ntd::string_t struct_name ;

            if( v.fq == natus::nsl::flow_qualifier::in )
            {
                flow = "in" ;
                struct_name = "__input__" ;
            } else if( v.fq == natus::nsl::flow_qualifier::out )
            {
                flow = "out" ;
                struct_name = "__output__" ;
            } 

            std::regex rex( flow + " *(\\[ *[0-9]*[a-z]* *\\])? *\\." + v.old_name ) ;
            sub = std::regex_replace( sub, rex, " " + struct_name + "$1." + v.new_name  ) ; 
        }

        for( auto const& v : var_mappings )
        {
            if( v.st != s.type ) continue ;
            if( v.fq != natus::nsl::flow_qualifier::local ) continue ;

            std::regex rex(  v.old_name + "(.*)" ) ;
            sub = std::regex_replace( sub, rex, " " + v.new_name + "$1" ) ; 
        }

        shd = shd.substr( 0, off ) + sub ;
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