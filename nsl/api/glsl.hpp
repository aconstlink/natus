#pragma once

#include "../typedefs.h"
#include "../parser_structs.hpp"
#include "../generator.hpp"
#include "../dependency_resolver.hpp"

#include <natus/ntd/vector.hpp>
#include <natus/ntd/map.hpp>

#include <sstream>
#include <regex>

namespace natus
{
    namespace nsl
    {
        namespace glsl
        {
            namespace detail
            {
                static const natus::ntd::vector< natus::ntd::string_t > glsl_types = {
                    "void", "bool", "int", "uint", "float", "double",
                    "vec2", "vec3", "vec4", "mat2", "mat3", "mat4",
                    "dvec2", "dvec3", "dvec4", "dmat2", "dmat3", "dmat4",
                    "sampler1D", "sampler2D", "sampler3D",
                    "isampler1D", "isampler2D", "isampler3D",
                    "uisampler1D", "uisampler2D", "uisampler3D",
                    "image1D", "image2D", "image3D",
                    "iimage1D", "iimage2D", "iimage3D",
                    "samplerCube", "imageCube",
                    "sampler2DRect", "image2DRect"
                    "samplerBuffer", "imageBuffer",
                    "isamplerBuffer", "iimageBuffer",
                    "sampler1DShadow", "sampler2DShadow"
                } ;

                bool_t is_type( natus::ntd::string_cref_t s ) noexcept
                {
                    for( auto const& t : glsl_types )
                    {
                        if( t == s ) return true ;
                    }
                    return false ;
                }
            }

            class function_signature_analyser
            {
                natus::ntd::string_t return_type ;
                natus::ntd::string_t name ;
                natus::ntd::vector< natus::ntd::string_t > arg_types ;

                natus::ntd::vector< natus::ntd::string_t > _tokens ;

            public: 

                function_signature_analyser( natus::ntd::vector< natus::ntd::string_t > const & tokens ) noexcept
                {
                    _tokens = tokens ;
                }

                natus::nsl::post_parse::library_t::fragment_t::signature_t process( void_t ) noexcept
                {
                    natus::nsl::post_parse::library_t::fragment_t::signature_t s ;

                    auto iter_open = std::find( _tokens.begin(), _tokens.end(), "(" ) ;
                    auto iter_close = std::find( _tokens.begin(), _tokens.end(), ")" ) ;

                    if( iter_open == _tokens.end() ) return std::move( s ) ;
                    if( iter_close == _tokens.end() ) return std::move( s ) ;

                    // arg types
                    {
                        auto iter = iter_open ;
                        auto end = ++iter_close ;
                        --iter_close ;

                        natus::ntd::vector< natus::ntd::string_t > arg_ ;
                        while( ++iter != end )
                        {
                            if( *iter == "," || iter == iter_close )
                            {
                                size_t const n = arg_types.size() ;

                                for( auto const & str : arg_ )
                                {
                                    if( detail::is_type( str ) )
                                    {
                                        arg_types.emplace_back( str ) ;
                                        break ;
                                    }
                                }

                                if( arg_types.size() == n )
                                    arg_types.emplace_back( "unknown_type" ) ;

                                arg_.clear() ;
                            }
                            arg_.emplace_back( *iter ) ;
                        }

                        // if arg list is empty, replace with void
                        if( arg_types.size() == 1 && arg_types[ 0 ] == "unknown_type" )
                            arg_types[ 0 ] = "void" ;
                    }

                    
                    // name and return type
                    {
                        if( std::distance( _tokens.begin(), iter_open ) >= 2 )
                        {
                            name = *--iter_open ;
                            return_type = *--iter_open ;
                        }
                        else return std::move( s ) ;
                    }

                    s.args = arg_types ;
                    s.name = name ;
                    s.return_type = return_type ;

                    return std::move( s ) ;
                }
            };

