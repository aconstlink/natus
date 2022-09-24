#include "generator.h"

#include <sstream>
#include <regex>

using namespace natus::nsl::glsl ;

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
                return args[ 0 ] + " * " + args[ 1 ] ;
            }
        },
        {
            natus::ntd::string_t( ":add:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() == 0 || args.size() > 2 ) return "add ( INVALID_ARGS ) " ;
                return args.size() == 1 ? "+ " + args[ 0 ] : args[ 0 ] + " + " + args[ 1 ] ;
            }
        },
        {
            natus::ntd::string_t( ":sub:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() == 0 || args.size() > 2 ) return "sub ( INVALID_ARGS ) " ;
                return args.size() == 1 ? "- " + args[ 0 ] : args[ 0 ] + " - " + args[ 1 ] ;
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
                return  "texture( " + args[ 0 ] + " , " + args[ 1 ] + " ) " ;
            }
        },
        {
            natus::ntd::string_t( ":rt_texture:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 2 ) return "rt_texture ( INVALID_ARGS ) " ;
                return  "texture( " + args[ 0 ] + " , " + args[ 1 ] + " ) " ;
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
                return "mix ( " + args[ 0 ] + " , " + args[ 1 ] + " , " + args[ 2 ] + " ) " ;
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
                if( args.size() != 1 ) return "fract ( INVALID_ARGS ) " ;
                return "fract ( " + args[ 0 ] + " ) " ;
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
            natus::ntd::string_t( ":as_vec2:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 1 ) return "as_vec2 ( INVALID_ARGS ) " ;
                return "vec2_t ( " + args[ 0 ] + " ) " ;
            }
        },
        {
            natus::ntd::string_t( ":as_vec3:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 1 ) return "as_vec3 ( INVALID_ARGS ) " ;
                return "vec3_t ( " + args[ 0 ] + " ) " ;
            }
        },
        {
            natus::ntd::string_t( ":as_vec4:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() != 1 ) return "as_vec4 ( INVALID_ARGS ) " ;
                return "vec4_t ( " + args[ 0 ] + " ) " ;
            }
        },
        {
            natus::ntd::string_t( ":make_array:" ),
            [=] ( natus::ntd::vector< natus::ntd::string_t > const& args ) -> natus::ntd::string_t
            {
                if( args.size() < 3 ) return "__make_array ( INVALID_ARGS ) " ;
                natus::ntd::string_t tmp ;
                for( size_t i=0; i<args.size()-3; ++i ) tmp += args[3+i] + " , " ;
                tmp = tmp.substr( 0, tmp.size() - 3 ) ;
                return args[0] + " " + args[1] + " [ " + args[2] + " ] = " + args[0] + " [ " + args[2] + " ] " + " ( " + tmp + " ) " ;
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
        mapping_t( natus::nsl::type_t::as_vec2(), "vec2" ),
        mapping_t( natus::nsl::type_t::as_vec3(), "vec3" ),
        mapping_t( natus::nsl::type_t::as_vec4(), "vec4" ),
        mapping_t( natus::nsl::type_t::as_mat2(), "mat2" ),
        mapping_t( natus::nsl::type_t::as_mat3(), "mat3" ),
        mapping_t( natus::nsl::type_t::as_mat4(), "mat4" ),
        mapping_t( natus::nsl::type_t::as_tex1d(), "sampler1D" ),
        mapping_t( natus::nsl::type_t::as_tex2d(), "sampler2D" ),
        mapping_t( natus::nsl::type_t::as_tex2d_array(), "sampler2DArray" )
    } ;

    for( auto const& m : __mappings ) if( m.first == type ) return m.second ;

    return __mappings[ 0 ].second ;
}

natus::ntd::string_cref_t generator::to_texture_type( natus::nsl::type_cref_t t ) noexcept
{
    typedef std::pair< natus::nsl::type_ext, natus::ntd::string_t > __mapping_t ;

    static __mapping_t const __mappings[] = {
        __mapping_t( natus::nsl::type_ext::unknown, "unknown" ),
        __mapping_t( natus::nsl::type_ext::texture_1d, "sampler1D" ),
        __mapping_t( natus::nsl::type_ext::texture_2d, "sampler2D" )
    } ;

    for( auto const& m : __mappings ) if( m.first == t.ext ) return m.second ;
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
                var.new_name = "gl_Position" ;
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

            shd.codes.emplace_back( this_t::generate( genable, s, var_map, natus::nsl::api_type::es3 ) ) ;
            shd.codes.emplace_back( this_t::generate( genable, s, var_map, natus::nsl::api_type::gl3 ) ) ;
        }

        ret.emplace_back( std::move( shd ) ) ;
    }

    return std::move( ret ) ;
}