            // the glsl backend code generator
            class generator
            {
                natus_this_typedefs( generator ) ;

            private:

                natus::nsl::generateable_t _genable ;

                // not vertex attributes, only varyings
                natus::ntd::map< natus::ntd::string_t, natus::ntd::string_t > _ins ;
                natus::ntd::map< natus::ntd::string_t, natus::ntd::string_t > _outs ;

            public:

                generator( void_t ) noexcept {}
                generator( natus::nsl::generateable_rref_t gen ) noexcept : _genable( std::move( gen ) ) {}
                generator( this_cref_t rhv ) noexcept : _genable( rhv._genable ){}
                generator( this_rref_t rhv ) noexcept : _genable( std::move(rhv._genable) ){}
                ~generator( void_t ) {}

            public:

                natus::nsl::generated_code_t generate( void_t ) noexcept
                {
                    natus::nsl::generated_code_t ret ;
                    ret.rres = _genable ;

                    natus::nsl::generated_code_t::shader_t shd ;
                    {
                        for( auto const & s : _genable.config.shaders )
                        {
                            natus::nsl::shader_type s_type = natus::nsl::shader_type::unknown ;

                            if( s.type == "vertex_shader" )
                            {
                                s_type = natus::nsl::shader_type::vertex_shader ;
                            }
                            else if( s.type == "pixel_shader" ) 
                            {
                                s_type = natus::nsl::shader_type::pixel_shader ;
                            }

                            if( s_type == natus::nsl::shader_type::unknown )
                            {
                                natus::log::global_t::warning( "[glsl generator] : unknown shader type" ) ;
                                continue;
                            }
                            shd.type = s_type ;

                            for( auto const & v : s.variables )
                            {
                                natus::nsl::generated_code_t::variable_t var ;

                                {
                                    natus::ntd::string_t const flow = v.flow_qualifier.empty() ? "uniform" : v.flow_qualifier ;

                                    if( s_type == natus::nsl::shader_type::vertex_shader && v.flow_qualifier == "out" )
                                    {
                                        var.name = "var_" + v.name ;
                                        _outs[ v.name ] = var.name ;
                                    }
                                    else if( s_type == natus::nsl::shader_type::pixel_shader && v.flow_qualifier == "in" )
                                    {
                                        var.name = "var_" + v.name  ;
                                        _ins[ v.name ] = var.name ;
                                    }
                                    else
                                    {
                                        var.name = ( flow == "in" || flow == "out" ) ? flow + "_" + v.name : v.name ;
                                    }
                                }
                                
                                if( v.binding.empty() || v.flow_qualifier == "out" ) continue ;

                                var.binding = v.binding ;
                                shd.variables.emplace_back( std::move( var ) ) ;
                            }

                            shd.codes.emplace_back( this_t::generate( s, natus::nsl::api_type::es3 ) ) ;
                            shd.codes.emplace_back( this_t::generate( s, natus::nsl::api_type::gl3 ) ) ;

                            ret.shaders.emplace_back( std::move( shd ) ) ;
                        }
                    }
                    _ins.clear() ;
                    _outs.clear() ;

                    return std::move( ret ) ;
                }

                natus::nsl::generated_code_t::code_t generate( natus::nsl::post_parse::config_t::shader_cref_t s, natus::nsl::api_type const type ) noexcept
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

                    // 2. make prototypes declarations from function signatures
                    // the prototype help with not having to sort funk definitions
                    {
                        text << "// Declarations // " << std::endl ;
                        for( auto const & f : _genable.frags )
                        {
                            text << f.sig.return_type << " " ;
                            text << f.sym_long.expand( "_" ) << " ( " ;
                            for( auto const& a : f.sig.args ) text << a + ", " ;
                            text.seekp( -2, std::ios_base::end ) ;
                            text << " ) ; " << std::endl ;
                        }
                        text << std::endl ;
                    }

                    // 3. make all functions with replaced symbols
                    {
                        text << "// Definitions // " << std::endl ;
                        for( auto const & f : _genable.frags )
                        {
                            // start by replacing the function names' symbol itself
                            {
                                auto const & frag = f.fragments[ 0 ] ;

                                auto const p0 = frag.find( f.sig.name ) ;
                                if( p0 == std::string::npos ) continue ;
                                auto const p1 = frag.find_first_of( ' ', p0 ) ;
                                text << frag.substr( 0, p0 ) + f.sym_long.expand( "_" ) + frag.substr( p1 ) ;
                            }

                            // then lets go over every symbol in the code
                            for( size_t i=1; i<f.fragments.size(); ++i )
                            {
                                text << f.fragments[i] ;
                            }
                            text << std::endl ;
                        }
                        text << std::endl ;
                    }

                    // 4. make all glsl uniforms from shader variables
                    {
                        text << "// Uniforms and in/out // " << std::endl ;
                        for( auto const & v : s.variables )
                        {
                            if( v.flow_qualifier == "out" && v.binding == "position" ) continue ;

                            natus::ntd::string_t const flow = v.flow_qualifier.empty() ? "uniform" : v.flow_qualifier ;
                            natus::ntd::string_t name = v.name ;
                            natus::ntd::string_t type_ = v.type ;

                            if( flow == "in" || flow == "out" )
                                name = flow + "_" + name ;

                            if( flow == "in" && s.type == "pixel_shader" )
                                name = _ins[ v.name ] ;
                            else if( flow == "out" && s.type == "vertex_shader" )
                                name = _outs[ v.name ] ;
                            

                            // do some regex replacements
                            {
                                type_ = std::regex_replace( type_, std::regex( "tex([1-3]+)d" ), "sampler$1D" ) ;
                            }

                            text << flow << " " << type_ << " " << name << " ; " << std::endl ;
                        }
                        text << std::endl ;
                    }

                    // 5. insert main/shader from config
                    {
                        text << "// The shader // " << std::endl ;
                        for( auto const & c : s.codes )
                        {
                            if( c.versions[ 0 ] != "glsl" ) continue ;
                                
                            for( auto const & l : c.lines )
                            {
                                text << l << std::endl ;
                            }
                        }
                    }

                        
                    // 6. post over the code and replace all dependencies and in/out
                    {
                        auto shd = text.str() ;

                        // variable dependencies
                        {
                            for( auto const& v : _genable.vars )
                            {
                                size_t const p0 = shd.find( v.sym_long.expand() ) ;
                                if( p0 == std::string::npos ) continue ;
                                size_t const p1 = shd.find_first_of( " ", p0 ) ;

                                shd = shd.substr( 0, p0 ) + v.value + shd.substr( p1 ) ;
                            }
                        }

                        // fragment dependencies
                        {
                            for( auto const& f : _genable.frags )
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

                        // repace in/out
                        {
                            {
                                natus::ntd::string_t const repl = s.type != "pixel_shader" ? "in_" : "var_" ;

                                size_t p0 = shd.find( "in." ) ;
                                while( p0 != std::string::npos )
                                {
                                    shd.replace( p0, 3, repl ) ;
                                    p0 = shd.find( "in.", p0 + 3 ) ;
                                }
                            }
                            {
                                natus::ntd::string_t const repl = s.type != "vertex_shader" ? "out_" : "var_" ;

                                size_t p0 = shd.find( "out." ) ;
                                while( p0 != std::string::npos )
                                {
                                    shd.replace( p0, 4, repl ) ;
                                    p0 = shd.find( "out.", p0 + 4 ) ;
                                }
                            }
                        }
                            
                        code.shader = shd ;
                    }

                    code.api = type ;

                    //ret.emplace_back( std::move( code ) ) ;
                    return std::move( code ) ;
                    
                }
            };
            natus_typedef( generator ) ;
        }
    }
}