natus::nsl::generated_code_t::code_t generator::generate( natus::nsl::generatable_cref_t genable, natus::nsl::post_parse::config_t::shader_cref_t s, natus::nsl::variable_mappings_cref_t var_mappings, natus::nsl::api_type const type ) noexcept
{
    natus::nsl::generated_code_t::code code ;

    std::stringstream text ;

    // 1. glsl stuff at the front
    {
        switch( type )
        {
        case natus::nsl::api_type::gl3:
            text << "#version 130" << std::endl << std::endl ;
            break ;
        case natus::nsl::api_type::es3:
            text << "#version 300 es" << std::endl ;
            text << "precision mediump float ;" << std::endl << std::endl ;
            break ;
        default:
            text << "#version " << "glsl_type case missing" << std::endl << std::endl ;
            break ;
        }
    }

    // add extensions for pixel shader
    if( s.type == natus::nsl::shader_type::pixel_shader )
    {
        size_t num_color = 0 ;
        for( auto const& var : s.variables )
        {
            num_color += natus::nsl::is_color( var.binding ) ? 1 : 0 ;
        }

        // mrt requires extensions for glsl 130
        if( num_color > 1 && type == natus::nsl::api_type::gl3 )
        {
            text <<
                "#extension GL_ARB_separate_shader_objects : enable" << std::endl <<
                "#extension GL_ARB_explicit_attrib_location : enable" << std::endl << std::endl ;
        }
    }

    // 2. make prototypes declarations from function signatures
    // the prototype help with not having to sort funk definitions
    {
        text << "// Declarations // " << std::endl ;
        for( auto const& f : genable.frags )
        {
            text << this_t::map_variable_type( f.sig.return_type ) << " " ;
            text << f.sym_long.expand( "_" ) << " ( " ;
            for( auto const& a : f.sig.args )
            {
                text << this_t::map_variable_type( a.type ) + ", " ;
            }

            text.seekp( -2, std::ios_base::end ) ;
            text << " ) ; " << std::endl ;
        }
        text << std::endl ;
    }

    // 3. make all functions with replaced symbols
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

    // 4. make all glsl uniforms from shader variables
    {
        size_t num_color = 0 ;
        for( auto const& var : s.variables )
        {
            num_color += natus::nsl::is_color( var.binding ) ? 1 : 0 ;
        }

        size_t layloc_id = 0 ;
        text << "// Uniforms and in/out // " << std::endl ;
        for( auto const& v : s.variables )
        {
            if( v.fq == natus::nsl::flow_qualifier::out &&
                v.binding == natus::nsl::binding::position ) continue ;

            natus::ntd::string_t name = v.name ;
            natus::ntd::string_t const type_ = this_t::map_variable_type( v.type ) ;


            size_t const idx = natus::nsl::find_by( var_mappings, v.name, v.binding, v.fq, s.type ) ;
            if( idx != size_t( -1 ) )
            {
                name = var_mappings[ idx ].new_name ;
            }

            // do some regex replacements
            {
                //type_ = std::regex_replace( type_, std::regex( "tex([1-3]+)d" ), "sampler$1D" ) ;
            }

            natus::ntd::string_t layloc ;

            if( v.fq == natus::nsl::flow_qualifier::out &&
                s.type == natus::nsl::shader_type::pixel_shader &&
                num_color > 1 )
            {
                layloc = "layout( location = " + std::to_string( layloc_id++ ) + " ) " ;
            }

            natus::ntd::string_t const flow = v.fq == natus::nsl::flow_qualifier::global ?
                "uniform" : natus::nsl::to_string( v.fq ) ;

            text << layloc << flow << " " << type_ << " " << name << " ; " << std::endl ;
        }
        text << std::endl ;
    }

    // 5. insert main/shader from config
    {
        text << "// The shader // " << std::endl ;
        for( auto const& c : s.codes )
        {
            for( auto const& l : c.lines )
            {
                text << this_t::replace_types( l ) << std::endl ;
            }
        }
    }

    // 6. post over the code and replace all dependencies and in/out
    {
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

                if( v.fq == natus::nsl::flow_qualifier::in )
                    flow = "in." ;
                else if( v.fq == natus::nsl::flow_qualifier::out )
                    flow = "out." ;

                {
                    natus::ntd::string_t const repl = flow + v.old_name ;
                    size_t p0 = shd.find( repl, off ) ;
                    while( p0 != std::string::npos )
                    {
                        shd.replace( p0, repl.size(), v.new_name ) ;
                        p0 = shd.find( repl, p0 + 3 ) ;
                    }
                }
            }
        }

        // replace numbers
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
            shd = this_t::replace_buildin_symbols( std::move( shd ) ) ;
        }

        code.shader = shd ;
    }

    code.api = type ;

    //ret.emplace_back( std::move( code ) ) ;
    return std::move( code ) ;

}